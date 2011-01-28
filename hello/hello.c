/* Just print "hello" */

#include "gcc-plugin.h"
#include <stdio.h>

int plugin_is_GPL_compatible;

int
plugin_init(struct plugin_name_args *plugin_info,
            struct plugin_gcc_version *version)
{
    printf("hello\n");
    return 0;
}

