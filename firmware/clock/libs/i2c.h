// #############################################################################
// #                   --- IW18 VFD Clock Firmware ---                         #
// #############################################################################
// # i2c.h - Header: I2C routines for DS75 and DS1338                          #
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

#ifndef I2C_H
 #define I2C_H

 #define DS75ADDR          0x9E
 #define DS1338ADDR        0xD0
 #define DS1338STATUS      0x10 // SQW_OUT, 1Hz
 #define DS1338STATUS_TEST 0x13 // SQW_OUT, 32.768kHz

 extern volatile uint8_t ClkChng;
 
 void InitI2C( void );
 void InitRTC( void );
 void InitDS75( void );
 void DS75read( uint8_t *dshigh, uint8_t *dslow, uint8_t *dssign );
 uint8_t BCDtoDEC( uint8_t wert );
 void DS1338read( uint8_t *sec, uint8_t *min, uint8_t *hour, uint8_t *wday, uint8_t *day, uint8_t *mon, uint8_t *year );
 void DS1338write_time( uint8_t sec, uint8_t min, uint8_t hour );
 void DS1338write_date( uint8_t wday, uint8_t day, uint8_t mon, uint8_t year );
 uint8_t DS1338read_status( void );
 void DS1338write_status( uint8_t status );
 uint8_t DaylightSaving( uint8_t *sec, uint8_t *min, uint8_t *hour, uint8_t *wday, uint8_t *day, uint8_t *mon );

#endif
