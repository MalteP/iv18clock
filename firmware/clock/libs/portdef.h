// #############################################################################
// #                   --- IW18 VFD Clock Firmware ---                         #
// #############################################################################
// # portdef.h - Header: Defines for port settings                             #
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

#ifndef PORTDEF_H
 #define PORTDEF_H

 // Define descriptions for easy use
 #define RTC_INT		PD3    // INT1
 #define RTC_DDR_INT		DDRD
 #define RTC_PRT_INT		PORTD
  
 #define DISP_CLK		PB0
 #define DISP_DDR_CLK		DDRB
 #define DISP_PRT_CLK		PORTB
 
 #define DISP_LOAD		PB1
 #define DISP_DDR_LOAD		DDRB
 #define DISP_PRT_LOAD		PORTB
 
 #define DISP_DATA		PB4
 #define DISP_DDR_DATA		DDRB
 #define DISP_PRT_DATA		PORTB
 
 #define DISP_BLNK		PB3
 #define DISP_DDR_BLNK		DDRB
 #define DISP_PRT_BLNK		PORTB
  
 #define DISP_STB		PC7
 #define DISP_DDR_STB		DDRC
 #define DISP_PRT_STB		PORTC
 
 #define LED_A                  PD4
 #define LED_DDR_A              DDRD
 #define LED_PRT_A              PORTD
  
 #define LED_B                  PD5
 #define LED_DDR_B              DDRD
 #define LED_PRT_B              PORTD
 
 #define ISD_CLK		PD7
 #define ISD_DDR_CLK		DDRD
 #define ISD_PRT_CLK		PORTD
 
 #define ISD_LOAD		PC2
 #define ISD_DDR_LOAD		DDRC
 #define ISD_PRT_LOAD		PORTC
 
 #define ISD_DATA		PD6
 #define ISD_DDR_DATA		DDRD
 #define ISD_PRT_DATA		PORTD
 
 #define ISD_REC		PA5
 #define ISD_DDR_REC		DDRA
 #define ISD_PRT_REC		PORTA

 #define ISD_INT		PB2    // INT3
 #define ISD_DDR_INT		DDRB
 #define ISD_PRT_INT		PORTB
 #define ISD_PIN_INT		PINB

 #define ISD_PD			PA4
 #define ISD_DDR_PD		DDRA
 #define ISD_PRT_PD		PORTA

 #define ISD_CE			PA3
 #define ISD_DDR_CE		DDRA
 #define ISD_PRT_CE		PORTA

 #define ISD_EOM		PA2
 #define ISD_DDR_EOM		DDRA
 #define ISD_PRT_EOM		PORTA 
 #define ISD_PIN_EOM		PINA
  
 #define TAST_OK		PC4
 #define TAST_DDR_OK		DDRC
 #define TAST_PRT_OK		PORTC
 #define TAST_PIN_OK		PINC
  
 #define TAST_PLUS		PC5
 #define TAST_DDR_PLUS		DDRC
 #define TAST_PRT_PLUS		PORTC
 #define TAST_PIN_PLUS		PINC  
  
 #define TAST_MINUS		PC6
 #define TAST_DDR_MINUS		DDRC
 #define TAST_PRT_MINUS		PORTC
 #define TAST_PIN_MINUS		PINC

 #define LDR_ADC_PRT		0
 #define VOL_ADC_PRT		1
  
 #define RFM_SCK		PB7    // SCK
 #define RFM_SDI		PB5    // MOSI
 #define RFM_SDO		PB6    // MISO
 #define RFM_PORT		PORTB
 #define RFM_PIN		PINB 
 #define RFM_DDR		DDRB
 #define RFM_NSEL		PC3 
 #define RFM_NSEL_PORT		PORTC
 #define RFM_NSEL_DDR		DDRC
 #define RFM_NIRQ		PD2    // INT0
 #define RFM_DDR_NIRQ		DDRD
 #define RFM_PIN_NIRQ           PIND
     
#endif
