#ifndef TIMER_H
#define TIMER_H


#ifdef WIN32
#include <windows.h>
#endif
#if _POSIX_C_SOURCE >= 199309L
#include <time.h>
#endif

#include <sys/time.h>


/** @brief provide the current UNIX time in milliseconds */
long currentTimeMillis();

/** cross-platform sleep function */
void sleep_ms(int milliseconds);

#endif
