#include <stdio.h>
#include "dev_config.h"

#include "command.h"
#include "console.h"

int do_helloworld(cmd_tbl_t *cmdtp,int flag,int argc,char *const argv[])
{
    console_printf("hello world \n");
    return 0;
}

CMD_INIT(test,MENU_ARGS_MAX,1,do_helloworld,"print hello world","test");

int do_hello(cmd_tbl_t *cmdtp,int flag,int argc,char *const argv[])
{
    console_printf("1111111\n");
    return 0;
}
CMD_INIT(tt,MENU_ARGS_MAX,1,do_hello,"11","11");
