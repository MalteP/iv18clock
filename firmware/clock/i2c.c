// #############################################################################
// #                   --- IV18 VFD Clock Firmware ---                         #
// #############################################################################
// # i2c.c - I2C routines for DS75 and DS1338                                  #
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


 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include "i2c.h"
 #include "i2cmaster.h"
 #include "portdef.h"

 volatile uint8_t ClkChng;
 volatile uint8_t dst_status;
 
 
 // --- Initialize the TWI registers --- 
 void InitI2C( void )
  {
   i2c_init();                                             // Init the i2c bus
  }
  
  
 // --- Setup RTC interrupt ---
 void InitRTC( void )
  {
   RTC_PRT_INT |= (1<<RTC_INT);                            // Pullup
   MCUCR = (MCUCR | (1 << ISC11)) & ~ (1 << ISC10);	   // Trigger on falling edge
   GIFR = (1 << INTF1);                                    // Delete flag
   GICR |= (1<<INT1);                                      // Setup and enable interrupt
   dst_status=0;
  }
  
  
 // --- Set temp sensor to 11 bit resolution ---
 void InitDS75( void )
  {
   i2c_start(DS75ADDR+I2C_WRITE);                          // Write to DS75 
   i2c_write(0x01);                                        // Pointer to configuration register
   i2c_write(0x40);                                        // 11 bit resolution
   i2c_stop();                                             // Stop
  } 
  
  
 // --- Interrupt on clock change ---
 ISR(INT1_vect)
  {
   ClkChng = 1;                                            // Save that clock has changed
  }
  
   
 // --- Get temperature --- 
 void DS75read( uint8_t *dshigh, uint8_t *dslow, uint8_t *dssign ) 
  {
   uint8_t ds75high, ds75low;
   i2c_start(DS75ADDR+I2C_WRITE);                          // Write to DS75 
   i2c_write(0x00);                                        // Pointer to temperature register
   i2c_rep_start(DS75ADDR+I2C_READ);                       // Read from DS75
   ds75high = i2c_readAck();                               // Read high byte
   ds75low = i2c_readNak();                                // Read low byte
   i2c_stop();                                             // Stop
   *dssign = 0;
   if( ds75high > 127 )                                    // Negative value?
   {
    ds75high = ~ds75high;
    *dssign = 1;
   }
   *dshigh=ds75high;                                       // High byte
   *dslow=0;
   if( ds75low & 0x20 ) *dslow=1;                          // Convert low byte to 1 digit
   if( ds75low & 0x40 ) *dslow+=3;
   if( ds75low & 0x80 ) *dslow+=5;
  }
  
  
 // --- BCD to decimal ---
 uint8_t BCDtoDEC( uint8_t wert ) 
  {
   return ((wert & 0xF0) >> 4) * 10 + (wert & 0x0F);
  }


 // --- decimal to BCD ---
 uint8_t DECtoBCD( uint8_t wert ) 
  {
   return ((wert/10)<<4) + (wert % 10);
  } 

  
 // --- Get time and date ---
 void DS1338read( uint8_t *sec, uint8_t *min, uint8_t *hour, uint8_t *wday, uint8_t *day, uint8_t *mon, uint8_t *year ) 
  {
   uint8_t tsec, tmin, thour, twday, tday, tmon, tyear;
   i2c_start(DS1338ADDR+I2C_WRITE);
   i2c_write(0x00);
   i2c_rep_start(DS1338ADDR+I2C_READ);  
   tsec = i2c_readAck();
   tmin = i2c_readAck();
   thour = i2c_readAck();
   twday = i2c_readAck();
   tday = i2c_readAck();
   tmon = i2c_readAck();
   tyear = i2c_readNak();
   i2c_stop();
   *sec=BCDtoDEC(tsec);
   *min=BCDtoDEC(tmin);
   *hour=BCDtoDEC(thour);
   *wday=twday;
   *day=BCDtoDEC(tday);
   *mon=BCDtoDEC(tmon);
   *year=BCDtoDEC(tyear);
  }


 // --- Write time to RTC ---
 void DS1338write_time( uint8_t sec, uint8_t min, uint8_t hour )
  {
   uint8_t tsec, tmin, thour;
   tsec=DECtoBCD(sec);
   tmin=DECtoBCD(min);
   thour=DECtoBCD(hour);    
   i2c_start(DS1338ADDR+I2C_WRITE);
   i2c_write(0x00);
   i2c_write(tsec);
   i2c_write(tmin);
   i2c_write(thour);
   i2c_stop();
  }


 // --- Write date to RTC ---
 void DS1338write_date( uint8_t wday, uint8_t day, uint8_t mon, uint8_t year )
  {
   uint8_t tday, tmon, tyear;
   tday=DECtoBCD(day);
   tmon=DECtoBCD(mon);
   tyear=DECtoBCD(year);    
   i2c_start(DS1338ADDR+I2C_WRITE);
   i2c_write(0x03);
   i2c_write(wday);
   i2c_write(tday);
   i2c_write(tmon);
   i2c_write(tyear);
   i2c_stop();
  }


 // --- Read RTC status register ---
 uint8_t DS1338read_status( void ) 
  {
   uint8_t status;
   i2c_start(DS1338ADDR+I2C_WRITE);
   i2c_write(0x07);
   i2c_rep_start(DS1338ADDR+I2C_READ);  
   status = i2c_readNak();
   i2c_stop();
   return status;
  }


 // --- Write RTC status register ---
 void DS1338write_status( uint8_t status )
  {
   i2c_start(DS1338ADDR+I2C_WRITE);
   i2c_write(0x07);
   i2c_write(status);
   i2c_stop();
  }
  

 // --- Check for time correction (summer/winter in germany) ---
 // Last sunday in march at 2:00am -> +1h
 // Last sunday in october at 3:00am -> -1h 
 uint8_t DaylightSaving( uint8_t *sec, uint8_t *min, uint8_t *hour, uint8_t *wday, uint8_t *day, uint8_t *mon )
  {
   if( *wday == 7 )
    {
     if( *mon == 3 )
      {
       if( (*day+7) > 31 )
        {
         if(*hour==2&&*min==0&&*sec==0)
          {
           if(dst_status==0)
            {
             *hour=*hour+1;
             dst_status = 1;
             return 1;
            }
          } else dst_status = 0;
        }
      } else {
       if( *mon == 10 )
        {
         if( (*day+7) > 31 )
          {
           if(dst_status==0)
            {
             if(*hour==3&&*min==0&&*sec==0)
              {
               *hour=*hour-1;
               dst_status = 1;
               return 1;
              }
            } else if(*hour<2||*hour>=3) dst_status=0;
          }
        }
      }
    }  
   return 0;
  }
