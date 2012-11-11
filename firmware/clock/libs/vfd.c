// #############################################################################
// #                   --- IW18 VFD Clock Firmware ---                         #
// #############################################################################
// # vfd.c - VFD multiplexing and decoding handler                             #
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

// #############################################################################
// # Remarks                                                                   #
// #############################################################################
// # Optimized for 8MHz crystal.                                               #
// # Memory map on shift register: (Maxim MAX6921)                             #
// # Out0  : nc                                                                #
// # Out1  : nc                                                                #
// # Out2  : Decimal point                                                     #
// # Out3  : Segment d                                                         #
// # Out4  : Segment c                                                         #
// # Out5  : Segment e                                                         #
// # Out6  : Segment g                                                         #
// # Out7  : Segment b                                                         #
// # Out8  : Segment f                                                         #
// # Out9  : Segment a                                                         #
// # Out10 : nc                                                                #
// # Out11 : Grid 1                                                            #
// # Out12 : Grid 2                                                            #
// # Out13 : Grid 3                                                            #
// # Out14 : Grid 4                                                            #
// # Out15 : Grid 5                                                            #
// # Out16 : Grid 6                                                            #
// # Out17 : Grid 7                                                            #
// # Out18 : Grid 8                                                            #
// # Out19 : Grid 9                                                            #
// #############################################################################

 #include <avr/io.h>
 #include <util/delay.h>
 #include <avr/interrupt.h>
 #include "libs/vfd.h"
 #include "libs/portdef.h"
 #include "libs/vfdchar.h"
  
 uint8_t dim_val;


 // --- Initialize vfd ports and interrupt ---
 void InitVFD( void ) 
  {
   // First we set up the ports for the MAX6921 display shift register
   DISP_DDR_CLK  |= (1<<DISP_CLK);      // Clock
   DISP_DDR_LOAD |= (1<<DISP_LOAD);     // Load
   DISP_DDR_DATA |= (1<<DISP_DATA);     // Data
   DISP_DDR_BLNK |= (1<<DISP_BLNK);     // Blank
   
   DISP_PRT_CLK  &= ~(1<<DISP_CLK);     // Set ports to zero
   DISP_PRT_LOAD &= ~(1<<DISP_LOAD);
   DISP_PRT_DATA &= ~(1<<DISP_DATA);
   DISP_PRT_BLNK &= ~(1<<DISP_BLNK);
  
   // And of course the port for the LM9022 heater / hv chip
   DISP_DDR_STB  |= (1<<DISP_STB);      // Display standby

   DISP_PRT_STB  &= ~(1<<DISP_STB);     // Set port to zero
   
   TCCR2 = (1<<CS20) | (1<<CS21);       // Setup prescaler (8MHz / 32)
   TIMSK |= (1<<TOIE2);                 // Setup interrupt
   
   segdata[0]=59;
   segdata[1]=59;
   segdata[2]=59;
   segdata[3]=59;
   segdata[4]=59;
   segdata[5]=59;
   segdata[6]=59;
   segdata[7]=59;
   segdata[8]=59;

   dim_val = 0;	
  }

  
 // --- Definition for the interrupt ---
 ISR( TIMER2_OVF_vect ) 
  {
   GridShiftOut( segcounter+1 );                                            // Send grid data+1 -> 0 enables segment 1
   DontCare();                                                              // Unused pin
   SegShiftOut( pgm_read_byte(&chartab[segdata[segcounter]]) );             // Send segment data  
   DontCare();                                                              // And two unused pins
   DontCare(); 	

   DISP_PRT_LOAD |= (1<<DISP_LOAD);                                         // Pulse LOAD pin
   DISP_PRT_LOAD &= ~(1<<DISP_LOAD);  
    
   // Dim function and segment counter reset
   if( segcounter==0 )
    {
     segcounter = 9;                                                        // Reset to nine if zero
     if( dimcounter==0 )
      {
       dimcounter = dim_val;
       DISP_PRT_BLNK &= ~(1<<DISP_BLNK);
      } else {
       dimcounter--;
       DISP_PRT_BLNK |= (1<<DISP_BLNK);
      }
    }
   if(dim_val!=0) TCNT2 = 0xA0;                                             // Faster PWM if display dimmed
   segcounter--;                                                            // And decrement the counter by one
  }

  
 // --- Switch off display ---
 void DisplayStb( void )
  {
   GridShiftOut( 0 );							// Send grid data+1 -> 0 enables segment 1
   DontCare();								// Unused pin
   SegShiftOut( 0 );							// Send segment data  
   DontCare();								// And two unused pins
   DontCare();   
   DISP_PRT_STB  |= (1<<DISP_STB);					// And disable heater
  }

  
 // --- Send grid data to shift register ---
 void GridShiftOut( uint8_t data )
  {
   uint8_t i;

   for( i=9; i>0; i-- ) 
    {
     DISP_PRT_DATA &= ~(1<<DISP_DATA);    
     if(data==i) 
      {
       DISP_PRT_DATA |= (1<<DISP_DATA);   
      }
     DISP_PRT_CLK |= (1<<DISP_CLK);
     DISP_PRT_CLK &= ~(1<<DISP_CLK);
    }
  }


 // --- Send segment data to shift register ---
 void SegShiftOut( uint8_t data )
  {
   uint8_t i;

   for( i=8; i>0; i-- ) 
    {
     DISP_PRT_DATA &= ~(1<<DISP_DATA);
     if ( data & 0x80 )
      {
       DISP_PRT_DATA |= (1<<DISP_DATA);
      }
     data <<= 1;
     DISP_PRT_CLK |= (1<<DISP_CLK);
     DISP_PRT_CLK &= ~(1<<DISP_CLK);
    }  
  }


 // --- Send one empty bit to shift register ---
 void DontCare( void )
  {
   DISP_PRT_DATA &= ~(1<<DISP_DATA);
   DISP_PRT_CLK |= (1<<DISP_CLK);
   DISP_PRT_CLK &= ~(1<<DISP_CLK);
  }


 // --- Control display brightness ---
 void SetDim( uint8_t d )
  {
   dim_val = d;
  }

  
 // --- Convert to two digits ---
 void DisplayConvert( uint8_t val, uint8_t *digit_ten, uint8_t *digit_one ) 
  {
   *digit_ten = ( val / 10 );
   *digit_one = ( val % 10 );
  }

  
 // --- Convert to two digits /w dot ---
 void DisplayConvertDot( uint8_t val, uint8_t *digit_ten, uint8_t *digit_one ) 
  {
   *digit_ten = ( val / 10 );
   *digit_one = ( val % 10 ) + 10;
  }


 // --- Convert to two digits without leading zero if <10 ---
 void DisplayConvertNoZero( uint8_t val, uint8_t *digit_ten, uint8_t *digit_one ) 
  {
   *digit_ten = ((val/10)==0?59:(val/10));
   *digit_one = ( val % 10 );
  }


 // --- Convert weekday to text ---
 void WeekdayConvert( uint8_t val, uint8_t *digit_one, uint8_t *digit_two ) 
  {
   switch( val ) 
    {
     case 1:
      *digit_one = 32;
      *digit_two = 34;
      break;
     case 2:
      *digit_one = 23;
      *digit_two = 28;
      break;
     case 3:
      *digit_one = 32;
      *digit_two = 28;
      break;
     case 4:
      *digit_one = 23;
      *digit_two = 34;
      break;
     case 5:
      *digit_one = 25;
      *digit_two = 37;
      break;
     case 6:
      *digit_one = 38;
      *digit_two = 20;
      break;
     case 7:
      *digit_one = 38;
      *digit_two = 34;
      break;
     default:
      *digit_one = 49;
      *digit_two = 49;
    }
  } 


 // --- Convert delimiter id to vfd chars ---
 uint8_t delimiter_convert_h( uint8_t id )
  {
   return pgm_read_byte(&deltab_h[id]);
  }


 // --- Convert delimiter id to vfd chars ---
 uint8_t delimiter_convert_m( uint8_t id )
  {
   return pgm_read_byte(&deltab_m[id]);
  }


 // --- Convert moon phase to vfd chars ---
 uint8_t moon_convert_a( uint8_t id )
  {
   return pgm_read_byte(&moontab_a[id]);
  } 


 // --- Convert moon phase to vfd chars ---
 uint8_t moon_convert_b( uint8_t id )
  {
   return pgm_read_byte(&moontab_b[id]);
  } 
