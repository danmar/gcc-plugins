
#include "gcc-plugin.h"
#include "tree.h"
#include "cp/cp-tree.h"
#include <stdio.h>

int plugin_is_GPL_compatible;

static void print_expr(tree t, int indent)
{
    int i;
    enum tree_code code;

    // null => return
    if (t == 0)
        return;

    // indentation..
    for (i = 1; i <= indent; ++i)
        printf(" ");

    code = TREE_CODE(t);
    printf("%s\n", tree_code_name[(int)code]);

    if (code == RESULT_DECL || code == PARM_DECL)
        return;

    // print first expression operand
    tree operand = TREE_OPERAND(t, 0);
    print_expr(operand, indent + 2);

    // print second expression operand
    if (operand && code != RETURN_EXPR)
        print_expr(TREE_OPERAND(t, 1), indent + 2);
}

void pre_generic(void *gcc_data, void *user_data)
{
    printf("myplugin1:pre_generic\n");

    // Print AST
    tree decl = gcc_data;
    enum tree_code code = TREE_CODE(decl);

    if (TREE_CODE(decl) == FUNCTION_DECL) {
        tree id = DECL_NAME(decl);
        const char *name = id ? IDENTIFIER_POINTER(id) : "<unnamed>";
        printf("%s %s\n", tree_code_name[(int)FUNCTION_DECL], name);

        tree t = DECL_SAVED_TREE(decl);
        if (TREE_CODE(t) == BIND_EXPR) {
            print_expr(TREE_OPERAND(t, 1), 4);
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

