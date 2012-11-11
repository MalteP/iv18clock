// #############################################################################
// #                   --- IW18 VFD Clock Firmware ---                         #
// #############################################################################
// # key.c - Key handler                                                       #
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
 #include "libs/key.h"
 #include "libs/portdef.h"
  
 volatile uint16_t debounce;
 volatile uint16_t tick;
 volatile uint16_t tick_spd;

 // --- Initialize key ports ---
 void InitKeys( void ) 
  {
   TAST_DDR_OK    &= ~(1<<TAST_OK);                             // Input Ok
   TAST_DDR_PLUS  &= ~(1<<TAST_PLUS);                           // Input Plus
   TAST_DDR_MINUS &= ~(1<<TAST_MINUS);                          // Input Minus
   TAST_PRT_OK    |= (1<<TAST_OK);                              // Pullup Ok
   TAST_PRT_PLUS  |= (1<<TAST_PLUS);                            // Pullup Plus
   TAST_PRT_MINUS |= (1<<TAST_MINUS);                           // Pullup Minus
   tick_spd = 0;
  }


 // --- Check if there are keys down ---
 void GetKeys( void )
  {
   uint8_t pressed = 0;
  
   if(keys & (1<<KEY_HANDLED)) { keys &= ~(1<<KEY_OK); keys &= ~(1<<KEY_PLUS); keys &= ~(1<<KEY_MINUS); keys &= ~(1<<KEY_HOLD); }

   // -- PLUS key --
   if(!(TAST_PIN_PLUS & (1<<TAST_PLUS))) 
    {
     // Key pressed
     pressed = 1;
     debounce = DEBOUNCE_VAL;
     if(!(keys & (1<<KEY_HANDLED)))
      {
       // Event not handled before
       keys |= (1<<KEY_PLUS);
      }
    }

   // -- OK key --
   if(!(TAST_PIN_PLUS & (1<<TAST_OK))) 
    {
     // Key pressed
     pressed = 1;
     debounce = DEBOUNCE_VAL;
     if(!(keys & (1<<KEY_HANDLED)))
      {
       // Event not handled before
       keys |= (1<<KEY_OK);
      }
    }

   // -- MINUS key --
   if(!(TAST_PIN_PLUS & (1<<TAST_MINUS))) 
    {
     // Key pressed
     pressed = 1;
     debounce = DEBOUNCE_VAL;
     if(!(keys & (1<<KEY_HANDLED)))
      {
       // Event not handled before
       keys |= (1<<KEY_MINUS);
      }
    }

   // -- Debounce and repeat stuff --       
   if(pressed==0)
    {
     // No key pressed
     tick_spd = 0;
     tick = TICK_VAL_A;
     if(debounce!=0)
      {
       debounce--;
      } else {
       keys &= ~(1<<KEY_LONG);
       keys &= ~(1<<KEY_REPEAT);     
       keys &= ~(1<<KEY_HOLD);
       keys &= ~(1<<KEY_HANDLED);
      }    
    } else {
     // A key pressed
     if((keys & (1<<KEY_REPEAT)))
      {
       // Key repeat
       if((keys & (1<<KEY_HANDLED)))
        {      
         // Key handled before
         if(tick>0)
          {
           tick--;          
          } else {
           if(tick_spd<TICK_SPEED)
            {
             tick = TICK_VAL_A;
             tick_spd++;
            } else {
             tick = TICK_VAL_B;
            }
           keys &= ~(1<<KEY_HANDLED);
           keys &= ~(1<<KEY_REPEAT);            
          }
        } else {
         // Key was not handled
         if(tick_spd<TICK_SPEED)
          {
           tick = TICK_VAL_A;
           tick_spd++;
          } else {
           tick = TICK_VAL_B;
          }       
        }
      } else {
       // No key repeat
       if((keys & (1<<KEY_LONG)))
        {
         if(tick>1)
          {
           tick--;          
          } else {
           if(tick>0)
            {
             // Only once
             tick--;
             keys |= (1<<KEY_HOLD);
             keys &= ~(1<<KEY_HANDLED);
            }
          }   
        } 
      }  
    }
    
  }
