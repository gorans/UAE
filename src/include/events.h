 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Events
  * These are best for low-frequency events. Having too many of them,
  * or using them for events that occur too frequently, can cause massive
  * slowdown.
  *
  * Copyright 1995-1998 Bernd Schmidt
  */

#include "machdep/rpt.h"

extern frame_time_t vsynctime, vsyncmintime;
extern void reset_frame_rate_hack (void);
extern int rpt_available;

extern unsigned long currcycle, nextevent, is_lastline;
extern unsigned long sample_evtime;
typedef void (*evfunc)(void);

struct ev
{
    int active;
    unsigned long int evtime, oldcycles;
    evfunc handler;
};

enum {
    ev_hsync, ev_copper, ev_audio, ev_cia, ev_blitter, ev_disk,
    ev_max
};

extern struct ev eventtab[ev_max];

STATIC_INLINE void events_schedule (void)
{
    int i;

    unsigned long int mintime = ~0L;
    for (i = 0; i < ev_max; i++) {
	if (eventtab[i].active) {
	    unsigned long int eventtime = eventtab[i].evtime - currcycle;
	    if (eventtime < mintime)
		mintime = eventtime;
	}
    }
    nextevent = currcycle + mintime;
}

STATIC_INLINE void do_cycles_slow (unsigned long cycles_to_add)
{
    if (is_lastline && eventtab[ev_hsync].evtime - currcycle <= cycles_to_add
	&& (long int)(read_processor_time () - vsyncmintime) < 0)
	return;

    while ((nextevent - currcycle) <= cycles_to_add) {
        int i;
        cycles_to_add -= (nextevent - currcycle);
        currcycle = nextevent;

        for (i = 0; i < ev_max; i++) {
	    if (eventtab[i].active && eventtab[i].evtime == currcycle) {
		(*eventtab[i].handler)();
	    }
	}
        events_schedule();
    }
    currcycle += cycles_to_add;
}

STATIC_INLINE void do_cycles_fast (void)
{
    if (is_lastline && eventtab[ev_hsync].evtime - currcycle <= 1
	&& (long int)(read_processor_time () - vsyncmintime) < 0)
	return;

    currcycle++;
    if (nextevent == currcycle) {
	int i;

	for (i = 0; i < ev_max; i++) {
	    if (eventtab[i].active && eventtab[i].evtime == currcycle) {
		(*eventtab[i].handler) ();
	    }
	}
	events_schedule();
    }

}

STATIC_INLINE unsigned long get_cycles (void)
{
    return currcycle;
}

#if /* M68K_SPEED == 1 */  0
#define do_cycles do_cycles_fast
#else
#define do_cycles do_cycles_slow
#endif