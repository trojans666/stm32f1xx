#include "stm32f4xx_rtc.h"
#include "stm32f4xx_pwr.h"

#include "rtc_time.h"
#include "hardware_rtc.h"

#include <string.h>

//#define INN_RTC_CLOCK_SOURCE_LSI 1
#define INN_RTC_CLOCK_SOURCE_LSE    1

static int RTC_Configuration(void)
{
    RTC_InitTypeDef RTC_InitStruct;

    /* 使能pwr时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* 允许访问RTC */
	PWR_BackupAccessCmd(ENABLE);

#if defined (INN_RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
	/* Enable the LSI OSC */
	RCC_LSICmd(ENABLE);

	/* Wait till LSI is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{
	}

	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

	/* ck_spre(1Hz) = RTCCLK(LSI) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
	//uwSynchPrediv = 0xFF;
	//uwAsynchPrediv = 0x7F;

#elif defined (INN_RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
	/* 使能LSE */
	RCC_LSEConfig(RCC_LSE_ON);

	/* Wait till LSE is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}
	/* 选择LSE作为rtc时钟源 */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
#endif /* RTC_CLOCK_SOURCE_LSE */

	/* 使能RTC时钟 */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC APB registers synchronisation */
	RTC_WaitForSynchro();

	/* 设置异步预分频器的值*/
	RTC_InitStruct.RTC_AsynchPrediv = 0x7F;
	/* 设置同步预分频器的值 */
    RTC_InitStruct.RTC_SynchPrediv = 0xFF;
    RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;

    RTC_Init(&RTC_InitStruct);

	return 0;
}

static rtc_time_t read_rtc_device()
{
    struct rtc_tm ct;

    RTC_TimeTypeDef t;
    RTC_DateTypeDef d;

    //memset(&ct,0,sizeof(struct rtc_tm));

    //read device
    RTC_GetDate(RTC_Format_BIN,&d);
    RTC_GetTime(RTC_Format_BIN,&t);

    ct.tm_year = d.RTC_Year + 100;
    ct.tm_mon = d.RTC_Month - 1;
    ct.tm_mday = d.RTC_Date;
    ct.tm_wday = d.RTC_WeekDay;

    ct.tm_hour = t.RTC_Hours;
    ct.tm_min = t.RTC_Minutes;
    ct.tm_sec = t.RTC_Seconds;

    return rtc_mktime(&ct);
}

static int get_week(int year, int month, int day)
{
	if (month==1||month==2)
	{
		year -=1;
		month +=12;
	}
	return (day+1+2*month+3*(month+1)/5+year+(year/4)-year/100+year/400)%7+1;
}

static void write_rtc_device(rtc_time_t *time)
{
    struct rtc_tm *ct;
    struct rtc_tm tm_new;

    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_InitTypeDef RTC_InitStructure;
    RTC_DateTypeDef RTC_DateStructure;

    //将日历时间转换为本地时间
    ct = rtc_localtime(time);

    memcpy(&tm_new,ct,sizeof(struct rtc_tm));

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
    RTC_InitStructure.RTC_SynchPrediv = 0xFF;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStructure);

    //设置时间
    RTC_DateStructure.RTC_Year = tm_new.tm_year - 100;
    RTC_DateStructure.RTC_Month = tm_new.tm_mon + 1;
    RTC_DateStructure.RTC_Date = tm_new.tm_mday;
    RTC_DateStructure.RTC_WeekDay = get_week(tm_new.tm_year+1900,tm_new.tm_mon+1,tm_new.tm_mday);
    RTC_SetDate(RTC_Format_BIN,&RTC_DateStructure);

    //设置时间
    if(tm_new.tm_hour > 11)
    {
        RTC_TimeStructure.RTC_H12 = RTC_H12_PM;
    }
    else
    {
        RTC_TimeStructure.RTC_H12 = RTC_H12_AM;
    }
    RTC_TimeStructure.RTC_Hours = tm_new.tm_hour;
    RTC_TimeStructure.RTC_Minutes = tm_new.tm_min;
    RTC_TimeStructure.RTC_Seconds = tm_new.tm_sec;
    RTC_SetTime(RTC_Format_BIN,&RTC_TimeStructure);

    RTC_WriteBackupRegister(RTC_BKP_DR0,0x32F2);
}


//获取从1970-1-1到现在的时间
int rtc_gettimeofday(struct rtc_timeval *tp,void *ignore)
{
    rtc_time_t time;

    if(!tp)
        return -1;

    time = read_rtc_device();

    tp->tv_sec = time;
    tp->tv_usec = 0;

    return 0;
}

int rtc_settimeofday(struct rtc_timeval *tp,void *ignore)
{
    if(!tp)
        return -1;

    write_rtc_device(&tp->tv_sec);
    return 0;
}

//返回1970-1-1到现在的秒数
rtc_time_t rtc_time(rtc_time_t *t)
{
    rtc_time_t time_now = 0;

    time_now = read_rtc_device();

    if(t)
    {
        *t = time_now;
    }
    return time_now;
}


void hardware_rtc_init()
{
    if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)
    {
        if(RTC_Configuration() != 0)
        {
            return ;
        }
    }
    else
    {
        RTC_WaitForSynchro();
    }
}


