// #############################################################################
// #                   --- IW18 VFD Clock Firmware ---                         #
// #############################################################################
// # key.c - Key handler                                                       #
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

 #include <avr/io.h>
 #include "libs/key.h"
 #include "libs/portdef.h"
  
 volatile uint16_t debounce;


 // --- Initialize key ports ---
 void InitKeys( void ) 
  {
   TAST_DDR_OK    &= ~(1<<TAST_OK);                             // Input Ok
   TAST_DDR_PLUS  &= ~(1<<TAST_PLUS);                           // Input Plus
   TAST_DDR_MINUS &= ~(1<<TAST_MINUS);                          // Input Minus
   TAST_PRT_OK    |= (1<<TAST_OK);                              // Pullup Ok
   TAST_PRT_PLUS  |= (1<<TAST_PLUS);                            // Pullup Plus
   TAST_PRT_MINUS |= (1<<TAST_MINUS);                           // Pullup Minus
  }


 // --- Check if there are keys down ---
 void GetKeys( void )
  {
   if(keys & (1<<KEY_HANDLED)) { keys &= ~(1<<KEY_OK); keys &= ~(1<<KEY_PLUS); keys &= ~(1<<KEY_MINUS); }
	 
   if(!(TAST_PIN_OK & (1<<TAST_OK))) 
    {
     if(!(keys & (1<<KEY_HANDLED))) keys |= (1<<KEY_OK);
     debounce = 0xFFF; 
    } else {
     if(debounce!=0) debounce--; else keys &= ~(1<<KEY_HANDLED);
    }
	 
   if(!(TAST_PIN_PLUS & (1<<TAST_PLUS))) 
    {
     if(!(keys & (1<<KEY_HANDLED))) keys |= (1<<KEY_PLUS);
     debounce = 0xFFF; 
    } else {
     if(debounce!=0) debounce--; else keys &= ~(1<<KEY_HANDLED);
    }	 
	 
   if(!(TAST_PIN_MINUS & (1<<TAST_MINUS))) 
    {
     if(!(keys & (1<<KEY_HANDLED))) keys |= (1<<KEY_MINUS);
     debounce = 0xFFF; 
    } else {
     if(debounce!=0) debounce--; else keys &= ~(1<<KEY_HANDLED);
    }		 
  }
