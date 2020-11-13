//-------+---------+---------+---------+---------+---------+---------+---------+
// 1991-1992 Betz Associates. Released to the Public Domain. This source code
// is offered without warranty or guarantee of any kind, and the author(s)
// expressly disclaim any responsibility for consequences of it's use in
// products for commercial, shareware, or public domain release of any kind.
//
// This release of Copyright applies only to this source code module, and
// does not affect other source code modules which may be included with it
// in a program.
//
// File Name: HTIMER.CPP
// Project:   Flights of Fantasy
// Creation:  August 2, 1992
// Author:    Mark Betz (MB)
//
// Machine:   IBM PC and Compatibles
//
// Includes:  htimer.h, types.h, dos.h
//
//-------+---------+---------+---------+---------+---------+---------+---------+
// Change History
// ------ -------
//
//      Date            Rev.	Author		Purpose
//      ----            ----    ------      -------
//      8-2-1992        1.0     MB          Initial release
//      9-20-1992       1.1     MB          -corrected several potential hdwr
//                                          problems.
//                                          -added getElapsed() member func.
//                                          -added setCount() member func.
//                                          -see HTIMER.DOC for change list
//
// Description
// -----------
//      Class member definitions, support functions, and control variables
//      for the hi-res timer class. The HTimer implements a microsecond
//      timer. Multiple timers may be instanced. The first timer created
//      initializes the 8254 timer channel 1. The last timer resets it to
//      it's default mode.
//
//      NOTE: it is strongly recommended that you read the accompanying
//      HTIMER.DOC file before using this class.
//-------+---------+---------+---------+---------+---------+---------+---------+

#include <dos.h>
#include <stdio.h>

#ifndef __TYPES__
    #include "types.h"
#endif
#ifndef __HTIMER__
    #include "htimer.h"
#endif

// initialization of static member data
word HTimer::numTimers = 0;                   // instance counter
word HTimer::countVal = 0;                    // 8254 counter value
dword HTimer::ticks = 0;                      // interrupt 8 tick counter

// 8254 timer control commands
static const int timerMode = 0x043;           // 8254 mode register port
static const int timer0Data = 0x040;          // counter port for channel 0
static const byte latchCmd = 0;               // 0 byte for latch command
static const byte mode3set = 0x36;            // 8254 mode set commands
static const byte mode2set = 0x34;

static dword longVal = 4294967295;         // maximum value of unsigned long

// pointer to low word of bios time of day counter at 0040:006C
static volatile word far* bios_ticks = (word far*)MK_FP(0x40, 0x6c);

static void interrupt (*Old08Handler)(...);   // pointer to old timer isr

// The following two functions program the 8254 timer chip to operate in
// either pulse mode (2), or square wave mode (3). Square wave is the bios
// default mode of timer operation. In both modes the value for the channel
// 1 counter is 65536 (actually 0 .. 65535). The difference between the two
// modes lies in the manipulation of the counter. In square wave mode, each
// pulse of the 1.193180 Mhz input signal decrements the value by 2. At
// terminal count (0) the CLKOUT1 line is lowered and the counter is reloaded.
// On the next terminal count CLKOUT1 is raised, and the timer interrupt
// is generated on IRQ 0. The state of CLKOUT1 plotted against time results
// in a square wave. In pulse mode each pulse of the input signal decrements
// the counter value by 1, and CLKOUT1 starts high. At terminal count the
// counter is reloaded, CLKOUT1 goes low for one clock pulse, and is then
// raised again, and the timer interrupt on IRQ 0 is generated. Both modes
// generate the timer interrupt at approx. 18.2 hz, but mode 2 is more
// accurate for this purpose, since mode 3 timings must be divided by two
// to get an accurate count of elapsed microseconds.

// program 8254 timer chip channel 0 to operate in pulse mode

void SetTimerMode2( word count )
	{
	outportb(timerMode, mode2set);              // send the mode 2 set command
	__emit__(0x90, 0x90);                       // to the 8254, then dwell
	outportb(timer0Data, *((byte*)&count));     // send the lsb of the count
	__emit__(0x90, 0x90);
	outportb(timer0Data, *(((byte*)&count)+1)); // send the msb of the count
	return;
	}

// reprogram the timer chip to operate in the bios default square wave mode

void SetTimerMode3( word count )
	{
	outportb(timerMode, mode3set);              // send the mode 3 set command
	__emit__(0x90, 0x90);                       // to the 8254
	outportb(timer0Data, *((byte*)&count));     // send the lsb of the count
	__emit__(0x90, 0x90);
	outportb(timer0Data, *(((byte*)&count)+1)); // send the msb of the count
    return;
	}

