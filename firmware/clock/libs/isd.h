// #############################################################################
// #                   --- IW18 VFD Clock Firmware ---                         #
// #############################################################################
// # isd.h - Header: ISD2590 speech handler                                    #
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

#ifndef ISD_H
 #define ISD_H

 void InitISD( void );
 void SetVolume( uint8_t v );
 void PollISD( void );
 uint8_t IsPlaying( void );
 void AbortPlaying( void );
 void SayTime( uint8_t h, uint8_t m );
 uint8_t CountSounds( void );
 void SetCountSounds( uint8_t count );
 void PlaySound( uint8_t s );
 void PlayNumber( uint8_t n );
 void AnalyzeSpeechISD( void );
 void RecordMode( uint8_t m );
 uint8_t Record( uint8_t m );

 #define ISD_ADDR   0
 #define ISD_MUTE   11
 #define ISD_AMP_EN 12
 #define ISD_VOL    13
 
 #define ISD_MODE_MSG_CUE  0
 #define ISD_MODE_DEL_EOM  1
 #define ISD_MODE_MSG_LOOP 3
 #define ISD_MODE_CON_ADDR 4
 #define ISD_MODE_CE_L_ACT 5
 #define ISD_MODE_PUSH_BTN 6
 // Pull following pins high if using commands above
 #define ISD_MODE_OP1      8
 #define ISD_MODE_OP2      9

 #define WAKESOUND_PRESET 28
  
#endif
