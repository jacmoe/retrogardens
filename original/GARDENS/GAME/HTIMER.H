//-------+---------+---------+---------+---------+---------+---------+---------+
// 1991-1992 Betz Associates. Released to the Public Domain. This source code
// is offered without warranty or guarantee of any kind, and the author(s)
// expressly disclaim any responsibility for consequences of it's use in
// products for commercial, shareware, or public domain release of any kind.
//
// File Name: HTIMER.H
// Creation:  August 2, 1992
// Author:    Mark Betz (MB)
//
// Machine:   IBM PC and Compatibles
//
// Includes:  nothing
//
//-------+---------+---------+---------+---------+---------+---------+---------+
// Change History
// ------ -------
//
//      Date            Rev.	Author		Purpose
//      ----            ----    ------      -------
//      8-2-1992        1.0     MB          Initial release
//      9-20-1992       1.1     MB          Update release v 1.1
//
// Description
// -----------
//      Header file for the hi-res multi-channel timer
//
//-------+---------+---------+---------+---------+---------+---------+---------+

#ifndef __HTIMER__
#define __HTIMER__

// The HTimer class implements a microsecond timer. Each instance of the
// class provides a seperate 32-bit timer channel (up to 255). When the
// constructor is called it tests a static member counter to see if it is the
// first timer instance. If it is, then it reprograms the 8254 channel 1 to
// operate in pulse mode, and increments the counter. When the destructor
// is called it checks the same counter to see if it is the "last guy out".
// If so, it resets the channel to default operation (square wave mode).

class HTimer
    {
    public:

    HTimer();
    ~HTimer();
    void timerOn();        // starts the timer
    dword timerOff();      // stops the timer, returns elapsed time in mics
    dword getElapsed();    // returns the current elapsed time without
                           // resetting the timer
    void setCount( word regCount );  // used to tell the timer what value
                                     // has been loaded into the 8254
                                     // counter register
    private:

    dword calcElapsed();   // used internally to calculate elapsed time
    static void interrupt int8Handler(...);

    boolean tmrOn;         // true if the timer is running
    word start8254;        // counter value at start of current run
    dword startTick;       // number of int 8 ticks at start of current run
    static dword ticks;    // incremented on every IRQ 0 level interrrupt
    static word numTimers; // the number of timer instances in existence
    static word countVal;  // value loaded into the 8254 counter register
    };

// These three functions are a test frame for the timer object

dword testTimerOnOff();
dword testGetElapsed();
dword testFastCount( word intRate );

#endif