
/**
 * Simple gcc plugin that search for such null pointers:
 *   if (ab != 0 || ab->a != 0) ..
 */

#include "gcc-plugin.h"
#include "tree.h"
#include "cp/cp-tree.h"
#include "tree-iterator.h"
#include <stdio.h>

int plugin_is_GPL_compatible;

// Match tree_code for the given "tree" 
static int is_tree_code(tree t, enum tree_code tc)
{
    return (t && (TREE_CODE(t) == tc)) ? 1 : 0;
}

// Is the given "tree" a PARM_DECL or VAR_DECL?
static int is_declaration(tree t)
{
    return (is_tree_code(t, PARM_DECL) || is_tree_code(t, VAR_DECL)) ? 1 : 0;
}

// Is the given "tree" a INTEGER_CST with the given value?
static int is_value(tree t, int value)
{
    return (is_tree_code(t, INTEGER_CST) && TREE_INT_CST_HIGH(t) == 0 && TREE_INT_CST_LOW(t) == value) ? 1 : 0;
}

static void check_tree_node(tree t)
{
    if (TREE_CODE(t) == TRUTH_ORIF_EXPR) {
        tree var = 0;

        // first truth expression: p != 0
        {
            tree t0 = TREE_OPERAND(t,0);

            if (is_tree_code(t0, NE_EXPR) && 
                is_declaration(TREE_OPERAND(t0,0)) && 
                is_value(TREE_OPERAND(t0,1), 0)) {
                var = DECL_NAME(TREE_OPERAND(t0,0));
            }
        }

        if (!var)
            return;

        // second truth expression: dereference p
        {
            tree t1 = TREE_OPERAND(t,1);
            tree t10 = t1 ? TREE_OPERAND(t1,0) : 0;
            tree t100 = t10 ? TREE_OPERAND(t10,0) : 0;
            tree t1000 = t100 ? TREE_OPERAND(t100,0) : 0;

            if (is_tree_code(t10, COMPONENT_REF) &&
                is_tree_code(t100, INDIRECT_REF) &&
                is_declaration(t1000) &&
                DECL_NAME(t1000) == var) {
                warning_at(EXPR_LOCATION(t10), 0, "possible null pointer dereference if subcondition is reachable");
            }
        }
    }
}


/**
 * Print given tree recursively
 */
static void parse_tree(tree t)
{
    // null => return
    if (t == 0)
        return;

    enum tree_code code = TREE_CODE(t);

    // Declarations..
    if (code == RESULT_DECL || 
        code == PARM_DECL || 
        code == LABEL_DECL || 
        code == VAR_DECL ||
        code == FUNCTION_DECL) {
        return;
    }

    // Integer constant..
    if (code == INTEGER_CST) {
        return;
    }

    // Statement list..
    if (code == STATEMENT_LIST) {
        tree_stmt_iterator it;
        for (it = tsi_start(t); !tsi_end_p(it); tsi_next(&it)) {
            parse_tree(tsi_stmt(it));
        }
        return;
    }

    // Check tree node..
    check_tree_node(t);

    // print first expression operand
    parse_tree(TREE_OPERAND(t, 0));

    // print second expression operand
    if (code != RETURN_EXPR && 
        code != LABEL_EXPR &&
        code != GOTO_EXPR &&
        code != NOP_EXPR &&
        code != DECL_EXPR &&
        code != ADDR_EXPR && 
        code != INDIRECT_REF &&
        code != COMPONENT_REF)
        parse_tree(TREE_OPERAND(t, 1));
}

/**
 * Callback that is called in the finish_function. The
 * given gcc_data is the tree for a function.
 */
static void pre_generic(void *gcc_data, void *user_data)
{
    // Print AST
    tree decl = gcc_data;
    enum tree_code code = TREE_CODE(decl);

    if (TREE_CODE(decl) == FUNCTION_DECL) {
        // Print function body..
        tree fnbody = DECL_SAVED_TREE(decl);
        if (TREE_CODE(fnbody) == BIND_EXPR) {
            parse_tree(TREE_OPERAND(fnbody, 1));
        }
    }
}

int
plugin_init (struct plugin_name_args *plugin_info,
             struct plugin_gcc_version *version)
{
    register_callback(plugin_info->base_name,
                      PLUGIN_PRE_GENERICIZE,
                      &pre_generic,
                      0);

    return 0;
}

