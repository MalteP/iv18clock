// #############################################################################
// #                   --- IV18 VFD Clock Firmware ---                         #
// #############################################################################
// # alarm.c - Alarm handler                                                   #
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
 #include "portdef.h"
 #include "isd.h"
 #include "random.h"
 #include "vfd.h"
 #include "alarm.h"

 volatile uint8_t alarm;      // Which alarm is active?
 volatile tAlarm alrm[4];     // Structure for times.

 volatile uint8_t alarmtime;  // Each in minutes
 volatile uint8_t snoozetime;
 volatile uint8_t tries;

 volatile uint16_t pwm_value;
 volatile uint8_t alarmstatus;
 volatile uint8_t sound;
 volatile uint16_t alarmtimer; // In seconds, for internal routines
 volatile uint16_t snoozetimer;
 volatile uint8_t triescounter; 
 volatile uint8_t predelay;


 // --- Initialize PWM and variables for alarm ---
 void InitAlarm( void )
  {
   alarmstatus = ALARM_STATUS_OFF;
   sound = 0;
   alarmtimer = 0;
   snoozetimer = 0;

   LED_DDR_A |= (1<<LED_A);
   LED_DDR_B |= (1<<LED_B);

   // Timer Phase correct 10 bit PWM, CLK/1
   TCCR1A = (1<<COM1A1) | (1<<COM1B1) | (1<<WGM10) | (1<<WGM11);
   TCCR1B = (1<<CS10);

   pwm_value = 0x0000;
   OCR1A = pwm_value;
   OCR1B = pwm_value;
  }


 // --- Should be called every second ---
 void TickAlarm( void )
  {
   uint8_t status_temp = alarmstatus;  
   switch(status_temp)
    {
     case ALARM_STATUS_INIT:
      if(predelay>0) predelay--;
      break;
     case ALARM_STATUS_ON:
      if(alarmtimer>0) alarmtimer--;
      break;
     case ALARM_STATUS_SNOOZE:
      if(snoozetimer>0) snoozetimer--;
      break;
    }
  }


 // --- Check if there is something to do ---
 void PollAlarm( uint8_t hour, uint8_t min, uint8_t sec, uint8_t wsound )
  {
   uint8_t status_temp;
   uint8_t i;
   // New alarm or some status change?  
   if(alarm>0)
    {
     for(i=0; i<4; i++) // Compare alarm mask for all alarms
      {
       if(alarm&(1<<i)&&alrm[i].hour==hour&&alrm[i].min==min&&sec==0)
        {
         if(alarmstatus==ALARM_STATUS_OFF) 
          {
           alarmstatus = ALARM_STATUS_INIT;
           predelay = 3; // Give other events 3 ticks time to initialize.
          }
        }
      }
    } else {
     if(alarmstatus!=ALARM_STATUS_OFF) // Should not happen
      {
       alarmstatus=ALARM_STATUS_DISABLE;
      }
    }
   // Handle running alarm
   status_temp = alarmstatus;
   switch(status_temp)
    {
     case ALARM_STATUS_INIT:
      if(predelay==0)
       {
        triescounter = tries;
        alarmtimer = alarmtime*60;
        alarmstatus=ALARM_STATUS_ON;
        if(wsound==0) sound = GetRandom(); else sound = wsound-1;
        SayTime( hour, min );
       }
      break;
     case ALARM_STATUS_ON:
       if(alarmtimer>0)
        {
         if(!IsPlaying()) PlaySound(sound);
         SetDim( 0 );
        } else {
         if(snoozetime>0)
          {
           triescounter--;
           if(triescounter>0)
            {
             AbortPlaying();
             snoozetimer = snoozetime*60;
             alarmstatus=ALARM_STATUS_SNOOZE;
            } else {
             alarmstatus=ALARM_STATUS_DISABLE;
            }
          } else {
           alarmstatus=ALARM_STATUS_DISABLE; 
          }
        }      
      break;
     case ALARM_STATUS_SNOOZE:
      if(snoozetimer==0)
       {
        alarmtimer = alarmtime*60;
        alarmstatus=ALARM_STATUS_ON;
        SayTime( hour, min ); 
       }    
      break;
     case ALARM_STATUS_DISABLE:
      AbortPlaying();
      alarmstatus=ALARM_STATUS_WAIT;
      break; 
     case ALARM_STATUS_WAIT:
      for(i=0; i<4; i++) // Compare alarm mask for all alarms
       {
        if(alarm&(1<<i)&&alrm[i].hour==hour&&alrm[i].min==min&&sec==0) return;
       }
      // Nothing matched, disable wait status
      alarmstatus=ALARM_STATUS_OFF;
      break;
    }
   // Power to the leds :-)
   if(alarmstatus==ALARM_STATUS_ON||alarmstatus==ALARM_STATUS_INIT)
    {
     if(pwm_value<0x03FF)
      {
       pwm_value=0x03FF;
       OCR1A = pwm_value;
       OCR1B = pwm_value;
      }   
    } else {
     if(pwm_value>0x0000)
      {
       pwm_value--;
       OCR1A = pwm_value;
       OCR1B = pwm_value;
      }    
    }
  }


 // --- Is alarm waiting for other processes ---
 uint8_t AlarmWait( void )
  {
   if(alarmstatus==ALARM_STATUS_INIT) return 1; else return 0;   
  }


 // --- Called by other processes if ready for alarm ---
 void AlarmSetInitFinished( void )
  {
   predelay = 0;
  }


 // --- Is an alarm running? ---
 uint8_t RunningAlarm( void ) 
  {
   if(alarmstatus==ALARM_STATUS_ON) return 1; else return 0;
  } 


 // --- Is the snooze function active? ---
 uint8_t RunningSnooze( void )
  {
   if(alarmstatus==ALARM_STATUS_SNOOZE) return 1; else return 0;
  }
  

 // --- Something to be disabled? ---
 uint8_t CanDisableAlarm( void ) 
  {
   if(alarmstatus==ALARM_STATUS_ON||alarmstatus==ALARM_STATUS_SNOOZE) return 1; else return 0;
  }


 // --- Disable running alarm or snooze ---
 void DisableAlarm( void ) 
  {
   if(alarmstatus==ALARM_STATUS_ON&&snoozetime>0)
    {
     alarmtimer = 0;
    } else {
     alarmstatus = ALARM_STATUS_DISABLE;
    }
  } 
