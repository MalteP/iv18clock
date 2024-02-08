// #############################################################################
// #                   --- IV18 VFD Clock Firmware ---                         #
// #############################################################################
// # key.h - Header: Key handler                                               #
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

#ifndef KEY_H
 #define KEY_H
  
 #define KEY_OK      0 // Key ok pressed now
 #define KEY_PLUS    1 // Key plus pressed now
 #define KEY_MINUS   2 // Key minus pressed now
 #define KEY_LONG    4 // Key can be pressed longer to retrigger handled with HOLD flag set
 #define KEY_REPEAT  5 // Key auto repeat
 #define KEY_HOLD    6 // Flag that this key is held
 #define KEY_HANDLED 7 // Flag that this key was handled

 #define DEBOUNCE_VAL 0x01FF

 // Increase speed after n+1 ticks
 #define TICK_SPEED   2
 #define TICK_VAL_A   0x3F00
 #define TICK_VAL_B   0x1000

 extern volatile uint8_t keys;
  
 void InitKeys( void );
 void GetKeys( void );
   
#endif
