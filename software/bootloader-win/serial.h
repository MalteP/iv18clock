/***************************************************************************
 *   Copyright (C) 2008 by Blake Leverett                                  *
 *   bleverett@att.net                                                     *
 *                                                                         *
 *   FUNterm is free software; you can redistribute it and/or modify       *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

//  Um wenige Zeilen fuer Bootloader Programmer von Peter Dannegger erweitert!
// 15.5.09 A.N.


#ifndef SERIAL_H
#define SERIAL_H
/*
  CVS info:
  $Id: serial.h,v 1.1.1.1 2008/04/21 14:29:25 blakelev Exp $
  $Revision: 1.1.1.1 $
  $Date: 2008/04/21 14:29:25 $
 */

/**
   @file serial.h Includes defines for serial port operation.
   @addtogroup serial
 */


#include <stdio.h>
#include <windows.h>


BOOL __fastcall ProcessMessages(VOID);

#define UART_NO_DATA          0x0100              /* no receive data available  */

#define MESS_SERIAL (WM_USER+1)  ///< Custom windows message ID for serial messages.

BOOL OpenPort(int port,long baud,int HwFc, HWND handle);
void CloseSerialPort(void);
void PutSerialChar(int c);
int SerialPortIsOpen(void);
BOOL SerialIsChar(void);
int SerialGetChar(void);

void uart_flush(void);
void uart_puts(const unsigned char *s );
void uart_putc(unsigned char data);
unsigned int  uart_getc_X(int timeout);
unsigned int  uart_getc(unsigned long zeit);


#endif
