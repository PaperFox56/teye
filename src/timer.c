#include "timer.h"

#ifdef WIN32
#include <windows.h>
#endif
#if _POSIX_C_SOURCE >= 199309L
#include <time.h>
#endif


long currentTimeMillis() {
	struct timeval time;
	gettimeofday(&time, 0);
	long s1 = (long)(time.tv_sec) * 1000;
	long s2 = (time.tv_usec / 1000);
	return s1 + s2;
}


void sleep_ms(int milliseconds){
	#ifdef WIN32
	Sleep(milliseconds);
	#elif _POSIX_C_SOURCE >= 199309L
	struct timespec ts;
	ts.tv_sec = milliseconds / 1000;
	ts.tv_nsec = (milliseconds % 1000) * 1000000;
	nanosleep(&ts, 0);
	#else
	if (milliseconds >= 1000)
		sleep(milliseconds / 1000);
	usleep((milliseconds % 1000) * 1000);
	#endif
}
