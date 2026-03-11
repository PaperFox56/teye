#ifndef TIMER_H
#define TIMER_H


#include <sys/time.h>


/** @brief provide the current UNIX time in milliseconds */
long currentTimeMillis();

/** cross-platform sleep function */
void sleep_ms(int milliseconds);

#endif
