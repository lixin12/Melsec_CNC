
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include "LinuxTimer.h"

#define CLOCKID CLOCK_REALTIME
#define SIG SIGALRM

static long long freq_nanosecs;
static timer_t timerid;


static void
handler(int sig, siginfo_t *si, void *uc)
{
   /* Note: calling printf() from a signal handler is not
      strictly correct, since printf() is not async-signal-safe;
      see signal(7) */

   printf("Caught signal %d\n", sig);
}
void set_freq_nanosecs(long long ns)
{
	freq_nanosecs = ns;
}

void destroy_timer()
{
	if(timerid != 0)
	{
		printf("timer delete ret:%d\n", timer_delete (timerid));
		timerid = 0;
	}
}

void create_timer()
{
   struct sigevent sev;
   struct itimerspec its;
   struct sigaction sa;

   /* Establish handler for timer signal */

   printf("Establishing handler for signal %d\n", SIG);
   sa.sa_flags = SA_SIGINFO;
   sa.sa_sigaction = handler;
   sigemptyset(&sa.sa_mask);
   if (sigaction(SIG, &sa, NULL) == -1)
       printf("sigaction fail\n");

   /* Create the timer */

   sev.sigev_notify = SIGEV_SIGNAL;
   sev.sigev_signo = SIG;
   sev.sigev_value.sival_ptr = &(timerid);
   if (timer_create(CLOCKID, &sev, &(timerid)) == -1)
       printf("timer_create fail\n");

   printf("timer ID is 0x%lx\n", (long)timerid);

   /* Start the timer */

   its.it_value.tv_sec = freq_nanosecs / 1000000000;
   its.it_value.tv_nsec = freq_nanosecs % 1000000000;
   its.it_interval.tv_sec = its.it_value.tv_sec;
   its.it_interval.tv_nsec = its.it_value.tv_nsec;

   if (timer_settime(timerid, 0, &its, NULL) == -1)
        printf("timer_settime fail\n");

}


