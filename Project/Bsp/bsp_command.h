#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include "def.h"

struct cmd_table
{
    char *name; /* command name */
    int maxargs; /* maximum number of argnuments */
    int repeatable; /* autorepeat allowed */
    int (*cmd)(struct cmd_table *,int,int,char *const []);
    char *usage; /* usage message */
    char *help; /* help message */
    /* auto completion on the arguments */
    int (*complete)(int argc,char *const argv[],char last_char, int maxv, char *cmdv[]);
};

typedef struct cmd_table cmd_tbl_t;

cmd_tbl_t *find_cmd(const char *cmd);
cmd_tbl_t *find_cmd_tbl(const char *cmd,cmd_tbl_t *table,int table_len);

int cmd_help(cmd_tbl_t *cmd_start,int cmd_items, cmd_tbl_t * cmdtp, int
	      flag, int argc, char * const argv[]);

int cmd_usage(const cmd_tbl_t *cmdtp);


int cmd_auto_complete(const char *const prompt, char *buf, int *np, int *colp);

enum command_ret_t {
	CMD_RET_SUCCESS,	/* 0 = Success */
	CMD_RET_FAILURE,	/* 1 = Failure */
	CMD_RET_USAGE = -1,	/* Failure, please report 'usage' error */
};

#define CMD_FLAG_REPEAT		0x0001	/* repeat last command		*/

/*
*@brief process a command
*@params
*
*@return
*   sucess = 0 fail = 1
*/
int cmd_process(int flag,int argc,char * const argv[],int *repeatable);

extern cmd_tbl_t  _cmd_start;
extern cmd_tbl_t  _cmd_end;

#if 0
#define Struct_Section(level)  __attribute__((unused, section("command_section."level), \
		aligned(4)))
#endif
#if 0
		/* arm compiler */
#if defined(__CC_ARM)
    #define DEF_SECTION(x)									__attribute__((section(x)))
		#define DEF_SecAligned(x)								__attribute__((section(x),aligned(4)))
    #define DEF_USED                        __attribute__((used))
    #define DEF_UNUSED                      __attribute__((unused))
    #define DEF_ALIGN(n)                    __attribute__((aligned(n)))
    #define DEF_WEAK                        __attribute__((weak))
    #define DEF_INLINE                      static __inline
#elif defined (__IAR_SYSTEMS_ICC__)     /* for IAR Compiler */
    #include <stdarg.h>
    #define DEF_SECTION(x)                  @ x
    #define DEF_UNUSED
    #define DEF_USED                     __root
    #define DEF_PRAGMA(x)                   _Pragma(#x)
    #define DEF_ALIGN(n)                    PRAGMA(data_alignment=n)
    #define DEF_WEAK                     __weak
    #define DEF_INLINE                   static inline
#endif
#endif

#define CMD_MKENT_COMPLETE(name,maxargs,rep,cmd,usage,help,comp) \
	{#name, maxargs, rep, cmd, usage, help, comp}

#if 0
#define CMD_COMPLETE(level,name,maxargs,rep,cmd,usage,help,comp) \
	cmd_tbl_t _cmd_##name Struct_Section(level) = \
		CMD_MKENT_COMPLETE(name,maxargs,rep,cmd,usage,help,comp)
#endif

#define CMD_COMPLETE(level,name,maxargs,rep,cmd,usage,help,comp) \
	  DEF_USED cmd_tbl_t _cmd_##name  DEF_SECTION(".board_cmd."level)  = \
		CMD_MKENT_COMPLETE(name,maxargs,rep,cmd,usage,help,comp)

#define CMD_INIT(name,maxargs,rep,cmd,usage,help) \
	CMD_COMPLETE("2",name,maxargs,rep,cmd,usage,help,NULL)

#endif // COMMAND_H
