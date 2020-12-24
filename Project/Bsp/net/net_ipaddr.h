#ifndef NET_IPADDR_H
#define NET_IPADDR_H

#define NET_HTONS(x) ((((x) & 0x00ffUL) << 8) | (((x) & 0xff00UL) >> 8))
#define NET_NTOHS(x) NET_HTONS(x)
#define NET_HTONL(x) ((((x) & 0x000000ffUL) << 24) | \
                     (((x) & 0x0000ff00UL) <<  8) | \
                     (((x) & 0x00ff0000UL) >>  8) | \
                     (((x) & 0xff000000UL) >> 24))
#define NET_NTOHL(x) NET_HTONL(x)

#define htons(x) (unsigned short)NET_HTONS(x)
#define ntohs(x) (unsigned short)NET_NTOHS(x)
#define htonl(x) (unsigned int)NET_HTONL(x)
#define ntohl(x) (unsigned int)NET_NTOHL(x)


typedef unsigned int in_addr_t;
typedef unsigned int ip4_addr_t;

in_addr_t inet_addr(const char *cp);

int inet_aton(const char *cp,ip4_addr_t *addr);
char *inet_ntoa(const ip4_addr_t *addr);

#endif // NET_IPADDR_H
