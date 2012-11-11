// #############################################################################
// #                   --- IW18 VFD Clock Firmware ---                         #
// #############################################################################
// # vfd.h - Header: VFD multiplexing and decoding handler                     #
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

#ifndef VFD_H
 #define VFD_H

 // Globals for segment data
 volatile uint8_t segdata[9];		// Array for segment data
 volatile uint8_t segcounter;		// Counter for grid position
 volatile uint8_t dimcounter;           // Counter for PWM dimming
    
 void InitVFD( void );
 void DisplayStb( void );
 void GridShiftOut( uint8_t );
 void SegShiftOut( uint8_t data );
 void DontCare( void );
 void SetDim( uint8_t d );
 void DisplayConvert( uint8_t val, uint8_t *digit_ten, uint8_t *digit_one );
 void DisplayConvertDot( uint8_t val, uint8_t *digit_ten, uint8_t *digit_one );
 void DisplayConvertNoZero( uint8_t val, uint8_t *digit_ten, uint8_t *digit_one );
 void WeekdayConvert( uint8_t val, uint8_t *digit_one, uint8_t *digit_two );
 uint8_t delimiter_convert_h( uint8_t id );
 uint8_t delimiter_convert_m( uint8_t id );
 uint8_t moon_convert_a( uint8_t id );
 uint8_t moon_convert_b( uint8_t id );

#endif
