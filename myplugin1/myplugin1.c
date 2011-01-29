
/**
 * Simple gcc plugin that outputs internal data tree to stdout.
 */

#include "gcc-plugin.h"
#include "tree.h"
#include "cp/cp-tree.h"
#include "tree-iterator.h"
#include <stdio.h>

int plugin_is_GPL_compatible;

/**
 * Print given tree recursively
 */
static void print_tree(tree t, int indent)
{
    // null => return
    if (t == 0)
        return;

    // indentation..
    int i;
    for (i = 1; i <= indent; ++i)
        printf(" ");

    enum tree_code code = TREE_CODE(t);

    // Declarations..
    if (code == RESULT_DECL || 
        code == PARM_DECL || 
        code == LABEL_DECL || 
        code == VAR_DECL ||
        code == FUNCTION_DECL) {

        // Get DECL_NAME for this declaration
        tree id = DECL_NAME(t);

        // print name of declaration..
        const char *name = id ? IDENTIFIER_POINTER(id) : "<unnamed>";
        printf("%s : %s\n", tree_code_name[(int)code], name);

        // return..
        return;
    }

    // Integer constant..
    if (code == INTEGER_CST) {
        // value of integer constant is:
        // (HIGH << HOST_BITS_PER_WIDE_INT) + LOW

        if (TREE_INT_CST_HIGH(t)) {
            printf("%s : high=0x%X low=0x%X\n", 
                   tree_code_name[(int)code],
                   TREE_INT_CST_HIGH(t),
                   TREE_INT_CST_LOW(t));
        } else {
            printf("%s : %i\n", 
                   tree_code_name[(int)code],
                   TREE_INT_CST_LOW(t));
        }
        return;
    }

    // print tree_code_name for this tree node..
    printf("%s\n", tree_code_name[(int)code]);

    // Statement list..
    if (code == STATEMENT_LIST) {
        tree_stmt_iterator it;
        for (it = tsi_start(t); !tsi_end_p(it); tsi_next(&it)) {
            print_tree(tsi_stmt(it), indent + 2);
        }
        return;
    }

    // print first expression operand
    print_tree(TREE_OPERAND(t, 0), indent + 2);

    // print second expression operand
    if (code != RETURN_EXPR && 
        code != LABEL_EXPR &&
        code != GOTO_EXPR &&
        code != NOP_EXPR &&
        code != DECL_EXPR &&
        code != ADDR_EXPR &&
        code != INDIRECT_REF &&
        code != COMPONENT_REF)
        print_tree(TREE_OPERAND(t, 1), indent + 2);
}

/**
 * Callback that is called in the finish_function. The
 * given gcc_data is the tree for a function.
 */
static void pre_generic(void *gcc_data, void *user_data)
{
    printf("myplugin1:pre_generic\n");

    // Print AST
    tree decl = gcc_data;

    enum tree_code code = TREE_CODE(decl);

    if (TREE_CODE(decl) == FUNCTION_DECL) {
        tree id = DECL_NAME(decl);
        const char *name = id ? IDENTIFIER_POINTER(id) : "<unnamed>";
        printf("%s %s\n", tree_code_name[(int)FUNCTION_DECL], name);

        // Print function body..
        tree fnbody = DECL_SAVED_TREE(decl);
        if (TREE_CODE(fnbody) == BIND_EXPR) {
            print_tree(TREE_OPERAND(fnbody, 1), 4);
        }
    }
}

int
plugin_init (struct plugin_name_args *plugin_info,
             struct plugin_gcc_version *version)
{
    printf("myplugin1\n");

    register_callback(plugin_info->base_name,
                      PLUGIN_PRE_GENERICIZE,
                      &pre_generic,
                      0);

    return 0;
}

