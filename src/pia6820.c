/* napple1 ncurses Apple 1 emulator
 * Copyright (C) 2008 Nobu Hatano
 *
 * Pom1 Apple 1 Emulator
 * Copyright (C) 2000 Verhille Arnaud
 * Copyright (C) 2006 John D. Corrado
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "pia6820.h"
#include "screen.h"

static unsigned char _dspCr = 0, _dsp = 0, _kbdCr = 0, _kbd = 0;

void resetPia6820(void)
{
	_kbdCr = _dspCr = 0xA7;
	_kbdCr = 0x27;
}

void writeDspCr(unsigned char dspCr)
{
	_dspCr = dspCr;
}

void writeDsp(unsigned char dsp)
{
	_dsp = dsp | 0x80;			//	Will be catched by updateDisplay
}

void updateDisplay()
{
	if (_dsp&0x80)
	{
		outputDsp(_dsp&0x7f);	//	Output char on screen
		_dsp &= 0x7f;			//	Dsp is available
	}
}

void keyPressed(unsigned char key)
{
	_kbd = key|0x80;
	_kbdCr = 0xA7;		// bit 8 to 1 means there is a key
}

void writeKbdCr(unsigned char kbdCr)
{
	_kbdCr = kbdCr;
}

unsigned char readDspCr(void)
{
	return _dspCr;
}

unsigned char readDsp(void)
{
	return _dsp;
}

unsigned char readKbdCr(void)
{
	return _kbdCr;
}

unsigned char readKbd(void)
{
	return _kbd;
}
