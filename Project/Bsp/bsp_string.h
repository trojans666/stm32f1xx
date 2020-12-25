#ifndef MINI_LIBC_H
#define MINI_LIBC_H


/******************************************************************************
* ������ : HexToAscii()
* ��  �� : �õ����ֽ�ASCII
* ��  �� : data,��ת��������4λ��Ч
* ��  �� : ��
* ����ֵ : ���ֽ�ascii��
* ��  �� : ��
* ��  �� : ��Ч�������0x30��
******************************************************************************/
extern u8 HexToAscii(u8 data);

/******************************************************************************
* ������ : HexToStr()
* ��  �� : �õ����ֽ�ASCII
* ��  �� : data,��ת��������4λ��Ч
* ��  �� : ��
* ����ֵ : ���ֽ�ascii��
* ��  �� : ��
* ��  �� : ��Ч�������0x30��
******************************************************************************/
extern u8 HexToStr(u8 *ptr,u32 dat,u8 len,u8 zs);

/******************************************************************************
* ������ : HexToBcdStr()
* ��  �� : �õ����ֽ�ASCII
* ��  �� : data,��ת��������4λ��Ч
* ��  �� : ��
* ����ֵ : ���ֽ�ascii��
* ��  �� : ��
* ��  �� : ��Ч�������0x30��
******************************************************************************/

extern u8 HexToBcdStr(u8 *ptr,s32 dat,u8 len,u8 zs);
extern u8 uHexToBcdStr(u8 *ptr,u32 dat,u8 len,u8 zs);

/******************************************************************************
* ������ : StrToHex()
* ��  �� : �õ�HEX��
* ��  �� : data,��ת����
* ��  �� : ��
* ����ֵ : ���ֽ�hex��
* ��  �� : ��
* ��  �� : ��Ч�������0��
******************************************************************************/
extern u32 StrToHex(u8 *ptr,u8 len);

/******************************************************************************
* ������ : HexToBcd()
* ��  �� : �õ�HEX��
* ��  �� : data,��ת����
* ��  �� : ��
* ����ֵ : ���ֽ�hex��
* ��  �� : ��
* ��  �� : ��Ч�������0��
******************************************************************************/
extern u32 HexToBcd(u32 Num);

extern void GetYearMonDayStr(u8 *str,struct tm *Time);

/******************************************************************************
* ������ : BitToChar()
* ��  �� : λ����ת��������
* ��  �� : dat,��ת����
* ��  �� : ��
* ����ֵ : λת��������
* ��  �� : ��
* ��  �� : ��Ч�������0��
******************************************************************************/
extern u8 BitToChar(u8 dat);
/******************************************************************************
* ������ : CharToBit()
* ��  �� : ����ת����λ����
* ��  �� : dat,��ת����
* ��  �� : ת����λ����
* ����ֵ : ת����λ����
* ��  �� : ��
* ��  �� : ��Ч�������0��
******************************************************************************/
extern u8 CharToBit(u8 dat);

int atoi_m(const char *s);
long atol_m(const char *s);

/****************** string ***********************/

void *memset_m(void *s,int c,unsigned long n);

void *memcpy_m(void *dest,const void *src,unsigned long n);

void *memmove_m(void *dest,const void *src,unsigned long n);

void *memcmp_m(const void *s1,const void *s2,unsigned long n);

int tolower_m(int c);

int toupper_m(int c);

int strcmp_m(const char *s1,const char *s2);

int strncmp_m(const char *cs,const char *ct,unsigned long count);

int strcasecmp_m(const char *a,const char *b);

int strncasecmp_m(const char *cs,const char *ct,unsigned long count);

int sscanf_m(const char *buf,const char *fmt,...);

unsigned long strlen_m(const char *s);

char *strstr_m(const char *s1,const char *s2);

char *strcpy_m(char *dest,const char *src);

unsigned long strlen_m(const char *s);

unsigned long strnlen_m(const char *s,unsigned long maxlen);

char *strncpy_m(char *dest,const char *src,unsigned long n);

unsigned long strlcpy_m(char *dst,const char *src,unsigned long count);

char *strncat_m(char *dest,const char *src,unsigned long count);

char *strcat_m(char *dest,const char *src);

char *strchr_m(const char *s1,int i);

char *strrchr_m(const char *t,int c);

char *strtok_m(char *s,const char *delim);

char *strtok_r_m(char *s,const char *delim,char **ptrptr);

unsigned long strcspn_m(const char *s,const char *reject);

unsigned long strspn_m(const char *s,const char *accept);

long strtol(const char *str,char **endptr,int base);

long long strtoll(const char *str,char **endptr,int base);

int atoi(const char *s);
int atol(const char *s);


#endif // MINI_LIBC_H
