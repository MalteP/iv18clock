// #############################################################################
// #                   --- IW18 VFD Clock Firmware ---                         #
// #############################################################################
// # rfm.c - RFM12 Wireless routines                                           #
// # Thanks to Benedikt K. and das-labor.org for their RFM12 sample codes!     #
// # This code is using the XXTEA crypto function adapted from jeelabs.org,    # 
// # original written by Roger Needham and David Wheeler.                      # 
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

// #############################################################################
// # Protocol description:                                                     #
// # | LEN | SRC | DST | Data[n] | CRC16 |                                     #
// # where                                                                     #
// # LEN     = Length of data in bytes (4...36)                                #
// # SRC     = 1 Byte source id               \ 0 = Host (IW18 Clock)          #
// # DST     = 1 Byte destination id          /   or Sensor (1...254)          #
// # Data[n] = Block of LEN-4 bytes data                                       #
// # CRC16   = 2 Bytes CRC checksum of SRC, DST and Data[n]                    #
// # Everything except LEN and the CRC checksum is XXTEA encrypted.            #
// # Attention: SRC, DST, Data must be fit into blocks of 4 byte for           #
// # correct encryption!                                                       # 
// #############################################################################

 #include <avr/interrupt.h>
 #include <util/delay.h>
 #include <util/crc16.h>
 #include "libs/rfm.h"
 #include "libs/portdef.h"
 #include "libs/uart.h"
 #include "libs/rfm-cryptokey.h"

 uint16_t rfm_crc( void );
 void xxtea_crypt( uint32_t* v, uint8_t n );
 void xxtea_decrypt( uint32_t* v, uint8_t n );

 volatile uint8_t rfm_enabled;
 volatile uint8_t rfm_status;                                                // RFM status
 volatile uint8_t rfm_len;                                                   // Length of data
 volatile uint8_t rfm_pos;                                                   // Position in buffer
 volatile uint8_t rfm_buffer[(BUFFER_SIZE+1)];                               // Buffer for data

 volatile uint8_t counter;
 volatile uint8_t mask;
 volatile uint8_t buffer[3];
 volatile uint8_t repeat; 


 // --- Send via spi ---
 uint16_t rf12_trans(uint16_t wert)
  {	
   uint16_t werti = 0;
   RFM_NSEL_PORT &=~(1<<RFM_NSEL);
   
   SPDR = (0xFF00 & wert)>>8;
   while(!(SPSR & (1<<SPIF))){};
   werti = (SPDR<<8);

   SPDR = (0x00FF & wert);
   while(!(SPSR & (1<<SPIF))){};
   werti = werti + SPDR;

   RFM_NSEL_PORT |=(1<<RFM_NSEL);
   
   return werti;
 }


 // --- Sets bandwith ---
 void rf12_setbandwidth(uint8_t bandwidth, uint8_t gain, uint8_t drssi)
  {
   rf12_trans(0x9400|((bandwidth&7)<<5)|((gain&3)<<3)|(drssi&7));
  }


 // --- Sets frequency ---
 void rf12_setfreq(uint16_t freq)
  {	
   if (freq<96) freq=96;           // 430,2400MHz
    else if (freq>3903) freq=3903; // 439,7575MHz
   rf12_trans(0xA000|freq);
  }
  

 // --- Sets baudrate ---
 void rf12_setbaud(uint16_t baud)
  {
   if (baud<663) return;
   // Baudrate= 344827,58621/(R+1)/(1+CS*7)
   if (baud<5400) rf12_trans(0xC680|((43104/baud)-1));
    else rf12_trans(0xC600|((344828UL/baud)-1));
  }


 // --- Sets power output ---
 void rf12_setpower(uint8_t power, uint8_t mod)
  {	
   rf12_trans(0x9800|(power&7)|((mod&15)<<4));
  }


 // --- Initialize ports set some parameters ---
 void InitRFM( void ) 
  {
   rfm_enabled = 0;
  
   RFM_DDR |= (1<<RFM_SDI);
   RFM_DDR &= ~(1<<RFM_SDO);
   RFM_DDR |= (1<<RFM_SCK);
   RFM_NSEL_DDR |= (1<<RFM_NSEL);
   RFM_NSEL_PORT |=(1<<RFM_NSEL);

   // Sample on rising edge, send MSB first, no INT on rcv, divide host clock /8 = 2MHz
   SPCR = (1<<SPE)|(1<<MSTR);
   SPSR &= ~(0<<SPI2X);

   RFM_DDR_NIRQ &= ~(1<<RFM_NIRQ);                                                       // NIRQ is input
   MCUCR &= ~((1<<ISC00)|(1<<ISC01));                                                    // Trigger on low level
   GICR &= ~(1<<INT0);

   TCCR0 = (1<<CS00) | (1<<CS01);                                                        // Setup prescaler (8MHz / 64)
   TIMSK &= ~(1<<TOIE0);

   for (unsigned char i=0; i<10; i++) _delay_ms(10);                                     // wait until POR done

   rf12_trans(RF12_CLK);                                                                 // No clock or BOD
   rf12_trans(RF12_CFG|(1<<RF12_EL)|(1<<RF12_EF)|(1<<RF12_B)|(7<<RF12_X));               // Enable FIFO in config
   rf12_trans(RF12_FIL|(1<<RF12_AL2)|(1<<RF12_UK2)|(1<<RF12_UK1)|(3<<RF12_F2));          // Data Filter: internal
   rf12_trans(RF12_FIF|(4<<RF12_F)|(1<<RF12_DR));                                        // set FIFO mode
   rf12_trans(RF12_WKU);                                                                 // disable wakeuptimer
   rf12_trans(RF12_DTY);			                                         // disable low duty cycle
   rf12_trans(RF12_AFC|(3<<RF12_A)|(3<<RF12_RL)|(1<<RF12_FI)|(1<<RF12_OE)|(1<<RF12_EN)); // AFC settings: autotuning: -10kHz...+7,5kHz

   rf12_setfreq(RF12FREQ(433.92));                                                       // Set send/recieve frequency to 433,92MHz
   rf12_setbandwidth(4, 1, 4);                                                           // 200kHz bandwith, -6dB gain, DRSSI threshold: -79dBm 
   rf12_setbaud(19200);                                                                  // 19200 baud
   rf12_setpower(0, 6);                                                                  // 1mW power, 120kHz shift

   rfm_status = RF12_STATUS_IDLE;
  }


 // --- Something to be done with RFM12? ---
 void PollRFM( void )
  {
   uint8_t rfm_temp = rfm_status;
   uint16_t crc_temp;
   switch(rfm_temp)
    {
     case RF12_STATUS_IDLE:
      if(rfm_enabled!=0) rfm_status = RF12_STATUS_INIT;
      break;
     case RF12_STATUS_INIT:
      rfm_status = RF12_STATUS_RECV_INIT;
      break;
     case RF12_STATUS_RECV_INIT:
      rfm_pos = 0;
      rfm_len = BUFFER_SIZE;
      rf12_trans(RF12_PM|(1<<RF12_ER)|(1<<RF12_EBB)|(1<<RF12_EX)|(1<<RF12_DC));             // RX on
      rf12_trans(RF12_FIF|(8<<RF12_F)|(1<<RF12_DR));                                        // set FIFO mode
      rf12_trans(RF12_FIF|(8<<RF12_F)|(1<<RF12_FF)|(1<<RF12_DR));                           // enable FIFO
      GIFR = (1<<INTF0);                                                                    // Delete flag
      GICR |= (1<<INT0);                                                                    // Setup and enable interrupt
      rfm_status = RF12_STATUS_RECV;     
      break;
     case RF12_STATUS_RECV:
      if(rfm_enabled==0) rfm_status = RF12_STATUS_DISABLE;
      if(rfm_pos>rfm_len)
       {
        GICR &= ~(1<<INT0);
        rf12_trans(RF12_PM|(1<<RF12_EX)|(1<<RF12_DC));                                      // RX off       
        rfm_status = RF12_STATUS_RECV_DONE;
       }
      break;
     case RF12_STATUS_RECV_DONE:
     crc_temp = rfm_crc();
     if(((crc_temp>>8)&0x00FF)==rfm_buffer[(rfm_len-1)]&&(crc_temp&0x00FF)==rfm_buffer[rfm_len]&&(((rfm_len-2)%4)==0)) // CRC and len ok?
      {
       xxtea_decrypt( (uint32_t*) (rfm_buffer+1), ((rfm_len-2)/4) );
       if(rfm_buffer[1]>0&&rfm_buffer[2]==0) // SRC and DST ok?
        {
       
         /* TODO: handle recieved packet here! */

         /*
         // DEBUG
         uint8_t i;
         for(i=0; i<=rfm_len; i++) 
          {
           PutChar(rfm_buffer[i]);
          }
         PutChar('\r');
         PutChar('\n');
         */
         
        }
      } 
      rfm_status = RF12_STATUS_RECV_INIT;
      
      break;
     case RF12_STATUS_DISABLE:
      rf12_trans(RF12_PM|(1<<RF12_EX)|(1<<RF12_DC));                                      // RX off       
      GICR &= ~(1<<INT0);
      rfm_status = RF12_STATUS_IDLE;
      break;
     case RF12_STATUS_INIT_AM:
      if(rfm_enabled!=0)
       {
        GICR &= ~(1<<INT0);
        rf12_trans(RF12_PM|(1<<RF12_EX)|(1<<RF12_DC));
        mask=0;
        counter=0;
        repeat=5;
        TIMSK |= (1<<TOIE0);
        rfm_status = RF12_STATUS_SEND_AM;
       } else {
        rfm_status = RF12_STATUS_IDLE;
       }
      break;      
     case RF12_STATUS_SEND_AM:
      if(repeat==0)
       {
        TIMSK &= ~(1<<TOIE0);
        rfm_status = RF12_STATUS_IDLE;
       }
      break;      
    }
  }


 // --- Enable or disable complete RFM ---
 void SetWireless( uint8_t status )
  {
   rfm_enabled = status;  
  }


 // --- Switch on wireless plug ---
 void wireless_switch( uint8_t housecode, uint8_t code, uint8_t state )
  {
   uint8_t i;
   // Calculate pulses for buffer: 0 -> short, 1 -> long
   // where 00 = logical 0, 01 = floating, 11 = logical 1
   for(i=0; i<24; i++)
    {
     if(i<=9) // housecode
      {
       if(housecode&(1<<(i/2))) 
        {
         // 1 which means switch to ground = 00
         buffer[i/8] &= ~(0x80>>(i%8));
        } else {
         // 0 which means floating = 01 
         if(i&0x01) buffer[i/8] |= (0x80>>(i%8)); else buffer[i/8] &= ~(0x80>>(i%8));
        }      
      } else if(i<=19) // code 0-4 (A/B/C/D/E)
       {
        if(code==((i-10)/2)) 
         {
          // 1 which means switch to ground = 00
          buffer[i/8] &= ~(0x80>>(i%8));
         } else {
          // 0 which means floating = 01 
          if(i&0x01) buffer[i/8] |= (0x80>>(i%8)); else buffer[i/8] &= ~(0x80>>(i%8));
         }
       } else { // state
        if(state&(1<<((i-20)/2))) 
         {
          // 1 which means switch to ground = 00
          buffer[i/8] &= ~(0x80>>(i%8));
         } else {
          // 0 which means floating = 01 
          if(i&0x01) buffer[i/8] |= (0x80>>(i%8)); else buffer[i/8] &= ~(0x80>>(i%8));
         }        
       }
    } 
   rfm_status = RF12_STATUS_INIT_AM;
  }


 // --- Send in progress? ---
 uint8_t wireless_switch_running( void )
  {
   if(rfm_status==RF12_STATUS_INIT_AM||rfm_status==RF12_STATUS_SEND_AM) return 1; else return 0;
  }
  

 // --- Interrupt from RFM (INT0 low) ---
 ISR( INT0_vect ) 
  {
   rf12_trans(RF12_RD_STATUS);
   if(rfm_pos<=rfm_len)
    {
     rfm_buffer[rfm_pos]=rf12_trans(RF12_RD_FIFO);
     if(rfm_pos==0)
      {
       rfm_len = rfm_buffer[0];
       if(rfm_len>BUFFER_SIZE) rfm_len = BUFFER_SIZE;   
      }
    } else {
     GICR &= ~(1<<INT0); // disable interrupt if rx done
    }
   rfm_pos++;
  }


 // --- Calculate CRC16 of one packet (using the global variables) ---
 // Polynomial is x^16 + x^15 + x^2 + 1, Initial value 0xFFFF.
 uint16_t rfm_crc( void )
  {
   uint8_t i;
   uint16_t checksum = 0xFFFF;
   for( i=1; i<=(rfm_len-2); i++) { checksum = _crc16_update(checksum, rfm_buffer[i]); }
   return checksum;  
  }


 // XXTEA by David Wheeler, adapted from http://en.wikipedia.org/wiki/XXTEA
 // Also have a look at jeelabs.org blog on http://jeelabs.org/2010/02/23/secure-transmissions/

 #define DELTA 0x9E3779B9
 #define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (k[(uint8_t)((p&3)^e)] ^ z)))

 // v: pointer to data (n* 32 bit)
 // k: key (128 bit)
 // n: number of blocks

 // --- XXTEA encryption ---
 void xxtea_crypt( uint32_t* v, uint8_t n )
  {
   uint32_t y, z, sum;
   uint8_t p, e, rounds = 6;
   if (n > 1)
    {
     sum = 0;
     z = v[n-1];
     do
      {
       sum += DELTA;
       e = (sum >> 2) & 3;
       for (p=0; p<n-1; p++) y = v[p+1], z = v[p] += MX;
       y = v[0];
       z = v[n-1] += MX;
      } while (--rounds);
    } 
  }


 // --- XXTEA decryption ---
 void xxtea_decrypt( uint32_t* v, uint8_t n )
  {
   uint32_t y, z, sum;
   uint8_t p, e, rounds = 6;
   if (n > 1)
    {
     sum = rounds*DELTA;
     y = v[0];
     do
      {
       e = (sum >> 2) & 3;
       for (p=n-1; p>0; p--) z = v[p-1], y = v[p] -= MX;
       z = v[n-1];
       y = v[0] -= MX;
      } while ((sum -= DELTA) != 0);
    } 
  }


 // --- Interrupt for protocol waveform (wireless plug) ---
 ISR( TIMER0_OVF_vect ) 
  {
   if(repeat>0)
    {
     if(counter==0||mask==0) mask=0x80;
     if(counter<48) // Data or sync?
      {  
       if(!(counter&0x01)) // Last bit: 0 = TX on, 1 = TX off
        {
         rf12_trans(RF12_PM|(1<<RF12_EX)|(1<<RF12_ES)|(1<<RF12_ET));
         if(buffer[counter/16]&mask) // First bit in buffer 0 = short pulse, 1 = long pulse
          {
           TCNT0 = PRELOAD_LONG_PULSE;
          } else {
           TCNT0 = PRELOAD_SHORT_PULSE;        
          }
        } else {
         rf12_trans(RF12_PM|(1<<RF12_EX));
         if(buffer[counter/16]&mask) // First bit in buffer 0 = long break, 1 = short break
          {
           TCNT0 = PRELOAD_SHORT_PULSE;
          } else {
           TCNT0 = PRELOAD_LONG_PULSE;        
          }       
         mask=mask>>1; // Next bit in next interrupt!
        } 
      } else {
       if(counter==48)
        {
         rf12_trans(RF12_PM|(1<<RF12_EX)|(1<<RF12_ES)|(1<<RF12_ET)); // Sync pattern short pulse
         TCNT0 = PRELOAD_SHORT_PULSE;
        } else
         if(counter==49)
          {
           rf12_trans(RF12_PM|(1<<RF12_EX)); // Sync pattern break
          }
       TCNT0 = PRELOAD_SHORT_PULSE;
      }
     if(counter>=80)
      {
       counter = 0;
       repeat--;
      } else {
       counter++;
      }
    }
  }
