#ifndef RTC_TIME_H
#define RTC_TIME_H

typedef long rtc_time_t;

//struct timeval
struct rtc_timeval
{
    long tv_sec;
    long tv_usec;
};

// timespec
struct rtc_timespec
{
    rtc_time_t tv_sec;
    long tv_nsec;
};

//timezone
struct rtc_timezone
{
    int tz_minuteswest;
    int tz_dsttime;
};

//tm
struct rtc_tm
{
  int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
  int tm_min;			/* Minutes.	[0-59] */
  int tm_hour;			/* Hours.	[0-23] */
  int tm_mday;			/* Day.		[1-31] */
  int tm_mon;			/* Month.	[0-11] */
  int tm_year;			/* Year - 1900. */
  int tm_wday;			/* Day of week.	[0-6] */
  int tm_yday;			/* Days in year.[0-365]	*/
  int tm_isdst;			/* DST.		[-1/0/1]*/

  long int tm_gmtoff;		/* Seconds east of UTC.  */
  const char *tm_zone;		/* Timezone abbreviation.  */
};

//��ȡ��1970-1-1�����ڵ�ʱ��
extern int rtc_gettimeofday(struct rtc_timeval *tp,void *ignore);
//����1970-1-1�����ڵ�����
extern rtc_time_t rtc_time(rtc_time_t *t);
//����ʱ��
extern int rtc_settimeofday(struct rtc_timeval *tp,void *ignore);
//��ʱ��ת��Ϊ1970-1-1�����ڵ�����
rtc_time_t rtc_mktime(struct rtc_tm *const t);
//��ʽ�����ʱ��
char *rtc_asctime(const struct rtc_tm *timeptr);
//��ʽ�����
char *rtc_ctime(const rtc_time_t *timep);
//��ȡ����ʱ��
struct rtc_tm *rtc_localtime(const rtc_time_t *t);

char *rtc_asctime_r(const struct rtc_tm *t,char *buf);
//ת��Ϊ��ʵʹ�õĸ�ʽ
struct rtc_tm *rtc_gmtime_r(const rtc_time_t *timep,struct rtc_tm *r);

struct rtc_tm *rtc_localtime_r(const rtc_time_t *t,struct rtc_tm *r);

void rtc_set_time(int year,int month,int day,int hour,int minute,int second);

/*
	rtc_set_time(2019,12,6,11,55,50);
	
	rtc_time_t timer;   
	struct rtc_tm* t_tm;   
	rtc_time(&timer);   
	t_tm = rtc_localtime(&timer);   
	log_info("today is %4d-%02d-%02d %02d:%02d:%02d \n", t_tm->tm_year+1900,
	t_tm->tm_mon+1, t_tm->tm_mday, t_tm->tm_hour, t_tm->tm_min, t_tm->tm_sec); 
*/

#endif // RTC_TIME_H
