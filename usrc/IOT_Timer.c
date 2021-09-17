
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include "log.h"
#include "type.h"
#define CLOCKID CLOCK_REALTIME
#define SIG SIGALRM

static void handler(int sig, siginfo_t *si, void *uc)
{
   /* Note: calling printf() from a signal handler is not
      strictly correct, since printf() is not async-signal-safe;
      see signal(7) */

   zlg_debug("Caught signal %d\n", sig);
}


void destroy_timer(timer_t* timerid)
{
	if(*timerid != 0)
	{
		zlg_debug("timer delete ret:%d\n", timer_delete (*timerid));
		*timerid = 0;
	}
}

void create_timer(long long freq_nanosecs,timer_t* timerid)
{
   struct sigevent sev;
   struct itimerspec its;
   struct sigaction sa;

   /* Establish handler for timer signal */

   zlg_debug("Establishing handler for signal %d\n", SIG);
   sa.sa_flags = SA_SIGINFO;
   sa.sa_sigaction = IOT_TIMER.handler;
   sigemptyset(&sa.sa_mask);
   if (sigaction(SIG, &sa, NULL) == -1)
       zlg_debug("sigaction fail\n");

   /* Create the timer */

   sev.sigev_notify = SIGEV_SIGNAL;
   sev.sigev_signo = SIG;
   sev.sigev_value.sival_ptr = timerid;
   if (timer_create(CLOCKID, &sev, timerid) == -1)
       zlg_debug("timer_create fail\n");

   zlg_debug("timer ID is 0x%lx\n", (long)(*timerid));

   /* Start the timer */

   its.it_value.tv_sec = freq_nanosecs / 1000000000;
   its.it_value.tv_nsec = freq_nanosecs % 1000000000;
   its.it_interval.tv_sec = its.it_value.tv_sec;
   its.it_interval.tv_nsec = its.it_value.tv_nsec;

   if (timer_settime(*timerid, 0, &its, NULL) == -1)
        zlg_debug("timer_settime fail\n");
}
_IOT_TIMER IOT_TIMER = 
{
    create_timer,
    destroy_timer,
    handler
};
