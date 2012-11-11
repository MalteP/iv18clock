// #############################################################################
// #                   --- IW18 VFD Clock Firmware ---                         #
// #############################################################################
// # ui.c - User interface                                                     #
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
 #include <util/delay.h>
 #include <avr/wdt.h>
 #include <avr/interrupt.h>
 #include <avr/eeprom.h>
 #include "libs/vfd.h"
 #include "libs/i2c.h"
 #include "libs/key.h"
 #include "libs/isd.h"
 #include "libs/adc.h"
 #include "libs/uart.h"
 #include "libs/alarm.h"
 #include "libs/random.h"
 #include "libs/isd.h"
 #include "libs/moon.h"
 #include "libs/rfm.h"
 #include "libs/ui.h"
 #include "libs/portdef.h"

 // Reset vector for bootloader
 // (16 bit words, 32768 x 8bit -> 16384 words = 0x4000 words ) 
 // loader size is 256 words = 0x0100. 
 // 0x4000 - 0x200 = 0x3F00
 void ( *loaderreset )( void ) = ( void* ) 0x3F00;

 // EEP Values
 uint8_t eeVolume EEMEM;
 uint8_t eeWsound EEMEM;
 uint8_t eeAlarm EEMEM;
 uint8_t eeDim EEMEM;
 tAlarm eeAlrm[4] EEMEM;
 uint16_t eeDimval EEMEM;
 uint8_t eeAlarmtime EEMEM;
 uint8_t eeSnoozetime EEMEM;
 uint8_t eeTries EEMEM;
 uint8_t eeDelimiter_h EEMEM;
 uint8_t eeDelimiter_m EEMEM;
 uint8_t eeWirelessFunction EEMEM;
 uint8_t eeWirelessDcf EEMEM;
 uint8_t eeWirelessAlarmMode_On EEMEM;
 uint8_t eeWirelessAlarmOff EEMEM;
 uint8_t eeRemoteAddr EEMEM;
 uint8_t eeRemoteCode EEMEM;
 uint8_t eeDaylightSavingTime EEMEM;
 uint8_t eeAlarmMode EEMEM;


 void UI( void ) 
  {
   uint8_t menue = 0;
   uint8_t menue_tmp;
   uint8_t submenue = 0;
   uint8_t submenue_tmp;
   uint8_t hour, min, sec, wday, day, mon, year;
   uint8_t hour_tmp = 0;
   uint8_t min_tmp = 0;
   uint8_t sec_tmp = 0;
   uint8_t wday_tmp = 0;
   uint8_t day_tmp = 0;
   uint8_t mon_tmp = 0;
   uint8_t year_tmp = 0;
   uint8_t ds75high, ds75low, ds75sign;
   uint8_t outhigh = 0;
   uint8_t outlow = 0;
   uint8_t outsign = 0;
   uint16_t outvalid = 0;
   uint8_t alarm_tmp = 0; 
   uint8_t moon = 0;
   uint8_t dim;
   uint16_t dimval;
   uint8_t uart;
   uint8_t volume;  
   uint16_t blink_pre = 0;
   uint8_t blink = 0;
   uint8_t wsound;
   uint8_t delimiter_h;
   uint8_t delimiter_m;
   uint8_t sounds = CountSounds();
   uint8_t wireless_function;
   uint8_t wireless_switch_flag=0;   
   uint8_t wireless_dcf;
   uint8_t wireless_alarm_mode_on;
   uint8_t wireless_alarm_off;
   uint16_t wireless_alarm_off_timer=0;
   uint8_t remote_addr;
   uint8_t remote_code;
   uint8_t daylight_saving_time;
   uint8_t alarm_mode;
   InitRandom(sounds-1);

   // Init
   segdata[0]=59;
   segdata[1]=59;
   segdata[2]=59;
   segdata[3]=28;
   segdata[4]=33;
   segdata[5]=28;
   segdata[6]=39;
   segdata[7]=59; 
   segdata[8]=59;

   // Status register valid?
   if(DS1338read_status()!=DS1338STATUS)
    {
     DS1338write_status(DS1338STATUS);
    }

   // Invalid data in RTC?
   DS1338read( &sec, &min, &hour, &wday, &day, &mon, &year );
   if(sec>59||min>59||hour>23)
    {
     DS1338write_time(0,0,12);
     DS1338write_date(4,11,11,10);
    } 

   // Check data in eep
   volume = eeprom_read_byte(&eeVolume);
   if(volume>7)
    {
     volume = 2;
     eeprom_write_byte(&eeVolume, volume);
    } 
   SetVolume(volume);

   wsound = eeprom_read_byte(&eeWsound);
   if(wsound>sounds)
    {
     wsound = 1;
     eeprom_write_byte(&eeWsound, wsound);
    } 

   alarm = eeprom_read_byte(&eeAlarm);
   if(alarm>15)
    {
     alarm = 0;
     eeprom_write_byte(&eeAlarm, alarm);
    } 

   dim = eeprom_read_byte(&eeDim);
   if(dim>4)
    {
     dim=0;
     eeprom_write_byte(&eeDim, dim);
    } 

   dimval = eeprom_read_word(&eeDimval);
   if(dimval>2048)
    {
     dimval=768;
     eeprom_write_word(&eeDimval, dimval);
    } 

   eeprom_read_block((void *)&alrm[0],&eeAlrm[0],sizeof(tAlarm));
   if(alrm[0].hour>23||alrm[0].min>59)
    {
     alrm[0].hour = 6;
     alrm[0].min = 0;
     eeprom_write_block((void *)&alrm[0],&eeAlrm[0],sizeof(tAlarm));
    }

   eeprom_read_block((void *)&alrm[1],&eeAlrm[1],sizeof(tAlarm));
   if(alrm[1].hour>23||alrm[1].min>59)
    {
     alrm[1].hour = 7;
     alrm[1].min = 0;
     eeprom_write_block((void *)&alrm[1],&eeAlrm[1],sizeof(tAlarm));
    }

   eeprom_read_block((void *)&alrm[2],&eeAlrm[2],sizeof(tAlarm));
   if(alrm[2].hour>23||alrm[2].min>59)
    {
     alrm[2].hour = 8;
     alrm[2].min = 0;
     eeprom_write_block((void *)&alrm[2],&eeAlrm[2],sizeof(tAlarm));
    }

   eeprom_read_block((void *)&alrm[3],&eeAlrm[3],sizeof(tAlarm));
   if(alrm[3].hour>23||alrm[3].min>59)
    {
     alrm[3].hour = 9;
     alrm[3].min = 0;
     eeprom_write_block((void *)&alrm[3],&eeAlrm[3],sizeof(tAlarm));
    }   

   alarmtime = eeprom_read_byte(&eeAlarmtime);
   if(alarmtime==0||alarmtime>10)
    {
     alarmtime=1;
     eeprom_write_byte(&eeAlarmtime, alarmtime);
    }

   snoozetime = eeprom_read_byte(&eeSnoozetime);
   if(snoozetime>10)
    {
     snoozetime=0;
     eeprom_write_byte(&eeSnoozetime, snoozetime);
    }
    
   tries = eeprom_read_byte(&eeTries);
   if(tries<2||tries>10)
    {
     tries=2;
     eeprom_write_byte(&eeTries, tries);
    }
    
   delimiter_h = eeprom_read_byte(&eeDelimiter_h);
   if(delimiter_h>7)
    {
     delimiter_h=0;
     eeprom_write_byte(&eeDelimiter_h, delimiter_h);
    }

   delimiter_m = eeprom_read_byte(&eeDelimiter_m);
   if(delimiter_m>7)
    {
     delimiter_m=0;
     eeprom_write_byte(&eeDelimiter_m, delimiter_m);
    }
    
   wireless_function = eeprom_read_byte(&eeWirelessFunction);
   if(wireless_function>1)
    {
     wireless_function=0;
     eeprom_write_byte(&eeWirelessFunction, wireless_function);
    }
   SetWireless(wireless_function);

   wireless_dcf = eeprom_read_byte(&eeWirelessDcf);
   if(wireless_dcf>1)
    {
     wireless_dcf=0;
     eeprom_write_byte(&eeWirelessDcf, wireless_dcf);
    }

   wireless_alarm_mode_on = eeprom_read_byte(&eeWirelessAlarmMode_On);
   if(wireless_alarm_mode_on>3)
    {
     wireless_alarm_mode_on=0;
     eeprom_write_byte(&eeWirelessAlarmMode_On, wireless_alarm_mode_on);
    }
    
   wireless_alarm_off = eeprom_read_byte(&eeWirelessAlarmOff);
   if(wireless_alarm_off>30)
    {
     wireless_alarm_off=0;
     eeprom_write_byte(&eeWirelessAlarmOff, wireless_alarm_off);
    }

   remote_addr = eeprom_read_byte(&eeRemoteAddr);
   if(remote_addr>31)
    {
     remote_addr=0;
     eeprom_write_byte(&eeRemoteAddr, remote_addr);
    }

   remote_code = eeprom_read_byte(&eeRemoteCode);
   if(remote_code>15)
    {
     remote_code=0;
     eeprom_write_byte(&eeRemoteCode, remote_code);
    }

   daylight_saving_time = eeprom_read_byte(&eeDaylightSavingTime);
   if(daylight_saving_time>1)
    {
     daylight_saving_time=0;
     eeprom_write_byte(&eeDaylightSavingTime, daylight_saving_time);
    }   

   alarm_mode = eeprom_read_byte(&eeAlarmMode);
   if(alarm_mode>1)
    {
     alarm_mode=0;
     eeprom_write_byte(&eeAlarmMode, alarm_mode);
    }

   // Init finished, start main loop
   while(1)
    {
     GetKeys();
     PollISD();
     PollAlarm( hour, min, sec, wsound );
     PollRFM();

     // Cursor blink and rotation for "random" data
     blink_pre++;
     if(blink_pre>8192) 
      {
       blink_pre=0;
       if(blink==0) blink=1; else blink=0;
       RotateRandom();
      }
		 
     if( ClkChng==1 )
      {
       ClkChng=0; 
       wdt_reset();
       // Read time
       DS1338read( &sec, &min, &hour, &wday, &day, &mon, &year );
       // Daylight saving time automatic enabled?
       if(daylight_saving_time!=0)
        {
         if(DaylightSaving( &sec, &min, &hour, &wday, &day, &mon )!=0) DS1338write_time( sec, min, hour );
        }
       // Read temperature
       DS75read( &ds75high, &ds75low, &ds75sign );
       // Should be called every second (for snooze function)
       TickAlarm();       
       // Check if we can reduce display brightness
       if(dim!=0) { if(GetADC(0)>dimval&&!RunningAlarm()) SetDim(dim); else SetDim(0); } else SetDim(0);
       // New calculation if needed
       moon = 0xFF;
       // Timer for wireless plug auto switch off
       if(wireless_alarm_off_timer>0) wireless_alarm_off_timer--;
       // Timer for valid data from wireless temperature sensor
       if(outvalid>0) outvalid--;
      }

     // If this is a new alarm, enable wireless plug if needed
     // and if finished start alarm immediately. Also start 
     // timer to switch off the plug if alarm and snooze is finished!
     if(AlarmWait())
      {
       if(!wireless_alarm_mode_on&(1<<0))
        {
         // Plug disabled
         AlarmSetInitFinished();
        } else {
         // Plug enabled, trigger first then set init to finished if sent
         if(wireless_switch_flag==0||wireless_switch_flag==2)
          {
           wireless_switch_flag=1;
           if(!(wireless_alarm_mode_on&(1<<1))) wireless_switch_2( remote_addr, remote_code, PLUG_SWITCH_2_ON );
            else wireless_switch_4( remote_addr, remote_code, PLUG_SWITCH_4_ON );  
          } else {
           if(!wireless_switch_running()) AlarmSetInitFinished();          
          }
        }
      } else {
       if(wireless_alarm_mode_on&(1<<0)&&wireless_alarm_off!=0) // Switch on function enabled and switch off function enabled
        {
         if(!CanDisableAlarm()) // Alarm / snooze not running anymore
          {
           if(wireless_switch_flag==1)
            {
              wireless_alarm_off_timer = 60 * wireless_alarm_off; // Set timer
              wireless_switch_flag=2;
            } else if(wireless_switch_flag==2)
             {
              if(wireless_alarm_off_timer==0) // Timer zero?
               {
                if(!(wireless_alarm_mode_on&(1<<1))) wireless_switch_2( remote_addr, remote_code, PLUG_SWITCH_2_OFF );
                 else wireless_switch_4( remote_addr, remote_code, PLUG_SWITCH_4_OFF );
                wireless_switch_flag=0;
               }              
             }
          }
        } else wireless_switch_flag=0;
      } 

     // First grid of the vfd has two segments (dot for alarm and bar for snooze active)
     if(alarm!=0) { if(blink==0&&RunningSnooze()) segdata[0]=56; else segdata[0]=55; } else segdata[0]=59;
		 
     menue_tmp = menue;
     switch( menue_tmp )
      {
       case 0:
        // Screen #00 Time HH:MM:SS
        DisplayConvert( hour, ( void* ) segdata+1, ( void* ) segdata+2 );
        segdata[3]=delimiter_convert_h(delimiter_h); 
        DisplayConvert( min, ( void* ) segdata+4, ( void* ) segdata+5 );
        segdata[6]=delimiter_convert_m(delimiter_m);
        DisplayConvert( sec, ( void* ) segdata+7, ( void* ) segdata+8 );
        if(keys & (1<<KEY_OK)) { if(CanDisableAlarm()) { DisableAlarm(); } else { SayTime( hour, min ); } keys |= (1<<KEY_HANDLED); }
        if(keys & (1<<KEY_PLUS)) { menue++; keys |= (1<<KEY_HANDLED); }
        if(keys & (1<<KEY_MINUS)) { keys |= (1<<KEY_HANDLED); }
        break;
       case 1:
        // Screen #01 IN Temperature +xx.x °C
        segdata[1]=28;
        segdata[2]=33;
        if( ds75sign ) segdata[3]=49; else segdata[3]=59;
        DisplayConvertDot( ds75high, ( void* ) segdata+4, ( void* ) segdata+5 );
        segdata[6]=ds75low;
        segdata[7]=46;
        segdata[8]=22;
        if(keys & (1<<KEY_OK)) { if(CanDisableAlarm()) { DisableAlarm(); } keys |= (1<<KEY_HANDLED); }
        if(keys & (1<<KEY_PLUS)) { if(outvalid!=0) { menue++; } else { menue +=2; } keys |= (1<<KEY_HANDLED); }
        if(keys & (1<<KEY_MINUS)) { menue--; keys |= (1<<KEY_HANDLED); }
        break;
       case 2:
        // Screen #02 OUT Temperature +xx.x °C
        segdata[1]=34;
        segdata[2]=40;
        if( outvalid != 0)
         {
          if( outsign ) segdata[3]=49; else segdata[3]=59;
          DisplayConvertDot( outhigh, ( void* ) segdata+4, ( void* ) segdata+5 );
          segdata[6]=outlow;
         } else {
          segdata[3]=59;
          segdata[4]=49;
          segdata[5]=56;
          segdata[6]=49;
         }
        segdata[7]=46;
        segdata[8]=22;
        if(keys & (1<<KEY_OK)) { if(CanDisableAlarm()) { DisableAlarm(); } keys |= (1<<KEY_HANDLED); }
        if(keys & (1<<KEY_PLUS)) { menue++; keys |= (1<<KEY_HANDLED); }
        if(keys & (1<<KEY_MINUS)) { menue--; keys |= (1<<KEY_HANDLED); }
        break;
       case 3:
        // Screen #03 Weekday & Date WWDD.MM.YY
        WeekdayConvert( wday, ( void* ) segdata+1, ( void* ) segdata+2 );
        DisplayConvertDot( day, ( void* ) segdata+3, ( void* ) segdata+4 );
        DisplayConvertDot( mon, ( void* ) segdata+5, ( void* ) segdata+6 );
        DisplayConvert( year, ( void* ) segdata+7, ( void* ) segdata+8 );
        if(keys & (1<<KEY_OK)) { if(CanDisableAlarm()) { DisableAlarm(); } keys |= (1<<KEY_HANDLED); }
        if(keys & (1<<KEY_PLUS)) { menue++; keys |= (1<<KEY_HANDLED); }
        if(keys & (1<<KEY_MINUS)) { if(outvalid!=0) { menue--; } else { menue -=2; } keys |= (1<<KEY_HANDLED); }
        break;
       case 4:
        // Screen #04 Moon phase
        segdata[1]=32;
        segdata[2]=34;
        segdata[3]=34;
        segdata[4]=33;
        segdata[5]=59;
        if(moon==0xFF) moon = Moon_phase((2000+year), mon, day); // Only calculated one time per second
        segdata[6]=moon_convert_a(moon);
        segdata[7]=moon_convert_b(moon);
        segdata[8]=59;
        if(keys & (1<<KEY_OK)) { if(CanDisableAlarm()) { DisableAlarm(); } keys |= (1<<KEY_HANDLED); }
        if(keys & (1<<KEY_PLUS)) { menue++; keys |= (1<<KEY_HANDLED); }
        if(keys & (1<<KEY_MINUS)) { menue--; keys |= (1<<KEY_HANDLED); }        
        break;
       case 5:
        // Screen #05 Choose alarm
        if(alarm_mode==0)
         {
          // Single alarm time
          submenue_tmp = submenue;
          switch( submenue_tmp )
           {
            case 0:
             if(alarm&(1<<0)) alarm_tmp=1;
              else if(alarm&(1<<1)) alarm_tmp=2;
               else if(alarm&(1<<2)) alarm_tmp=3;
                else if(alarm&(1<<3)) alarm_tmp=4;    
                 else alarm_tmp=0;
             if(alarm_tmp!=0)
              {
               segdata[1]=20;
               segdata[2]=31;
               segdata[3]=alarm_tmp;
               segdata[4]=59;
               DisplayConvertDot( alrm[alarm_tmp-1].hour, ( void* ) segdata+5, ( void* ) segdata+6 );
               DisplayConvert( alrm[alarm_tmp-1].min, ( void* ) segdata+7, ( void* ) segdata+8 );             
              } else {
               segdata[1]=20;
               segdata[2]=31;
               segdata[3]=59;
               segdata[4]=59;
               segdata[5]=34;
               segdata[6]=25;
               segdata[7]=25;
               segdata[8]=59;             
              }      
             if(keys & (1<<KEY_OK)) { if(CanDisableAlarm()) { DisableAlarm(); } else { submenue++; } keys |= (1<<KEY_LONG); keys |= (1<<KEY_HANDLED); }
             if(keys & (1<<KEY_PLUS)) { menue++; keys |= (1<<KEY_HANDLED); }
             if(keys & (1<<KEY_MINUS)) { menue--; keys |= (1<<KEY_HANDLED); }
             break;
            case 1:
             if(alarm_tmp!=0)
              {
               segdata[1]=20;
               segdata[2]=31;
               if(blink==0) segdata[3]=alarm_tmp; else segdata[3]=59;
               segdata[4]=59;
               DisplayConvertDot( alrm[alarm_tmp-1].hour, ( void* ) segdata+5, ( void* ) segdata+6 );
               DisplayConvert( alrm[alarm_tmp-1].min, ( void* ) segdata+7, ( void* ) segdata+8 );             
              } else {
               segdata[1]=20;
               segdata[2]=31;
               segdata[3]=59;
               segdata[4]=59;
               if(blink==0)
                {
                 segdata[5]=34;
                 segdata[6]=25;
                 segdata[7]=25; 
                } else {
                 segdata[5]=59;
                 segdata[6]=59;
                 segdata[7]=59;                 
                }   
               segdata[8]=59;              
              }
             if(keys & (1<<KEY_OK)) { if(CanDisableAlarm()) { DisableAlarm(); } else { if(keys & (1<<KEY_HOLD)) { alarm_tmp=0; } submenue++; } keys &= ~(1<<KEY_LONG); keys |= (1<<KEY_HANDLED); }
             if(keys & (1<<KEY_PLUS)) { alarm_tmp++; if(alarm_tmp>4) alarm_tmp=0; keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
             if(keys & (1<<KEY_MINUS)) { if(alarm_tmp==0) alarm_tmp=4; else alarm_tmp--; keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
             break;            
            case 2:
             if(alarm_tmp!=0)
              { 
               alarm=(1<<(alarm_tmp-1));
              } else {
               alarm=0;
              }
             eeprom_write_byte(&eeAlarm, alarm);
             submenue = 0;
             break;
            default:
             submenue = 0;
             break;
           }
         } else {
          // Multiple alarm times
          submenue_tmp = submenue;
          switch( submenue_tmp )
           {
            case 0:
             if(alarm!=0)
              {
               segdata[1]=20;
               segdata[2]=31;
               segdata[3]=59;
               segdata[4]=59;
               segdata[5]=34;
               segdata[6]=33;
               segdata[7]=59;
               segdata[8]=59;             
              } else {
               segdata[1]=20;
               segdata[2]=31;
               segdata[3]=59;
               segdata[4]=59;
               segdata[5]=34;
               segdata[6]=25;
               segdata[7]=25;
               segdata[8]=59;             
              }
             if(keys & (1<<KEY_OK)) { if(CanDisableAlarm()) { DisableAlarm(); } else { submenue++; alarm_tmp=1; } keys |= (1<<KEY_LONG); keys |= (1<<KEY_HANDLED); }
             if(keys & (1<<KEY_PLUS)) { menue++; keys |= (1<<KEY_HANDLED); }
             if(keys & (1<<KEY_MINUS)) { menue--; keys |= (1<<KEY_HANDLED); }            
             break;
            case 1:
             segdata[1]=20;
             segdata[2]=31;
             segdata[3]=alarm_tmp;
             segdata[4]=59;
             if(alarm&(1<<(alarm_tmp-1)))
              {
               if(blink==0)
                {
                 DisplayConvertDot( alrm[alarm_tmp-1].hour, ( void* ) segdata+5, ( void* ) segdata+6 );
                 DisplayConvert( alrm[alarm_tmp-1].min, ( void* ) segdata+7, ( void* ) segdata+8 ); 
                } else {
                 segdata[5]=59;
                 segdata[6]=59;
                 segdata[7]=59;
                 segdata[8]=59; 
                }
              } else {
               if(blink==0)
                {
                 segdata[5]=34;
                 segdata[6]=25;
                 segdata[7]=25;
                } else {
                 segdata[5]=59;
                 segdata[6]=59;
                 segdata[7]=59;
                }
               segdata[8]=59;         
              }
             if(keys & (1<<KEY_OK)) { if(CanDisableAlarm()) { DisableAlarm(); } else { if(keys & (1<<KEY_HOLD)) { alarm=0; submenue++; } else { if(alarm_tmp<4) alarm_tmp++; else submenue++; } } keys &= ~(1<<KEY_LONG); keys |= (1<<KEY_HANDLED); }
             if(keys & (1<<KEY_PLUS)) { alarm |= (1<<(alarm_tmp-1)); keys |= (1<<KEY_HANDLED); }
             if(keys & (1<<KEY_MINUS)) { alarm &= ~(1<<(alarm_tmp-1)); keys |= (1<<KEY_HANDLED); }             
             break;
            case 2:
             eeprom_write_byte(&eeAlarm, alarm);
             submenue = 0;
             break;
            default:
             submenue = 0;
             break;            
           }         
         }		  
        break; 
       case 6:
        // Screen #06 Goto setup?
        segdata[1]=59;
        segdata[2]=38;
        segdata[3]=24;
        segdata[4]=39;
        segdata[5]=40;
        segdata[6]=35;
        segdata[7]=59;
        segdata[8]=59;
        if(keys & (1<<KEY_OK)) { if(CanDisableAlarm()) { DisableAlarm(); } else { menue++; } keys |= (1<<KEY_HANDLED); }
        if(keys & (1<<KEY_PLUS)) { keys |= (1<<KEY_HANDLED); }
        if(keys & (1<<KEY_MINUS)) { menue--; keys |= (1<<KEY_HANDLED); }
        break;
       case 7:
        // Screen #07 Back to main?
        segdata[1]=59;
        segdata[2]=21;
        segdata[3]=20;
        segdata[4]=22;
        segdata[5]=30;
        segdata[6]=59;
        segdata[7]=59;
        segdata[8]=59;
        if(keys & (1<<KEY_OK)) { menue = 0; keys |= (1<<KEY_HANDLED); }
        if(keys & (1<<KEY_PLUS)) { menue++; keys |= (1<<KEY_HANDLED); }
        if(keys & (1<<KEY_MINUS)) { keys |= (1<<KEY_HANDLED); }
        break;
       case 8:
        // Screen #08 Setup alarm time
        submenue_tmp = submenue;
        switch( submenue_tmp )
         {
          case 0:
           // Enter to setup 
           segdata[1]=59;
           segdata[2]=20;
           segdata[3]=31;
           segdata[4]=59;
           segdata[5]=38;
           segdata[6]=24;
           segdata[7]=39; 
           segdata[8]=59;
           if(keys & (1<<KEY_OK)) { submenue++; alarm_tmp=1; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { menue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_MINUS)) { menue--; keys |= (1<<KEY_HANDLED); }
           break;
          case 1:
           // Choose one of four alarm times to modify
           segdata[1]=20;
           segdata[2]=31;
           if(blink==0) segdata[3]=alarm_tmp; else segdata[3]=59;           
           segdata[4]=59;
           DisplayConvertDot( alrm[alarm_tmp-1].hour, ( void* ) segdata+5, ( void* ) segdata+6 );
           DisplayConvert( alrm[alarm_tmp-1].min, ( void* ) segdata+7, ( void* ) segdata+8 );
           if(keys & (1<<KEY_OK)) { submenue++; if(alarm_tmp==alarm) alarm=0; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(alarm_tmp<4) { alarm_tmp++; } else { alarm_tmp=1; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(alarm_tmp>1) { alarm_tmp--; } else { alarm_tmp=4; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           break;
          case 2:
           // Set hours
           segdata[1]=20;
           segdata[2]=31;
           segdata[3]=alarm_tmp;           
           segdata[4]=59;
           if(blink==0)
            {
             DisplayConvertDot( alrm[alarm_tmp-1].hour, ( void* ) segdata+5, ( void* ) segdata+6 );
            } else {
             segdata[5]=59;
             segdata[6]=55;
            }
           DisplayConvert( alrm[alarm_tmp-1].min, ( void* ) segdata+7, ( void* ) segdata+8 );
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(alrm[alarm_tmp-1].hour<23) { alrm[alarm_tmp-1].hour++; } else { alrm[alarm_tmp-1].hour=0; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(alrm[alarm_tmp-1].hour>0) { alrm[alarm_tmp-1].hour--; } else { alrm[alarm_tmp-1].hour=23; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           break;
          case 3:
           // Set minutes
           segdata[1]=20;
           segdata[2]=31;
           segdata[3]=alarm_tmp;           
           segdata[4]=59;
           DisplayConvertDot( alrm[alarm_tmp-1].hour, ( void* ) segdata+5, ( void* ) segdata+6 );
           if(blink==0)
            {
             DisplayConvert( alrm[alarm_tmp-1].min, ( void* ) segdata+7, ( void* ) segdata+8 );
            } else {
             segdata[7]=59;
             segdata[8]=59;
            }
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(alrm[alarm_tmp-1].min<59) { alrm[alarm_tmp-1].min++; } else { alrm[alarm_tmp-1].min=0; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(alrm[alarm_tmp-1].min>0) { alrm[alarm_tmp-1].min--; } else { alrm[alarm_tmp-1].min=59; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           break;
          case 4:
           // Save to eep
           eeprom_write_block((void *)&alrm[alarm_tmp-1],&eeAlrm[alarm_tmp-1],sizeof(tAlarm));
           if(alarm==0) // Activate modified alarm now if no other active
            {
             alarm=(1<<(alarm_tmp-1));
             eeprom_write_byte(&eeAlarm, alarm);
            }
           submenue=0;
           break;
          default:
           submenue=0;
           break;
         }
        break;
       case 9: 
        // Screen #09 Setup alarm sound
        submenue_tmp = submenue;
        switch( submenue_tmp )
         {
          case 0:  
           // Enter to setup 
           segdata[1]=38;
           segdata[2]=33;
           segdata[3]=23;
           segdata[4]=59;
           segdata[5]=38;
           segdata[6]=24;
           segdata[7]=39; 
           segdata[8]=59;
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { menue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_MINUS)) { menue--; keys |= (1<<KEY_HANDLED); }
           break;
          case 1:
           segdata[1]=38;
           segdata[2]=33;
           segdata[3]=23;
           segdata[4]=59;
           if(blink==0)
            {
             if(wsound!=0)
              {
               DisplayConvert( wsound, ( void* ) segdata+5, ( void* ) segdata+6 );
               segdata[7]=59; 
              } else {
               segdata[5]=37; 
               segdata[6]=33; 
               segdata[7]=23; 
              }
             } else {
              segdata[5]=59; 
              segdata[6]=59; 
              segdata[7]=59; 
             }
           segdata[8]=59;
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(wsound<(sounds)) { wsound++; } else { wsound=0; } if(wsound!=0) PlaySound(wsound-1); else PlaySound(GetRandom()); keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(wsound>0) { wsound--; } else { wsound=(sounds); } if(wsound!=0) PlaySound(wsound-1); else PlaySound(GetRandom()); keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           break;
          case 2:
           eeprom_write_byte(&eeWsound, wsound);
           submenue=0;
           break;
          default:
           submenue=0;
           break;
         }
        break;
       case 10:
        // Screen #10 Setup volume
        submenue_tmp = submenue;
        switch( submenue_tmp )
         {
          case 0:  
           // Enter to setup 
           segdata[1]=41;
           segdata[2]=34;
           segdata[3]=31;
           segdata[4]=59;
           segdata[5]=38;
           segdata[6]=24;
           segdata[7]=39; 
           segdata[8]=59;
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { menue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_MINUS)) { menue--; keys |= (1<<KEY_HANDLED); }
           break;
          case 1:
           // Choose volume
           segdata[1]=41;
           segdata[2]=34;
           segdata[3]=31; 
           segdata[4]=59;
           if(blink==0) segdata[5]=volume; else segdata[5]=59;
           segdata[6]=59;
           segdata[7]=59;
           segdata[8]=59;           
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(volume<7) { volume++; SetVolume(volume); if(volume!=1) PlayNumber(volume); else PlayNumber(17); if(volume<7) keys |= (1<<KEY_REPEAT); } else { volume=7; SetVolume(volume); PlayNumber(volume); } keys |= (1<<KEY_HANDLED); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(volume>0) { volume--; SetVolume(volume); if(volume!=1) PlayNumber(volume); else PlayNumber(17); if(volume>0) keys |= (1<<KEY_REPEAT); } else { volume=0; SetVolume(volume); PlayNumber(volume); } keys |= (1<<KEY_HANDLED); blink=0; blink_pre=0; }           
           break;
          case 2:
           // Save to eep
           eeprom_write_byte(&eeVolume, volume);
           submenue=0;
           break;
          default:
           submenue=0;
           break; 
         }
        break;
       case 11:
        // Screen #11 Setup time
        submenue_tmp = submenue;
        switch( submenue_tmp )
         {
          case 0:  
           // Enter to setup 
           segdata[1]=59;
           segdata[2]=39;
           segdata[3]=28;
           segdata[4]=59;
           segdata[5]=38;
           segdata[6]=24;
           segdata[7]=39; 
           segdata[8]=59;
           if(keys & (1<<KEY_OK)) { submenue++; sec_tmp=sec; min_tmp=min; hour_tmp=hour; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { menue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_MINUS)) { menue--; keys |= (1<<KEY_HANDLED); }
           break;
          case 1:
           // Setup hours
           if(blink==0)
            {
             DisplayConvert( hour_tmp, ( void* ) segdata+1, ( void* ) segdata+2 );
            } else {
             segdata[1]=59;
             segdata[2]=59;
            }
           segdata[3]=delimiter_convert_h(delimiter_h);
           DisplayConvert( min_tmp, ( void* ) segdata+4, ( void* ) segdata+5 );
           segdata[6]=delimiter_convert_m(delimiter_m);
           DisplayConvert( sec_tmp, ( void* ) segdata+7, ( void* ) segdata+8 );
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(hour_tmp<23) { hour_tmp++; } else { hour_tmp=0; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(hour_tmp>0) { hour_tmp--; } else { hour_tmp=23; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }           
           break;
          case 2:
           // Setup min
           DisplayConvert( hour_tmp, ( void* ) segdata+1, ( void* ) segdata+2 );
           segdata[3]=delimiter_convert_h(delimiter_h); 
           if(blink==0)
            {
             DisplayConvert( min_tmp, ( void* ) segdata+4, ( void* ) segdata+5 );
            } else {
             segdata[4]=59;
             segdata[5]=59;
            }
           segdata[6]=delimiter_convert_m(delimiter_m);
           DisplayConvert( sec_tmp, ( void* ) segdata+7, ( void* ) segdata+8 );
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(min_tmp<59) { min_tmp++; } else { min_tmp=0; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(min_tmp>0) { min_tmp--; } else { min_tmp=59; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }           
           break;
          case 3:
           // Setup sec
           DisplayConvert( hour_tmp, ( void* ) segdata+1, ( void* ) segdata+2 );
           segdata[3]=delimiter_convert_h(delimiter_h); 
           DisplayConvert( min_tmp, ( void* ) segdata+4, ( void* ) segdata+5 );
           segdata[6]=delimiter_convert_m(delimiter_m);
           if(blink==0)
            {
             DisplayConvert( sec_tmp, ( void* ) segdata+7, ( void* ) segdata+8 );
            } else {
             segdata[7]=59;
             segdata[8]=59;
            }
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(sec_tmp<59) { sec_tmp++; } else { sec_tmp=0; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(sec_tmp>0) { sec_tmp--; } else { sec_tmp=59; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }           
           break;
          case 4:
           // Save to RTC
           DS1338write_time( sec_tmp, min_tmp, hour_tmp );
           submenue = 0;
           break;
          default:
           submenue = 0;
           break; 
         }
        break;
       case 12:
        // Screen #12 Setup date
        submenue_tmp = submenue;
        switch( submenue_tmp )
         {
          case 0:  
           // Enter to setup 
           segdata[1]=59;
           segdata[2]=23;
           segdata[3]=20;
           segdata[4]=59;
           segdata[5]=38;
           segdata[6]=24;
           segdata[7]=39; 
           segdata[8]=59;
           if(keys & (1<<KEY_OK)) { submenue++; wday_tmp=wday; day_tmp=day; mon_tmp=mon; year_tmp=year; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { menue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_MINUS)) { menue--; keys |= (1<<KEY_HANDLED); }
           break;
          case 1:
           // Setup weekday
           if(blink==0)
            {
             WeekdayConvert( wday_tmp, ( void* ) segdata+1, ( void* ) segdata+2 );
            } else {
             segdata[1]=59;
             segdata[2]=59;
            }
           DisplayConvertDot( day_tmp, ( void* ) segdata+3, ( void* ) segdata+4 );
           DisplayConvertDot( mon_tmp, ( void* ) segdata+5, ( void* ) segdata+6 );
           DisplayConvert( year_tmp, ( void* ) segdata+7, ( void* ) segdata+8 );
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(wday_tmp<7) { wday_tmp++; } else { wday_tmp=1; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(wday_tmp>1) { wday_tmp--; } else { wday_tmp=7; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }           
           break;
          case 2:
           // Setup day
           WeekdayConvert( wday_tmp, ( void* ) segdata+1, ( void* ) segdata+2 );
           if(blink==0)
            {
             DisplayConvertDot( day_tmp, ( void* ) segdata+3, ( void* ) segdata+4 );
            } else {
             segdata[3]=59;
             segdata[4]=55;
            }
           DisplayConvertDot( mon_tmp, ( void* ) segdata+5, ( void* ) segdata+6 );
           DisplayConvert( year_tmp, ( void* ) segdata+7, ( void* ) segdata+8 );
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(day_tmp<31) { day_tmp++; } else { day_tmp=1; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(day_tmp>1) { day_tmp--; } else { day_tmp=31; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }           
           break;
          case 3:
           // Setup month
           WeekdayConvert( wday_tmp, ( void* ) segdata+1, ( void* ) segdata+2 );
           DisplayConvertDot( day_tmp, ( void* ) segdata+3, ( void* ) segdata+4 );
           if(blink==0)
            {
             DisplayConvertDot( mon_tmp, ( void* ) segdata+5, ( void* ) segdata+6 );
            } else {
             segdata[5]=59;
             segdata[6]=55;
            }
           DisplayConvert( year_tmp, ( void* ) segdata+7, ( void* ) segdata+8 );
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(mon_tmp<12) { mon_tmp++; } else { mon_tmp=1; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(mon_tmp>1) { mon_tmp--; } else { mon_tmp=12; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }           
           break;
          case 4:
           // Setup year
           WeekdayConvert( wday_tmp, ( void* ) segdata+1, ( void* ) segdata+2 );
           DisplayConvertDot( day_tmp, ( void* ) segdata+3, ( void* ) segdata+4 );
           DisplayConvertDot( mon_tmp, ( void* ) segdata+5, ( void* ) segdata+6 );
           if(blink==0)
            {
             DisplayConvert( year_tmp, ( void* ) segdata+7, ( void* ) segdata+8 );
            } else {
             segdata[7]=59;
             segdata[8]=59;
            }
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(year_tmp<99) { year_tmp++; } else { year_tmp=0; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(year_tmp>0) { year_tmp--; } else { year_tmp=99; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }           
           break;
          case 5:
           // Save to RTC
           DS1338write_date( wday_tmp, day_tmp, mon_tmp, year_tmp );
           submenue = 0;
           break;
          default:
           submenue = 0;
           break; 
         }
        break;
       case 13:
        // Screen #13 Setup time delimiter
        submenue_tmp = submenue;
        switch( submenue_tmp )
         {
          case 0:  
           // Enter to setup 
           segdata[1]=23;
           segdata[2]=24;
           segdata[3]=31;
           segdata[4]=59;
           segdata[5]=38;
           segdata[6]=24;
           segdata[7]=39; 
           segdata[8]=59;
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { menue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_MINUS)) { menue--; keys |= (1<<KEY_HANDLED); }
           break;
          case 1:
           DisplayConvert( hour, ( void* ) segdata+1, ( void* ) segdata+2 );
           if(blink==0) segdata[3]=delimiter_convert_h(delimiter_h); else segdata[3]=59;
           DisplayConvert( min, ( void* ) segdata+4, ( void* ) segdata+5 );
           segdata[6]=delimiter_convert_m(delimiter_m);
           DisplayConvert( sec, ( void* ) segdata+7, ( void* ) segdata+8 );          
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(delimiter_h<7) { delimiter_h++; } else { delimiter_h=0; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(delimiter_h>0) { delimiter_h--; } else { delimiter_h=7; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           break;
          case 2:
           DisplayConvert( hour, ( void* ) segdata+1, ( void* ) segdata+2 );
           segdata[3]=delimiter_convert_h(delimiter_h);
           DisplayConvert( min, ( void* ) segdata+4, ( void* ) segdata+5 );
           if(blink==0) segdata[6]=delimiter_convert_m(delimiter_m); else segdata[6]=59;
           DisplayConvert( sec, ( void* ) segdata+7, ( void* ) segdata+8 );          
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(delimiter_m<7) { delimiter_m++; } else { delimiter_m=0; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(delimiter_m>0) { delimiter_m--; } else { delimiter_m=7; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           break;
          case 3:
           eeprom_write_byte(&eeDelimiter_h, delimiter_h);
           eeprom_write_byte(&eeDelimiter_m, delimiter_m);
           submenue = 0;
           break;
          default:
           submenue = 0;
           break;         
         }
        break;
       case 14:
        // Screen #14 Setup snooze function
        submenue_tmp = submenue;
        switch( submenue_tmp )
         {
          case 0:  
           // Enter to setup 
           segdata[1]=38;
           segdata[2]=33;
           segdata[3]=34;
           segdata[4]=59;
           segdata[5]=38;
           segdata[6]=24;
           segdata[7]=39; 
           segdata[8]=59;
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { menue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_MINUS)) { menue--; keys |= (1<<KEY_HANDLED); }
           break;
          case 1:
           // Alarm mode (single / multi)
           segdata[1]=20;
           segdata[2]=31;
           segdata[3]=59;
           if(blink==0)
            {
             if(alarm_mode==0)
              {
               segdata[4]=38; // single
               segdata[5]=28;          
               segdata[6]=33; 
               segdata[7]=26; 
               segdata[8]=31;
              } else {
               segdata[4]=32; // multi
               segdata[5]=40;          
               segdata[6]=31; 
               segdata[7]=39; 
               segdata[8]=28;
              }
            } else {
             segdata[4]=59;
             segdata[5]=59;          
             segdata[6]=59; 
             segdata[7]=59; 
             segdata[8]=59; 
            }
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(alarm_mode==0) { alarm_mode=1; } else { alarm_mode=0; } keys |= (1<<KEY_HANDLED); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(alarm_mode==0) { alarm_mode=1; } else { alarm_mode=0; } keys |= (1<<KEY_HANDLED); blink=0; blink_pre=0; } 
           break;         
          case 2:
           alarm = 0;
           eeprom_write_byte(&eeAlarm, alarm);
           eeprom_write_byte(&eeAlarmMode, alarm_mode);
           min_tmp=alarmtime;
           submenue++;
           break;
          case 3: 
           // Alarm length
           segdata[1]=20;
           segdata[2]=31;
           if(blink==0) DisplayConvertNoZero( min_tmp, ( void* ) segdata+3, ( void* ) segdata+4 ); else { segdata[3]=59; segdata[4]=59; }
           segdata[5]=32;
           segdata[6]=28; 
           segdata[7]=33;
           segdata[8]=59;
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(min_tmp<10) { min_tmp++; } else { min_tmp=1; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(min_tmp>1) { min_tmp--; } else { min_tmp=10; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }        
           break;
          case 4:
           alarmtime=min_tmp;
           eeprom_write_byte(&eeAlarmtime, alarmtime);
           min_tmp=snoozetime; 
           submenue++;
           break;
          case 5: 
           // Snooze length
           segdata[1]=38;
           segdata[2]=33;
           if(min_tmp!=0)
            {
             if(blink==0) DisplayConvertNoZero( min_tmp, ( void* ) segdata+3, ( void* ) segdata+4 ); else { segdata[3]=59; segdata[4]=59; }
             segdata[5]=32;
             segdata[6]=28; 
             segdata[7]=33;
            } else {
             segdata[3]=59;
             if(blink==0) 
              {
               segdata[4]=34;
               segdata[5]=25;
               segdata[6]=25;
              } else {
               segdata[4]=59;
               segdata[5]=59;
               segdata[6]=59;              
              }
             segdata[7]=59;
            }
           segdata[8]=59;
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(min_tmp<10) { min_tmp++; } else { min_tmp=0; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(min_tmp>0) { min_tmp--; } else { min_tmp=10; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }        
           break;
          case 6:
           snoozetime=min_tmp;
           eeprom_write_byte(&eeSnoozetime, snoozetime);
           if(snoozetime!=0) { min_tmp=tries; submenue++; } else submenue=0;
           break;
          case 7: 
           // Tries (skip if snooze disabled)
           segdata[1]=39;
           segdata[2]=37;
           segdata[3]=28;
           segdata[4]=24; 
           segdata[5]=38;
           if(blink==0) DisplayConvertNoZero( min_tmp, ( void* ) segdata+6, ( void* ) segdata+7 ); else { segdata[6]=59; segdata[7]=59; }
           segdata[8]=59;
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(min_tmp<10) { min_tmp++; } else { min_tmp=2; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(min_tmp>2) { min_tmp--; } else { min_tmp=10; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }        
           break;
          case 8: 
           tries=min_tmp;
           eeprom_write_byte(&eeTries, tries);
           submenue = 0;
           break;
          default:
           submenue = 0;
           break;
         }
        break;
       case 15:
        // Screen #15 Setup wireless feature
        submenue_tmp = submenue;
        switch( submenue_tmp )
         {
          case 0:  
           // Enter to setup 
           segdata[1]=59;
           segdata[2]=37;
           segdata[3]=25;
           segdata[4]=59;
           segdata[5]=38;
           segdata[6]=24;
           segdata[7]=39; 
           segdata[8]=59;
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { menue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_MINUS)) { menue--; keys |= (1<<KEY_HANDLED); }
           break;
          case 1:
           // RF on / off
           segdata[1]=59;
           segdata[2]=37;
           segdata[3]=25;           
           segdata[4]=59;
           if(blink==0)
            {
             segdata[5]=34;
             if(wireless_function!=0) { segdata[6]=33; segdata[7]=59; } else { segdata[6]=25; segdata[7]=25; }
            } else {
             segdata[5]=59;
             segdata[6]=59;
             segdata[7]=59;
            }
           segdata[8]=59;
           if(keys & (1<<KEY_OK)) { eeprom_write_byte(&eeWirelessFunction, wireless_function); SetWireless(wireless_function); if(wireless_function==0) { submenue=0; } else { submenue++; } keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(wireless_function==0) { wireless_function=1; } else { wireless_function=0; } keys |= (1<<KEY_HANDLED); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(wireless_function==0) { wireless_function=1; } else { wireless_function=0; } keys |= (1<<KEY_HANDLED); blink=0; blink_pre=0; }           
           break;
          case 2:
           // Recieve DCF time via wireless
           segdata[1]=23;
           segdata[2]=22;
           segdata[3]=25;           
           segdata[4]=59;
           if(blink==0)
            {
             segdata[5]=34;
             if(wireless_dcf!=0) { segdata[6]=33; segdata[7]=59; } else { segdata[6]=25; segdata[7]=25; }
            } else {
             segdata[5]=59;
             segdata[6]=59;
             segdata[7]=59;
            }
           segdata[8]=59;
           if(keys & (1<<KEY_OK)) { eeprom_write_byte(&eeWirelessDcf, wireless_dcf); submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(wireless_dcf==0) { wireless_dcf=1; } else { wireless_dcf=0; } keys |= (1<<KEY_HANDLED); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(wireless_dcf==0) { wireless_dcf=1; } else { wireless_dcf=0; } keys |= (1<<KEY_HANDLED); blink=0; blink_pre=0; }  
           submenue++; /* TODO: dont skip this if implemented later ;-) */         
           break;
          case 3:
           // Wireless plug switch on if alarm triggers
           segdata[1]=37;
           segdata[2]=34;
           segdata[3]=33;           
           segdata[4]=59;
           if(blink==0)
            {
             if(wireless_alarm_mode_on&(1<<0)) { segdata[5]=44; segdata[6]=24; segdata[7]=38; } else { segdata[5]=33; segdata[6]=34; segdata[7]=59; }
            } else {
             segdata[5]=59;
             segdata[6]=59;
             segdata[7]=59;
            }
           segdata[8]=59;
           if(keys & (1<<KEY_OK)) { eeprom_write_byte(&eeWirelessAlarmMode_On, wireless_alarm_mode_on); if(wireless_alarm_mode_on&(1<<0)) submenue++; else submenue+=5; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(!(wireless_alarm_mode_on&(1<<0))) { wireless_alarm_mode_on |= (1<<0); } else { wireless_alarm_mode_on &= ~(1<<0); } keys |= (1<<KEY_HANDLED); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(!(wireless_alarm_mode_on&(1<<0))) { wireless_alarm_mode_on |= (1<<0); } else { wireless_alarm_mode_on &= ~(1<<0); } keys |= (1<<KEY_HANDLED); blink=0; blink_pre=0; }           
           break;
          case 4:
           // Wireless plug switch off time after alarm
           segdata[1]=37;
           segdata[2]=34;
           segdata[3]=25;           
           segdata[4]=25;           
           if(blink==0)
            {
             if(wireless_alarm_off==0)
              {
               segdata[5]=59;
               segdata[6]=33;
               segdata[7]=34;
              } else {
               DisplayConvertNoZero( wireless_alarm_off, ( void* ) segdata+5, ( void* ) segdata+6 );
               segdata[7]=32;
              }
            } else {
             segdata[5]=59;
             segdata[6]=59;
             segdata[7]=59;
            }
           segdata[8]=59;
           if(keys & (1<<KEY_OK)) { eeprom_write_byte(&eeWirelessAlarmOff, wireless_alarm_off); submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(wireless_alarm_off<30) { wireless_alarm_off++; } else { wireless_alarm_off=0; }  keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(wireless_alarm_off>0) { wireless_alarm_off--; } else { wireless_alarm_off=30; }  keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
           break;
          case 5:
           // Wireless plug protocol mode (A = Kangtai / B = Duewi)
           segdata[1]=37;
           segdata[2]=32;
           segdata[3]=34;           
           segdata[4]=23;           
           segdata[5]=24;           
           segdata[6]=59;           
           if(blink==0)
            {
             if(!(wireless_alarm_mode_on&(1<<1))) segdata[7]=20; else segdata[7]=21;
            } else {
             segdata[7]=59;
            }
           segdata[8]=59;
           if(keys & (1<<KEY_OK)) { eeprom_write_byte(&eeWirelessAlarmMode_On, wireless_alarm_mode_on); if(!(wireless_alarm_mode_on&(1<<1))) { if(remote_code>4) remote_code=4; eeprom_write_byte(&eeRemoteCode, remote_code); } else { if(remote_addr>15) remote_addr=15; eeprom_write_byte(&eeRemoteAddr, remote_addr); } submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(!(wireless_alarm_mode_on&(1<<1))) { wireless_alarm_mode_on |= (1<<1); } else { wireless_alarm_mode_on &= ~(1<<1); } keys |= (1<<KEY_HANDLED); blink=0; blink_pre=0; }
           if(keys & (1<<KEY_MINUS)) { if(!(wireless_alarm_mode_on&(1<<1))) { wireless_alarm_mode_on |= (1<<1); } else { wireless_alarm_mode_on &= ~(1<<1); } keys |= (1<<KEY_HANDLED); blink=0; blink_pre=0; }                          
           break;
          case 6:
           // Wireless remote socket address
           segdata[1]=37;
           segdata[2]=20;
           segdata[3]=23;           
           segdata[4]=59;
           if(!(wireless_alarm_mode_on&(1<<1)))
            {
             // Kangtai
             if(blink==0) DisplayConvert( remote_addr, ( void* ) segdata+5, ( void* ) segdata+6 ); else { segdata[5]=59; segdata[6]=59; }
             segdata[7]=20+remote_code;
             segdata[8]=59;
             if(keys & (1<<KEY_OK)) { eeprom_write_byte(&eeRemoteAddr, remote_addr); submenue++; keys |= (1<<KEY_HANDLED); }
             if(keys & (1<<KEY_PLUS)) { if(remote_addr<31) { remote_addr++; } else { remote_addr=0; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
             if(keys & (1<<KEY_MINUS)) { if(remote_addr>0) { remote_addr--;} else { remote_addr=31; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; } 
            } else {
             // Duewi
             if(blink==0) segdata[5]=20+remote_addr; else segdata[5]=59;            
             DisplayConvert( remote_code+1, ( void* ) segdata+6, ( void* ) segdata+7 );
             segdata[8]=59;
             if(keys & (1<<KEY_OK)) { eeprom_write_byte(&eeRemoteAddr, remote_addr); submenue++; keys |= (1<<KEY_HANDLED); }
             if(keys & (1<<KEY_PLUS)) { if(remote_addr<15) { remote_addr++; } else { remote_addr=0; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
             if(keys & (1<<KEY_MINUS)) { if(remote_addr>0) { remote_addr--;} else { remote_addr=15; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }        
            }
           break;
          case 7:
           // Wireless remote socket code
           segdata[1]=37;
           segdata[2]=20;
           segdata[3]=23;           
           segdata[4]=59;
           if(!(wireless_alarm_mode_on&(1<<1)))
            {
             // Kangtai
             DisplayConvert( remote_addr, ( void* ) segdata+5, ( void* ) segdata+6 );
             if(blink==0) segdata[7]=20+remote_code; else segdata[7]=59;
             segdata[8]=59;
             if(keys & (1<<KEY_OK)) { eeprom_write_byte(&eeRemoteCode, remote_code); submenue++; keys |= (1<<KEY_HANDLED); }
             if(keys & (1<<KEY_PLUS)) { if(remote_code<4) { remote_code++; } else { remote_code=0; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
             if(keys & (1<<KEY_MINUS)) { if(remote_code>0) { remote_code--;} else { remote_code=4; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; } 
            } else {
             // Duewi
             segdata[5]=20+remote_addr;
             if(blink==0) DisplayConvert( remote_code+1, ( void* ) segdata+6, ( void* ) segdata+7 ); else { segdata[6]=59; segdata[7]=59; }
             segdata[8]=59;
             if(keys & (1<<KEY_OK)) { eeprom_write_byte(&eeRemoteCode, remote_code); submenue++; keys |= (1<<KEY_HANDLED); }
             if(keys & (1<<KEY_PLUS)) { if(remote_code<15) { remote_code++; } else { remote_code=0; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }
             if(keys & (1<<KEY_MINUS)) { if(remote_code>0) { remote_code--;} else { remote_code=15; } keys |= (1<<KEY_HANDLED); keys |= (1<<KEY_REPEAT); blink=0; blink_pre=0; }          
            }
           break;
          default:
           submenue = 0;
           break;
         }        
        break;        
       case 16:
        // Screen #16 Setup daylight saving time auto correction
        segdata[1]=23;
        segdata[2]=38;
        segdata[3]=39;
        segdata[4]=59;
        if(daylight_saving_time==0)
         {
          segdata[5]=34;
          segdata[6]=25;
          segdata[7]=25;
          segdata[8]=59;
         } else {
          segdata[5]=20;
          segdata[6]=40;
          segdata[7]=39;
          segdata[8]=34;         
         }
        if(keys & (1<<KEY_OK)) { if(daylight_saving_time==0) daylight_saving_time=1; else daylight_saving_time=0; eeprom_write_byte(&eeDaylightSavingTime, daylight_saving_time); keys |= (1<<KEY_HANDLED); }
        if(keys & (1<<KEY_PLUS)) { menue++; keys |= (1<<KEY_HANDLED); }
        if(keys & (1<<KEY_MINUS)) { menue--; keys |= (1<<KEY_HANDLED); }
        break;
       case 17:
        // Screen #17 Setup dim feature
        submenue_tmp = submenue;
        switch( submenue_tmp )
         {
          case 0:  
           // Enter to setup 
           segdata[1]=23;
           segdata[2]=28;
           segdata[3]=32;
           segdata[4]=59;
           segdata[5]=38;
           segdata[6]=24;
           segdata[7]=39; 
           segdata[8]=59;
           if(keys & (1<<KEY_OK)) { submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_MINUS)) { menue--; keys |= (1<<KEY_HANDLED); }
           break;
          case 1:
           // Setup dim value
           segdata[1]=23;
           segdata[2]=28;
           segdata[3]=32;
           segdata[4]=59;
           if(dim!=0)
            {
             if(blink==0) segdata[5]=dim; else segdata[5]=59;
             segdata[6]=59;
             segdata[7]=59;            
            } else {
             if(blink==0)
              {
               segdata[5]=34;
               segdata[6]=25;
               segdata[7]=25;
              } else {
               segdata[5]=59;
               segdata[6]=59;
               segdata[7]=59;
              }
            }
           segdata[8]=59;
           SetDim(dim); // For preview           
           if(keys & (1<<KEY_OK)) { eeprom_write_byte(&eeDim, dim); submenue++; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_PLUS)) { if(dim<4) dim++; blink=0; keys |= (1<<KEY_HANDLED); }
           if(keys & (1<<KEY_MINUS)) { if(dim>0) dim--; blink=0; keys |= (1<<KEY_HANDLED); }           
           break;
          default:
           submenue = 0;
           break;
         }
        break;
       default:
        // This should not happen... ;-)
        menue=0;
        break;
      }
     if( (UCSRA & (1 << RXC)) ) 
      { 
       uart = UDR;
       switch(uart)
        {
         case 123: // {: bootloader reset
          DisplayStb();
          wdt_disable();
          cli();
          UCSRB &= ~(1 << RXEN);
          UCSRB &= ~(1 << TXEN);
          loaderreset();
          break;
         case 42: // *: jump to testmode
          wdt_disable();
          SetDim(0);
          TestMode();
          break;
         default:
          UDR = uart; // echo
          break;
        }
      }
    }
  }


 void TestMode( void )
  {
   uint8_t uart;
   uint8_t uart_temp = 0;
   uint16_t temp;
   segdata[0]=59;
   segdata[1]=39;
   segdata[2]=24;
   segdata[3]=38;
   segdata[4]=39;
   segdata[5]=32;
   segdata[6]=34;
   segdata[7]=23; 
   segdata[8]=24;  
   while(1)
    {
     PollISD();
     if( (UCSRA & (1 << RXC)) ) 
      { 
       uart = UDR;
       if(uart_temp==0)
        {
         switch(uart)
          {
           case 123: // {: bootloader reset
            RecordMode(0);
            DisplayStb();
            cli();
            UCSRB &= ~(1 << RXEN);
            UCSRB &= ~(1 << TXEN);
            loaderreset();
            break;
           case 65: // A: get brightness level
            PutInt(GetADC(0));
            break;
           case 66: // B: get audio level
            PutInt(GetADC(1));
            break;          
           case 67: // C: set brightness level
            temp = GetADC(0);
            eeprom_write_word(&eeDimval, temp);
            break;
           case 68: // D: enable rtc test
            DS1338write_status(DS1338STATUS_TEST);         
            break;
           case 69: // E: disable rtc test
            DS1338write_status(DS1338STATUS);
            break;
           case 70: // F: play sample
            uart_temp = uart; // copy command, check next byte in rx buffer
            break; 
           case 71: // G: enable recording
            RecordMode(1);
            break; 
           case 72: // H: disable recording
            RecordMode(0);
            break; 
           case 73: // I: start recording sample
            if(Record(1)==0) UDR = 43; else UDR = 45; // +: OK, -: FAIL
            break;    
           case 74: // J: stop recording sample
            if(Record(0)==0) UDR = 43; else UDR = 45; // +: OK, -: FAIL
            break;     
           case 75: // K: analyze speech samples in memory
            RecordMode(0);
            segdata[0]=59;
            segdata[1]=20;
            segdata[2]=33;
            segdata[3]=20;
            segdata[4]=31;
            segdata[5]=44;
            segdata[6]=45;
            segdata[7]=24; 
            segdata[8]=59;
            AnalyzeSpeechISD();
            UDR = 43; // +: OK
            segdata[0]=59;
            segdata[1]=39;
            segdata[2]=24;
            segdata[3]=38;
            segdata[4]=39;
            segdata[5]=32;
            segdata[6]=34;
            segdata[7]=23; 
            segdata[8]=24;
            break;  
           case 76: // L: get wsound count
            PutInt(CountSounds());
            break;            
           case 77: // M: set wsound count
            uart_temp = uart; // copy command, check next byte in rx buffer
            break;                            
          }
        } else {
         if(uart>0)
          {
           switch(uart_temp)
            {
             case 70:
              PlayNumber(uart-1);
              UDR = 43; // +: OK         
              break;
             case 77:
              SetCountSounds(uart-1);
              UDR = 43; // +: OK         
              break;
            }
           uart_temp = 0;
          }
        } 
      }
    } 
  }