// Constructor for HTimer class. On entry the function checks the numTimers
// counter to see if it is the only existing instance. If so, it programs
// the timer chip to mode 2 (pulse mode).
//
// NOTE: the numTimers counter is eight bits, and no test is performed on
// it's current value. Thus creating more than 255 timers will roll the
// counter over and cause the timer chip to be reset by the next timer
// object to go out of scope, while other timers are still using it.
//
// NOTE: the first instance of an HTimer object always reprograms the
// 8254 channel 0 timer to mode 2 operation using the default counter
// value of 0, which generates irq 0 interrupts at 18.2 hz. If you
// wish to reprogram the timer to run faster or slower you must do so
// AFTER instancing the first HTimer object, and you must use the
// HTimer::setCount() member function to do the reprogramming.

HTimer::HTimer()
    {
    if ( numTimers == 0 )                      // is this the first instance?
       {
       SetTimerMode2( 0 );                     // yes, program pulse mode
       Old08Handler = getvect(0x8);            // save current irq 0 isr
       setvect(0x8, int8Handler);              // hook irq 0 vector for int 8
       }
    numTimers++;                               // increment instances counter
    tmrOn = false;                             // reset timer on status
    }

// Destructor for HTimer class. On entry the function checks the numTimers
// counter to see if it is the last existing timer. If so, it reprograms the
// timer channel to mode 3 (square wave), the bios default mode for channel
// 0

HTimer::~HTimer()
    {
    if ( numTimers == 1 )
       {
       SetTimerMode3( countVal );
       setvect(0x8, Old08Handler);
       }
    numTimers--;
    }

// **************************************************************************
// >>>>>> ISR: hardware interrupt IRQ 0 <<<<<<

// this static member function is an interrupt service routine for the timer
// hardware interrupt. It increments the static member tick counter on
// every interrupt, and wraps it at the max value of a long. The current
// number of ticks is used in calculating the elapsed microsecond count in
// HTimer::timerOff(), and HTimer::getElapsed()

void interrupt HTimer::int8Handler(...)
    {
    ticks++;
    Old08Handler();                            // chain the previous handler
    }

// **************************************************************************

// this function is used by timerOff() and getElapsed() to calculate the
// elapsed time in microseconds

dword HTimer::calcElapsed()
    {
    dword elapsedTime = 0;    // the value that will be returned
    word pulsesPerTick;       // number of clock pulses per interrupt
    dword endTick;            // number of int 8 ticks at end of current run
    dword numTicks;           // number of int 8 ticks elapsed during run
    word finish8254;          // finishing counter value

    if ( tmrOn )
        {
        // disable interrupts and latch current 8254 channel1 counter value

	    asm cli;
        endTick = ticks;
	    outportb(timerMode, latchCmd);
	    __emit__(0x90, 0x90);
        *(byte *)&finish8254 = inportb(timer0Data);
	    __emit__(0x90, 0x90);
	    *(((byte *)&finish8254) + 1) = inportb(timer0Data);
	    asm sti;

        // handle the case of a 0 counter value yielding 65535 pulses per
        // tick (see 8254 programmable timer documentation for details).

        if (countVal == 0)
           pulsesPerTick = 65535;
        else
           pulsesPerTick = countVal;

        // calculate number of IRQ 0 interrupts since timing started

        if (endTick >= startTick)
	       numTicks = ticks - startTick;
        else
           numTicks = (longVal - startTick) + ticks;

        // calculate elapsed counter value since timing started

	    if (numTicks == 0)
		    elapsedTime = (start8254 - finish8254);
	    else if (numTicks == 1)
            {
		    elapsedTime = start8254;
            elapsedTime += (pulsesPerTick - finish8254);
            }
	    else
		    {
		    elapsedTime = ((numTicks - 1) * pulsesPerTick);
		    elapsedTime += start8254;
            elapsedTime += (pulsesPerTick - finish8254);
		    }

        // convert to microseconds, avoid overflowing elapsedTime

        if (elapsedTime <= 4200000L)      // for timing runs longer than 4
           {                              // seconds we lose precision
           elapsedTime *= 1000;           // this order of operations is more
           elapsedTime /= 1193;           // accurate because the fractional
           }                              // part is maintained
        else
           {
           elapsedTime /= 1193;           // this order of operations is
           elapsedTime *= 1000;           // less accurate because the frac-
           }                              // tional part is lost in the
        }                                 // division
    return(elapsedTime);
    }

