// #############################################################################
// #                   --- IV18 VFD Clock Firmware ---                         #
// #############################################################################
// # main.c - Main function                                                    #
// #############################################################################
// #              Version: 2.2 - Compiler: AVR-GCC 4.5.0 (Linux)               #
// #    (c) 08-24 by Malte Pöggel - www.MALTEPOEGGEL.de - malte@poeggel.de     #
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

 #include <util/delay.h>
 #include <avr/wdt.h>
 #include <avr/interrupt.h>
 #include "main.h"
 #include "ui.h"
 #include "vfd.h"
 #include "isd.h"
 #include "i2c.h"
 #include "adc.h"
 #include "uart.h"
 #include "key.h"
 #include "alarm.h"
 #include "rfm.h"


 int main( void )
  {
   InitVFD();
   InitISD();
   InitI2C();
   InitADC();
   InitRTC();
   InitDS75();
   InitKeys();
   InitUART();
   InitRFM();
   InitAlarm();

   sei();
   wdt_enable(WDT_DEFAULT);

   UI();   

   return 0;
 }
