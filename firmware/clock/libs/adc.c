// #############################################################################
// #                   --- IW18 VFD Clock Firmware ---                         #
// #############################################################################
// # adc.c - Analog digital converter handler                                  #
// #############################################################################
// #              Version: 2.1 - Compiler: AVR-GCC 4.5.0 (Linux)               #
// #  (c) 08-11 by Malte Pöggel - www.MALTEPOEGGEL.de - malte@maltepoeggel.de  #
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
 #include "libs/adc.h"
 #include "libs/portdef.h"

 
 // --- Initialize the ADC registers --- 
 void InitADC( void )
  {
   uint16_t result;
 
   ADMUX &= ~((1<<REFS1) | (1<<REFS0)); // Internal Vref turned off
   ADCSRA = (1<<ADPS1) | (1<<ADPS0);    // Prescaler Clock / 8
   ADCSRA |= (1<<ADEN);                 // Enable ADC
 
   // Dummy readout
   ADCSRA |= (1<<ADSC);
   while (ADCSRA & (1<<ADSC) );
   result = ADCW;
  }


 // --- Get channel value ---
 uint16_t GetADC(uint8_t chan)
  {
   ADMUX = (ADMUX & ~(0x1F)) | (chan & 0x1F);
   ADCSRA |= (1<<ADSC);                 // Single conversion
   while (ADCSRA & (1<<ADSC) );
   return ADCW;
  }
