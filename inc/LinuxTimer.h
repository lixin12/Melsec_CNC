#ifndef _LINUX_TIMER_H_
#define _LINUX_TIMER_H_

#include <time.h>


	void set_freq_nanosecs(long long ns);
	void create_timer();
	void destroy_timer();

#endif
