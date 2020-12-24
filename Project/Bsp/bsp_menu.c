#include <ctype.h>
#include <string.h>
#include "dev_config.h"
#include "command.h"
#include "console.h"
#include "menu.h"

static const char erase_seq[] = "\b \b";		/* erase sequence	*/
static const char tab_seq[] = "        ";		/* used to expand TABs	*/


/* prompt prefix */
static char console_buffer[MENU_CONSOLE_BUFFER + 1];

static char * delete_char (char *buffer, char *p, int *colp, int *np, int plen)
{
	char *s;

	if (*np == 0) {
		return (p);
	}

	if (*(--p) == '\t') {			/* will retype the whole line	*/
		while (*colp > plen) {
			console_puts (erase_seq);
			(*colp)--;
		}
		for (s=buffer; s<p; ++s) {
			if (*s == '\t') {
				console_puts (tab_seq+((*colp) & 07));
				*colp += 8 - ((*colp) & 07);
			} else {
				++(*colp);
				console_putc (*s);
			}
		}
	} else {
		console_puts (erase_seq);
		(*colp)--;
	}
	(*np)--;
	return (p);
}

int readline_into_buffer(const char *const prompt,char *buffer, int timeout)
{
    char *p = buffer;
    char *p_buf = p;
    int n = 0;
    int plen = 0;
    int col;
    char c;

    /* print prompt */
    if(prompt)
    {
        plen = strlen(prompt);
        console_puts(prompt);
    }
    col = plen;

    for(;;)
    {
        c = console_getc();
        switch(c)
        {
        case '\r':
        case '\n':
            *p = '\0';
            console_puts("\r\n");
            return (p - p_buf);
        case '\0':
            continue;
        case 0x03:      /* ^C break*/
            p_buf[0] = '\0';
            return (-1);
        case 0x15:      /* ^U erase line */
            while(col > plen)
            {
                console_puts(erase_seq);
                --col;
            }
            p = p_buf;
            n = 0;
            continue;
        case 0x17:      /* ^W erase word */
            p = delete_char(p_buf,p,&col,&n,plen);
            while((n > 0) && (*p != ' '))
            {
                p = delete_char(p_buf,p,&col,&n,plen);
            }
            continue;
        case 0x08:  /* ^H backsapce */
        case 0x7F:  /* DEL backspace */
            p = delete_char(p_buf,p,&col,&n,plen);
            continue;
        default:
            /* must be a normal character then */
            if(n < MENU_CONSOLE_BUFFER - 2)
            {
                if(c == '\t') /* Tab */
                {
                    *p = '\0';
                    if(cmd_auto_complete(prompt,console_buffer,&n,&col))
                    {
                        p = p_buf + n; /* reset */
                        continue;
                    }

                    console_puts(tab_seq + (col & 07));
                    col += 8 - (col & 07);
                }
                else
                {
                    ++col; /* echo input */
                    console_putc(c);
                }
                *p++ = c;
                ++n;
            }
            else   /* buffer is full */
            {
                console_putc('\a');
            }
        }
    }
}

int readline(const char *const prompt)
{
    console_buffer[0] = '\0';
    return readline_into_buffer(prompt,console_buffer,0);
}


int parse_line (char *line, char *argv[])
{
	int nargs = 0;

    console_printf ("parse_line: \"%s\"\n", line);

	while (nargs < MENU_CONSOLE_BUFFER) {

		/* skip any white space */
		while (isblank(*line))
			++line;

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;

		console_printf ("parse_line: nargs=%d\n", nargs);

			return (nargs);
		}

		argv[nargs++] = line;	/* begin of argument string	*/

		/* find end of string */
		while (*line && !isblank(*line))
			++line;

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;

		console_printf ("parse_line: nargs=%d\n", nargs);

			return (nargs);
		}

		*line++ = '\0';		/* terminate current arg	 */
	}

	console_printf ("** Too many args (max. %d) **\n", MENU_CONSOLE_BUFFER);

	console_printf ("parse_line: nargs=%d\n", nargs);

	return (nargs);
}