// timerOn() starts the timing process by grabbing the current countdown
// value from the 8254 timer chip, channel 0. This count is decremented
// 1,193,180 times per second by the 1.193 Mhz input signal to the chip.

void HTimer::timerOn()
	{
	startTick = ticks;                           // set tick counter
    tmrOn = true;                                // set timer on flag
	outportb(timerMode, latchCmd);               // tell 8254 to latch count
	__emit__(0x90, 0x90);                        // let the bus settle
	*(byte *)&start8254 = inportb(timer0Data);   // get the LSB of the count
	__emit__(0x90, 0x90);
	*(((byte *)&start8254) + 1) = inportb(timer0Data); // get the MSB
    }

// timerOff() closes down the current timing process by grabbing the
// counter value from channel 0, then calculating the elapsed time in
// units of a microsecond.

dword HTimer::timerOff()
	{
	dword elapsedTime;

    elapsedTime = calcElapsed();           // calculate elapsed time
    startTick = 0;                         // reset timer parameters
    start8254 = 0;
    tmrOn = false;                         // reset timer on flag
	return(elapsedTime);
	}

// this function returns the elapsed time for the current timing process in
// microseconds. The timer continues to run after the function returns

dword HTimer::getElapsed()
    {
    return(calcElapsed());                 // calc and return elapsed time
    }

// The HTimer::setCount() function is used to reprogram the channel 1 timer
// countdown value. Note that this function reprograms the timer chip and
// effects ALL instances of HTimer operating at the time of the call to it.
// See the detailed explanation of the counter operation in HTIMER.DOC

void HTimer::setCount( word regCount )
    {
    SetTimerMode2( regCount );
    countVal = regCount;
    }

// **************************************************************************
// a test frame for the timer object; these three functions time the number
// of microseconds in one tick of the bios time counter. Since this counter
// "ticks" at a frequency of approx. 18.2 hz, we should expect 54,945.05
// microseconds per tick. The first times the interval by turning the timer
// on, then off. The second performs the same test using the polling function.
// The timer continues to run. The third sets the timer to interrupt at a
// custom rate, and then times a bios tick to make sure that the higher
// hardware interrupt rate does not effect the timing.

// tests elapsed time by turning timer on, then off

dword testTimerOnOff()
    {
    HTimer timer1;                         // timer object for test
    dword etime;
    word lastTick;

    lastTick = *bios_ticks;                // grab current time tick
    while( lastTick == *bios_ticks );      // wait for it to change
    lastTick = *bios_ticks;                // it changed, grab the new value
    timer1.timerOn();                      // turn on the timer
    while( lastTick == *bios_ticks );      // wait for it to change
    etime = timer1.timerOff();             // turn off the timer
                                           // value should be about 54,945
    return(etime);
    }

// tests elapsed time by turning timer off, then calling HTimer::getElapsed.

dword testGetElapsed()
    {
    HTimer timer1;                         // timer object for test
    dword etime;
    word lastTick;

    lastTick = *bios_ticks;                // grab current time tick
    while( lastTick == *bios_ticks );      // wait for it to change
    lastTick = *bios_ticks;                // it changed, grab the new value
    timer1.timerOn();                      // turn on the timer
    while( lastTick == *bios_ticks );      // wait for it to change
    etime = timer1.getElapsed();           // turn off the timer
                                           // value should be about 54,945
    timer1.timerOff();
    return(etime);
    }

// this function first sets the channel 1 counter value to intRate, which
// changes the frequency at which interrupts are generated by the time on
// IRQ 0. It then times one bios time tick exactly as the previous two
// functions do. If caller of this function has set up an isr for interrupt
// 8, and is calling the original system isr for this interrupt at the
// proper frequency, we will expect to get the same value as with the other
// two test functions. Otherwise, this function will return the actual time
// between calls to the original interrupt handler.

dword testFastCount( word intRate )
    {
    HTimer timer1;                         // timer object for test
    dword etime;
    word lastTick;

    timer1.setCount( intRate );            // alter interrupt rate
    lastTick = *bios_ticks;                // grab current time tick
    while( lastTick == *bios_ticks );      // wait for it to change
    lastTick = *bios_ticks;                // it changed, grab the new value
    timer1.timerOn();                      // turn on the timer
    while( lastTick == *bios_ticks );      // wait for it to change
    etime = timer1.getElapsed();           // turn off the timer
                                           // value should be about 54,945
    timer1.setCount( 0 );                  // reset int rate to default
    return(etime);
    }

// **************************************************************************