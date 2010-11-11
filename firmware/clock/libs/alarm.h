// #############################################################################
// #                   --- IW18 VFD Clock Firmware ---                         #
// #############################################################################
// # alarm.h - Header: Alarm handler                                           #
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

#ifndef ALARM_H
 #define ALARM_H

 #define ALARM_STATUS_OFF     0 // Alarm currently inactive
 #define ALARM_STATUS_INIT    1 // Alarm waiting for other events
 #define ALARM_STATUS_ON      2 // Alarm active
 #define ALARM_STATUS_SNOOZE  3 // Snooze active
 #define ALARM_STATUS_DISABLE 4 // Alarm should be disabled
 #define ALARM_STATUS_WAIT    5 // Alarm is disabled but hour and min matching choosen alarm time, so wait 1 minute to go by
 
 typedef struct {
  uint8_t hour;
  uint8_t min;
 } tAlarm;

 volatile uint8_t alarm;      // Which alarm is active?
 volatile tAlarm alrm[4];     // Structure for times.
 
 volatile uint8_t alarmtime;  // Each in minutes
 volatile uint8_t snoozetime;
 volatile uint8_t tries;

 void InitAlarm( void );
 void TickAlarm( void );
 void PollAlarm( uint8_t hour, uint8_t min, uint8_t wsound );
 uint8_t AlarmWait( void );
 void AlarmSetInitFinished( void ); 
 uint8_t RunningAlarm( void );
 uint8_t RunningSnooze( void );
 uint8_t CanDisableAlarm( void );
 void DisableAlarm( void );

#endif
