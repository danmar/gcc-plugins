
/**
 * Simple gcc plugin that search for such null pointers:
 *   if (ab != 0 || ab->a != 0) ..
 */

#include "gcc-plugin.h"
#include "parse-tree.h"
#include <stdio.h>

int plugin_is_GPL_compatible;

static void check_tree_node(tree t, int indent)
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
 * Callback that is called in the finish_function. The
 * given gcc_data is the tree for a function.
 */
static void pre_generic(void *gcc_data, void *user_data)
{
    // Print AST
    tree fndecl = gcc_data;
    enum tree_code code = TREE_CODE(fndecl);

    if (TREE_CODE(fndecl) == FUNCTION_DECL) {
        // Print function body..
        tree fnbody = DECL_SAVED_TREE(fndecl);
        if (TREE_CODE(fnbody) == BIND_EXPR) {
            parse_tree(TREE_OPERAND(fnbody, 1), check_tree_node, 0);
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

