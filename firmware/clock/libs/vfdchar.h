// #############################################################################
// #                   --- IW18 VFD Clock Firmware ---                         #
// #############################################################################
// # vfdchar.h - Header: Display character table                               #
// #############################################################################
// #              Version: 2.0 - Compiler: AVR-GCC 4.5.0 (Linux)               #
// #  (c) 08-10 by Malte Pöggel - www.MALTEPOEGGEL.de - malte@maltepoeggel.de  #
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

// #############################################################################
// # Remarks                                                                   #
// #############################################################################
// #           a                                                               #
// #         -----                                                             #
// #      f |     | b           +---+---+---+---+---+---+---+---+              #
// #        |  g  |             |128| 64| 32| 16|  8|  4|  2|  1|              #
// #         -----              +---+---+---+---+---+---+---+---+              #
// #      e |     | c           |  a|  f|  b|  g|  e|  c|  d| dp|              #
// #        |  d  |             +---+---+---+---+---+---+---+---+              #
// #         -----  * dp                                                       #
// #                                                                           #
// #############################################################################


#ifndef VFDCHAR_H
 #define VFDCHAR_H
  
 #include <avr/pgmspace.h>

 const prog_uint8_t chartab[60] = {
  238, 36,186,182,116,214,222,164,254,246, // 0...9
  239, 37,187,183,117,215,223,165,255,247, // 0...9 /w dp
  252, 94,202, 62,218,216,206, 92, 72, 46, // A...J
   88, 74,236, 28, 30,248,244, 24,214, 90, // K...T
  110, 14, 98, 56,118,186,240, 30,128, 16, // U...Z, other chars
    2,  8,  4, 32, 64,  1, 17, 72,166,  0
 };

 const prog_uint8_t deltab_h[8] = {
  49, 27, 46, 47, 48, 50, 53, 59
 };

 const prog_uint8_t deltab_m[8] = {
  49, 32, 46, 47, 48, 50, 53, 59
 };

 const prog_uint8_t moontab_a[8] = {
  59, 59, 59,  1, 22, 22, 22, 57
 };

 const prog_uint8_t moontab_b[8] = {
  59,  1, 58, 58, 58, 57, 59, 59
 };

#endif
