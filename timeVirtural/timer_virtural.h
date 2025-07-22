#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <fw_types.h>

#define _INTERVAL_TICK 10 /*10 ms*/

typedef uint32_t clock_time_t;

	/**
	 * A timer.
	 *
	 * This structure is used for declaring a timer. The timer must be set
	 * with timer_set() before it can be used.
	 *
	 * \hideinitializer
	 */

	typedef enum
	{
		_timer_off,
		_timer_on,
		_timer_over // over time
	} timerStt_t;

	typedef struct
	{
	  timerStt_t status;
	  clock_time_t start;
	  clock_time_t interval;
	  clock_time_t left_time;
	} timer_virtual_t;

	clock_time_t timer_getTick(void);
	void timer_periodic_poll(void);

	void timer_set(timer_virtual_t *t, clock_time_t interval);
	void timer_reset(timer_virtual_t *t);
	void timer_restart(timer_virtual_t *t);
	void timer_stop(timer_virtual_t *t);
	uint8_t timer_expired(timer_virtual_t *t);
	void timer_makeExpired(timer_virtual_t *t);
	void timer_pause(timer_virtual_t *t);
	void timer_continue(timer_virtual_t *t);
	clock_time_t timer_remaining(timer_virtual_t *t);

#ifdef __cplusplus
}
#endif

#endif /* __TIMER_H__ */