#if 1
static void process_macros (const char *input, char *output)
{
	char c, prev;
	const char *varname_start = NULL;
	int inputcnt = strlen (input);
	int outputcnt = MENU_CONSOLE_BUFFER;
	int state = 0;		/* 0 = waiting for '$'  */

	/* 1 = waiting for '(' or '{' */
	/* 2 = waiting for ')' or '}' */
	/* 3 = waiting for '''  */

	char *output_start = output;

	console_printf ("[PROCESS_MACROS] INPUT len %d: \"%s\"\n", strlen (input),
		input);


	prev = '\0';		/* previous character   */

	while (inputcnt && outputcnt) {
		c = *input++;
		inputcnt--;

		if (state != 3) {
			/* remove one level of escape characters */
			if ((c == '\\') && (prev != '\\')) {
				if (inputcnt-- == 0)
					break;
				prev = c;
				c = *input++;
			}
		}

		switch (state) {
		case 0:	/* Waiting for (unescaped) $    */
			if ((c == '\'') && (prev != '\\')) {
				state = 3;
				break;
			}
			if ((c == '$') && (prev != '\\')) {
				state++;
			} else {
				*(output++) = c;
				outputcnt--;
			}
			break;
		case 1:	/* Waiting for (        */
			if (c == '(' || c == '{') {
				state++;
				varname_start = input;
			} else {
				state = 0;
				*(output++) = '$';
				outputcnt--;

				if (outputcnt) {
					*(output++) = c;
					outputcnt--;
				}
			}
			break;
		case 2:	/* Waiting for )        */
			if (c == ')' || c == '}') {
				int i;
				char envname[MENU_CONSOLE_BUFFER];
				char *envval;
				int envcnt = input - varname_start - 1;	/* Varname # of chars */

				/* Get the varname */
				for (i = 0; i < envcnt; i++) {
					envname[i] = varname_start[i];
				}
				envname[i] = 0;

				/* Get its value */
				envval = NULL; //getenv (envname);

				/* Copy into the line if it exists */
				if (envval != NULL)
					while ((*envval) && outputcnt) {
						*(output++) = *(envval++);
						outputcnt--;
					}
				/* Look for another '$' */
				state = 0;
			}
			break;
		case 3:	/* Waiting for '        */
			if ((c == '\'') && (prev != '\\')) {
				state = 0;
			} else {
				*(output++) = c;
				outputcnt--;
			}
			break;
		}
		prev = c;
	}

	if (outputcnt)
		*output = 0;
	else
		*(output - 1) = 0;


	console_printf ("[PROCESS_MACROS] OUTPUT len %d: \"%s\"\n",
		strlen (output_start), output_start);

}
#endif

static int builtin_run_command(const char *cmd,int flag)
{
    char cmdbuf[MENU_CONSOLE_BUFFER];	/* working copy of cmd		*/
	char *token;			/* start of token in cmdbuf	*/
	char *sep;			/* end of token (separator) in cmdbuf */
	char finaltoken[MENU_CONSOLE_BUFFER];
	char *str = cmdbuf;
	char *argv[MENU_ARGS_MAX + 1];	/* NULL terminated	*/
	int argc, inquotes;
	int repeatable = 1;
	int rc = 0;

	//clear_ctrlc();		/* forget any previous Control C */

	if (!cmd || !*cmd) {
		return -1;	/* empty command */
	}

	if (strlen(cmd) >= MENU_CONSOLE_BUFFER) {
		console_puts ("## Command too long!\n");
		return -1;
	}

	strcpy (cmdbuf, cmd);

	/* Process separators and check for invalid
	 * repeatable commands
	 */

	while (*str) {

		/*
		 * Find separator, or string end
		 * Allow simple escape of ';' by writing "\;"
		 */
		for (inquotes = 0, sep = str; *sep; sep++) {
			if ((*sep=='\'') &&
			    (*(sep-1) != '\\'))
				inquotes=!inquotes;

			if (!inquotes &&
			    (*sep == ';') &&	/* separator		*/
			    ( sep != str) &&	/* past string start	*/
			    (*(sep-1) != '\\'))	/* and NOT escaped	*/
				break;
		}

		/*
		 * Limit the token to data between separators
		 */
		token = str;
		if (*sep) {
			str = sep + 1;	/* start of command for next pass */
			*sep = '\0';
		}
		else
			str = sep;	/* no more commands for next pass */

		console_printf ("token: \"%s\"\n", token);


		/* find macros in this token and replace them */
		process_macros (token, finaltoken);

		/* Extract arguments */
		if ((argc = parse_line (finaltoken, argv)) == 0) {
			rc = -1;	/* no command at all */
			continue;
		}

		rc = cmd_process(flag, argc, argv, &repeatable);

		/* Did the user stop this? */
		//if (had_ctrlc ())
			//return -1;	/* if stopped then not repeatable */
	}

	return rc ? rc : repeatable;
}

int run_command(const char *cmd,int flag)
{
    if(builtin_run_command(cmd,flag) == -1)
        return 1;
    return 0;
}

void main_loop(void)
{
    int len;
    int flag = 0;
    int rc = 1;

    static char lastcommand[MENU_CONSOLE_BUFFER] = "";
    for(;;)
    {
        len = readline(MENU_PROMPT_NAME);
        if(len > 0)
        {
            strcpy(lastcommand,console_buffer);
        }
        else if(len == 0)
        {
            flag |= CMD_FLAG_REPEAT;
        }

        if(len == -1)
        {
            console_puts("<INTERRUPT>\n");
        }
        else
        {
            rc = run_command(lastcommand,flag);
        }
        if(rc <= 0)
        {
            lastcommand[0] = 0;
        }
    }
}
