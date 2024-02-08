// #############################################################################
// #                   --- IW18 VFD Clock Firmware ---                         #
// #############################################################################
// # uart.c - UART routines                                                    #
// #############################################################################
// #              Version: 2.2 - Compiler: AVR-GCC 4.5.0 (Linux)               #
// #  (c) 08-12 by Malte Pöggel - www.MALTEPOEGGEL.de - malte@maltepoeggel.de  #
// #############################################################################
// #  This program is free software; you can redistribute it and/or modify it  #
// #   under the terms of the GNU General Public License as published by the   #
// #        Free Software Foundation; either version 3 of the License,         #
// #                  or (at your option) any later version.                   #
// #                                                                           #
// #      This program is distributed in the hope that it will be useful,      #
// #      but WITHOUT ANY WARRANTY; without even the implied warranty of       #
// #           MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.            #
// #           See the GNU General Public License for more details.            #
// #                                                                           #
// #  You should have received a copy of the GNU General Public License along  #
// #      with this program; if not, see <http://www.gnu.org/licenses/>.       #
// #############################################################################

 #include <avr/io.h>
 #include "libs/uart.h"

 
 // --- Initialize the UART --- 
 void InitUART( void )
  {
   UCSRB |= (1 << RXEN) | (1 << TXEN);				// Enable UART TX 
   UCSRC |= (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);		// 8N1, 38400 baud 
   UBRRH = (UBRR_VAL >> 8);
   UBRRL = (UBRR_VAL & 0xFF);

   // Flush Receive-Buffer
   do { UDR; } while ( UCSRA & (1 << RXC) );
  }


 // --- Print character ---
 void PutChar( uint8_t c )
  {
   while (!(UCSRA & (1 << UDRE)));
   UDR = c;
  }


 // --- Print 8bit value ---
 void PutInt8( uint8_t i )
  {
    PutChar('0'+((i / 100) % 10));
    PutChar('0'+((i / 10) % 10));
    PutChar('0'+(i % 10));
    PutChar('\r');
    PutChar('\n');
  }


 // --- Print 16bit value ---
 void PutInt( uint16_t i )
  {
    PutChar('0'+(i / 1000));
    PutChar('0'+((i / 100) % 10));
    PutChar('0'+((i / 10) % 10));
    PutChar('0'+(i % 10));
    PutChar('\r');
    PutChar('\n');
  }
