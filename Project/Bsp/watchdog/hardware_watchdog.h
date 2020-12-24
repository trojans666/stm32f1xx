#ifndef HARDWARE_WATCHDOG_H
#define HARDWARE_WATCHDOG_H

//Î¹¹·
void keep_alive(void);

//ÃëÎªµ¥Î»
void wdt_set_timeout(unsigned short second);
unsigned int wdt_get_timeout(void);
unsigned int wdt_get_remain(void);

void wdt_enable(void);

void hardware_watchdog_init(void);

#endif // HARDWARE_WATCHDOG_H
