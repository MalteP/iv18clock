// #############################################################################
// #                   --- IW18 VFD Clock Firmware ---                         #
// #############################################################################
// # rfm.h - Header: RFM12 Wireless routines                                   #
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

#ifndef RFM_H
 #define RFM_H

 #define RF12FREQ(freq)	((freq-430.0)/0.0025)

 #define BUFFER_SIZE 36

 #define RF12_CFG 0x8000 // Configuration
 #define RF12_EL  7
 #define RF12_EF  6
 #define RF12_B   4
 #define RF12_X   0
 
 #define RF12_PM  0x8200 // Power Management
 #define RF12_ER  7
 #define RF12_EBB 6
 #define RF12_ET  5
 #define RF12_ES  4
 #define RF12_EX  3
 #define RF12_EB  2
 #define RF12_EW  1
 #define RF12_DC  0
 
 #define RF12_FIF 0xCA00 // FIFO and Reset Mode
 #define RF12_F   4
 #define RF12_SP  3
 #define RF12_AL  2
 #define RF12_FF  1
 #define RF12_DR  0

 #define RF12_AFC 0xC400 // AFC Config
 #define RF12_A   6
 #define RF12_RL  4
 #define RF12_ST  3
 #define RF12_FI  2
 #define RF12_OE  1
 #define RF12_EN  0

 #define RF12_SYN 0xCE00 // Sync Pattern
 
 #define RF12_DTY 0x6400 // Duty Cycle
 
 #define RF12_WKU 0xE000 // Wakeup Timer
 
 #define RF12_CLK 0xC000 // LowBatt / µC Clock Control
 #define RF12_D   5
 #define RF12_V   0

 #define RF12_FIL 0xC200 // Data Filter
 #define RF12_AL2 7
 #define RF12_ML  6
 #define RF12_UK2 5
 #define RF12_S   4
 #define RF12_UK1 3
 #define RF12_F2  0 
 
 // Read commands
 #define RF12_RD_STATUS  0x0000
 #define RF12_RD_FIFO    0xB000
 
 // Internal status
 #define RF12_STATUS_IDLE      0
 #define RF12_STATUS_INIT      1
 #define RF12_STATUS_RECV_INIT 2
 #define RF12_STATUS_RECV      3
 #define RF12_STATUS_RECV_DONE 4
 #define RF12_STATUS_SEND_INIT 5
 #define RF12_STATUS_SEND      6
 #define RF12_STATUS_DISABLE   7
 #define RF12_STATUS_INIT_AM   8
 #define RF12_STATUS_SEND_AM   9
 
 // Wireless remote plug
 #define PRELOAD_SHORT_PULSE 215 // ~320us
 #define PRELOAD_LONG_PULSE  135 // ~950us 
 
 #define PLUG_SWITCH_2_ON  0x01
 #define PLUG_SWITCH_2_OFF 0x02
 #define PLUG_SWITCH_4_ON  0x0E
 #define PLUG_SWITCH_4_OFF 0x06
 
 void InitRFM( void );
 void PollRFM( void );
 void SetWireless( uint8_t status );
 void wireless_switch_2( uint8_t housecode, uint8_t code, uint8_t state );
 void wireless_switch_4( uint8_t housecode, uint8_t code, uint8_t state );
 uint8_t wireless_switch_running( void );

#endif
