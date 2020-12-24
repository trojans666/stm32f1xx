#include <stdio.h>
#include "net_ipaddr.h"

static int isdigit(int ch)
{
    return (unsigned int )(ch - '0') < 10u;
}

static int isspace(int ch)
{
    switch(ch)
    {
    case ' ':
    case '\n':
    case '\f':
    case '\r':
    case '\t':
    case '\v':
        return 1;
    default:
        return 0;
    }
}

static int isxdigit(int ch)
{
    if(isdigit(ch) ||
            ((ch >= 'a') && (ch <= 'f'))||
            ((ch >= 'A') && (ch <= 'F')))
    {
        return 1;
    }
    return 0;
}

static int islower(int ch)
{
   if((ch >= 'a') && (ch <= 'z'))
	   return 1;

   return 0;
}

int inet_aton(const char *cp,ip4_addr_t *addr)
{
    unsigned int val;
    unsigned char base;
    char c;
    unsigned int parts[4];
    unsigned int *pp = parts;

    c = *cp;
    for (;;)
    {
        /*
         * Collect number up to ``.''.
         * Values are specified as for C:
         * 0x=hex, 0=octal, 1-9=decimal.
         */
        if (!isdigit(c))
        {
            return 0;
        }
        val = 0;
        base = 10;
        if (c == '0')
        {
            c = *++cp;
            if (c == 'x' || c == 'X')
            {
                base = 16;
                c = *++cp;
            }
            else
            {
                base = 8;
            }
        }
        for (;;)
        {
            if (isdigit(c))
            {
                val = (val * base) + (unsigned int) (c - '0');
                c = *++cp;
            }
            else if (base == 16 && isxdigit(c))
            {
                val = (val << 4) | (unsigned int) (c + 10 - (islower(c) ? 'a' : 'A'));
                c = *++cp;
            }
            else
            {
                break;
            }
        }
        if (c == '.')
        {
            /*
             * Internet format:
             *  a.b.c.d
             *  a.b.c   (with c treated as 16 bits)
             *  a.b (with b treated as 24 bits)
             */
            if (pp >= parts + 3)
            {
                return 0;
            }
            *pp++ = val;
            c = *++cp;
        }
        else
        {
            break;
        }
    }
    /*
     * Check for trailing characters.
     */
    if (c != '\0' && !isspace(c))
    {
        return 0;
    }
    /*
     * Concoct the address according to
     * the number of parts specified.
     */
    switch (pp - parts + 1)
    {

    case 0:
        return 0; /* initial nondigit */

    case 1: /* a -- 32 bits */
        break;

    case 2: /* a.b -- 8.24 bits */
        if (val > 0xffffffUL)
        {
            return 0;
        }
        if (parts[0] > 0xff)
        {
            return 0;
        }
        val |= parts[0] << 24;
        break;

    case 3: /* a.b.c -- 8.8.16 bits */
        if (val > 0xffff)
        {
            return 0;
        }
        if ((parts[0] > 0xff) || (parts[1] > 0xff))
        {
            return 0;
        }
        val |= (parts[0] << 24) | (parts[1] << 16);
        break;

    case 4: /* a.b.c.d -- 8.8.8.8 bits */
        if (val > 0xff)
        {
            return 0;
        }
        if ((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff))
        {
            return 0;
        }
        val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
        break;
    default:

        break;
    }
    if (addr)
    {
        *addr = htonl(val);
    }
    return 1;
}

static char *inet_ntoa_r(const ip4_addr_t *addr,char *buf,int buflen)
{
    unsigned int s_addr;
    char inv[3];
    char *rp;
    unsigned char *ap;
    unsigned char rem;
    unsigned char n;
    unsigned char i;
    int len = 0;

    s_addr = *addr;

    rp = buf;
    ap = (unsigned char *) &s_addr;
    for (n = 0; n < 4; n++)
    {
        i = 0;
        do
        {
            rem = *ap % (unsigned char) 10;
            *ap /= (unsigned char) 10;
            inv[i++] = (char) ('0' + rem);
        }
        while (*ap);
        while (i--)
        {
            if (len++ >= buflen)
            {
                return NULL;
            }
            *rp++ = inv[i];
        }
        if (len++ >= buflen)
        {
            return NULL;
        }
        *rp++ = '.';
        ap++;
    }
    *--rp = 0;
    return buf;
}

char *inet_ntoa(const ip4_addr_t *addr)
{
    static char str[16];
    return inet_ntoa_r(addr,str,sizeof(str));
}

in_addr_t inet_addr(const char *cp)
{
    ip4_addr_t val;
    if(inet_aton(cp,&val));
        return val;

    return (unsigned int)(0xffffffffUL);
}
