// #############################################################################
// #                   --- IW18 VFD Clock Firmware ---                         #
// #############################################################################
// # moon.c - Moon phase calculation                                           #
// # Using code from http://www.voidware.com/moon_phase.htm                    #
// # The non floating point (second) algorithm there seems to be accurate      #
// # enough for our purpose :-)                                                #
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
 #include "libs/moon.h"


 // --- Calculates the moon phase (0-7), accurate to 1 segment ---
 // 0 => new moon 
 // 4 => full moon
 uint8_t Moon_phase(uint16_t year, uint16_t month, uint16_t day)
 {
  uint16_t g, e;
  if (month == 1) --day;
   else if (month == 2) day += 30;
    else // m >= 3
     {
      day += 28 + (month-2)*3059/100;
      // adjust for leap years
      if (!(year & 3)) ++day;
      if ((year%100) == 0) --day;
     }
  g = (year-1900)%19 + 1;
  e = (11*g + 18) % 30;
  if ((e == 25 && g > 11) || e == 24) e++;
  return ((((e + day)*6+11)%177)/22 & 7);
 }
