/*
 * GCC-PLUGINS - A collection of simple GCC plugins
 * Copyright (C) 2011 Daniel Marjamäki
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


/**
 * Simple gcc plugin that outputs AST to stdout in XML format.
 */

#include "gcc-plugin.h"
#include "parse-tree.h"
#include <stdio.h>

#define PLUGIN_NAME "dump-tree-xml"

int plugin_is_GPL_compatible;

static void print_tree_node(tree t, int indent)
{
    // indentation..
    int i;
    for (i = 1; i <= indent; ++i)
        printf("  ");

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
    }

    // Integer constant..
    else if (code == INTEGER_CST) {
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

    else {
        // print tree_code_name for this tree node..
        printf("%s\n", tree_code_name[(int)code]);
    }
}

/**
 * Plugin callback that is called in the GCC finish_function. The
 * given gcc_data is the tree for a function.
 */
static void pre_generic(void *gcc_data, void *user_data)
{
    printf(PLUGIN_NAME ":pre_generic\n");

    // Print AST
    tree fndecl = gcc_data;

    if (TREE_CODE(fndecl) == FUNCTION_DECL) {
        tree id = DECL_NAME(fndecl);
        const char *fnname = id ? IDENTIFIER_POINTER(id) : "<unnamed>";
        printf("%s %s\n", tree_code_name[(int)FUNCTION_DECL], fnname);

        // Print function body..
        tree fnbody = DECL_SAVED_TREE(fndecl);
        if (TREE_CODE(fnbody) == BIND_EXPR) {
            // second operand of BIND_EXPR
            tree t = TREE_OPERAND(fnbody, 1);

            // use the utility function "parse_tree" to parse
            // through the tree recursively  (../include/parse-tree.h)
            parse_tree(t, print_tree_node, 1);
        }
    }
}

void tree_check_failed (const_tree a, const char *b, int c, const char *d, ...)
{
    printf(PLUGIN_NAME ":tree_check_failed\n");
    exit(EXIT_FAILURE);
}

void tree_class_check_failed (const_tree a, const enum tree_code_class b,
				     const char *c, int d, const char *e)
{
    printf(PLUGIN_NAME ":tree_class_check_failed\n");
    exit(EXIT_FAILURE);
}

void tree_operand_check_failed (int a, const_tree b,
				       const char *c, int d, const char *e)
{
    printf(PLUGIN_NAME ":tree_operand_check_failed\n");
    exit(EXIT_FAILURE);
}

void tree_contains_struct_check_failed (const_tree a,
					       const enum tree_node_structure_enum b,
					       const char *c, int d, const char *e)
{
    printf(PLUGIN_NAME ":tree_contains_struct_check_failed\n");
    exit(EXIT_FAILURE);
}





int
plugin_init (struct plugin_name_args *plugin_info,
             struct plugin_gcc_version *version)
{
    printf(PLUGIN_NAME "\n");

    register_callback(plugin_info->base_name,
                      PLUGIN_PRE_GENERICIZE,
                      &pre_generic,
                      0);

    return 0;
}

