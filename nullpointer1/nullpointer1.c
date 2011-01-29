
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

// get child tree
// path is a series of '0' and '1'
static tree getchildtree(tree parent, const char pathstr[])
{
    const char *p = pathstr;
    for (p = pathstr; parent && *p; ++p) {
        if (*p == '0' || *p == '1')
            parent = TREE_OPERAND(parent, *p - '0');
    }
    return parent;
}

// check if child is a INTEGER_CST with given value
static int ischildvalue(tree parent, const char pathstr[], int value)
{
    parent = getchildtree(parent, pathstr);
    if (parent && TREE_CODE(parent) == INTEGER_CST) {
        if (TREE_INT_CST_HIGH(parent) == 0 && TREE_INT_CST_LOW(parent) == value) {
            return 1;
        }
    }
    return 0;
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

    if (code == TRUTH_ORIF_EXPR) {
        // first truth expression: 'p != 0'?
        if (TREE_CODE(getchildtree(t, "0")) == NE_EXPR &&
            TREE_CODE(getchildtree(t, "00")) == PARM_DECL &&
            ischildvalue(t, "01", 0)) {
            tree var = DECL_NAME(getchildtree(t,"00"));

            // second truth expression dereferences p
            if (TREE_CODE(getchildtree(t, "10")) == COMPONENT_REF &&
                TREE_CODE(getchildtree(t, "100")) == INDIRECT_REF &&
                TREE_CODE(getchildtree(t, "1000")) == PARM_DECL &&
                DECL_NAME(getchildtree(t, "1000")) == var) {
                printf("possible null pointer dereference\n");
            }
        }
    }

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

