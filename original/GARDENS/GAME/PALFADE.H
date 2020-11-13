//-------+---------+---------+---------+---------+---------+---------+---------+
// 1991-1992 Betz Associates. Released to the Public Domain. This source code
// is offered without warranty or guarantee of any kind, and the author(s)
// expressly disclaim any responsibility for consequences of it's use in
// products for commercial, shareware, or public domain release of any kind.
//
// File Name: PALFADE.H
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
//      prototypes for two assembler functions for fading the VGA palette
//      in/out
//
//-------+---------+---------+---------+---------+---------+---------+---------+

#ifndef __PALFADE__
#define __PALFADE__

// These two function defintions prototype the assembler functions in
// PALFADE.ASM. Each performs a 64-pass fade on the VGA dac palette. The
// fadepalin() function requires three parameters: start - the first
// palette register to process; count - the number of subsequent registers
// to process; palette - a pointer to the first byte of a 768 byte table
// of six-bit R-G-B values. The display palette will start at 0, then fade
// in until it matches the values in the table.

// The fadepalout() function takes only two parameters, start and count,
// since it doesn't need a target palette. The current display palette is
// faded out until all R-G-B triplets are 0.


extern "C"
    {
    void fadepalin(int start, int count, const byte far* palette);
    void fadepalout(int start, int count);
    }

#endif