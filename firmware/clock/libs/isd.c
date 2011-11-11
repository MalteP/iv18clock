// #############################################################################
// #                   --- IW18 VFD Clock Firmware ---                         #
// #############################################################################
// # isd.c - ISD2590 speech handler                                            #
// #############################################################################
// #              Version: 2.1 - Compiler: AVR-GCC 4.5.0 (Linux)               #
// #  (c) 08-11 by Malte Pöggel - www.MALTEPOEGGEL.de - malte@maltepoeggel.de  #
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
 #include <avr/eeprom.h>
 #include <avr/interrupt.h>
 #include "libs/isd.h"
 #include "libs/portdef.h"
 #include "libs/voicetab.h"

 uint16_t eeSpeechArray[64] EEMEM;
 uint8_t eeSpeechCount EEMEM;
 uint8_t SpeechCount;
 uint8_t SpeechBuffer[8];
 uint8_t SpeechCounter;
 uint8_t SpeechVolume;
 volatile uint8_t SpeechActive;


 // --- Interrupt on EOM/OVF ---
 ISR(INT2_vect) 
  {
   SpeechActive=0; // Reset this on falling edge.
  } 


 // --- Activate recording ---
 void ISD_setRec( void ) 
  {
   ISD_PRT_REC  |= (1<<ISD_REC);  
  }
  
  
 // --- Deactivate recording ---
 void ISD_resetRec( void )
  {
   ISD_PRT_REC  &= ~(1<<ISD_REC);  
  }
  
  
 // --- Pulse /CE ---
 void ISD_ChipEnable( void )
  {
   ISD_PRT_CE &= ~(1<<ISD_CE);
   _delay_us(0.1); // tCE = 100nsec
   ISD_PRT_CE |= (1<<ISD_CE);
  }  
  

 // --- Power Up ---
 void ISD_PowerUp( void )
  {
   ISD_PRT_PD   &= ~(1<<ISD_PD);
   _delay_ms(38);
  }


 // --- Power Down ---
 void ISD_PowerDown( void )
  {
   ISD_PRT_PD   |= (1<<ISD_PD);
   _delay_ms(38);
  }


 // --- Send address data to shift register ---
 void ISDOut( uint16_t data )
  {
    uint8_t i;
   
    for( i=16; i>0; i-- ) 
    {
     ISD_PRT_DATA &= ~(1<<ISD_DATA);
     if ( data & 0x8000 )
     {
      ISD_PRT_DATA |= (1<<ISD_DATA);
     }
     data <<= 1;
     ISD_PRT_CLK |= (1<<ISD_CLK);
     ISD_PRT_CLK &= ~(1<<ISD_CLK);
    }  
    ISD_PRT_LOAD |= (1<<ISD_LOAD);
    ISD_PRT_LOAD &= ~(1<<ISD_LOAD);
  }

    
 // --- Start playing ---
 void ISDPlay( uint16_t addr )
  {
   ISD_PowerUp();
   ISDOut( (addr&0x01FF) | (1<<ISD_AMP_EN) | ((SpeechVolume&0x07)<<ISD_VOL) );
   _delay_us(0.3); // tSET = 300nsec
   ISD_ChipEnable();
  }


 // --- Play with adress from eeprom ---
 void ISDSay( uint8_t message )
  {
   ISDPlay(eeprom_read_word(&eeSpeechArray[message]));
  }

  
 // --- Initialize ports and shift registers ---
 void InitISD( void ) 
  {

   ISD_DDR_CLK  |= (1<<ISD_CLK);        // Clock
   ISD_DDR_LOAD |= (1<<ISD_LOAD);       // Load
   ISD_DDR_DATA |= (1<<ISD_DATA);       // Data
   ISD_DDR_REC  |= (1<<ISD_REC);        // Rec
   ISD_DDR_PD   |= (1<<ISD_PD);         // PD
   ISD_DDR_CE   |= (1<<ISD_CE);         // CE	

   ISD_PRT_CLK  &= ~(1<<ISD_CLK);       // Set ports to zero
   ISD_PRT_LOAD &= ~(1<<ISD_LOAD);
   ISD_PRT_DATA &= ~(1<<ISD_DATA);
   ISD_PRT_REC  &= ~(1<<ISD_REC);

   ISD_PRT_PD   |= (1<<ISD_PD);         // CE & PD are active low,
   ISD_PRT_CE   |= (1<<ISD_CE);         // so set them to HI level

   ISD_DDR_INT    &= ~(1<<ISD_INT);     // Input for overflow / end of message trigger from 7408
   ISD_DDR_EOM    &= ~(1<<ISD_EOM);     // Input for EOM signal

   MCUCSR &= ~(1 << ISC2);              // Trigger on falling edge
   GIFR = (1 << INTF2);                 // Delete flag
   GICR |= (1<<INT2);

   ISDOut(0x0000);                      // Clear shift register

   SpeechActive=0;
   SpeechCounter=0;
   SpeechVolume=0;
  }


 void SetVolume( uint8_t v )
  {
   SpeechVolume = v;
  }


 // --- Look for something to be done ---
 void PollISD( void )
  {
   if(SpeechCounter!=0)
    {
     if(SpeechActive==0)
      {
       if(SpeechCounter<=8)
        {
         ISDOut( (1<<ISD_MUTE) | ((SpeechVolume&0x07)<<ISD_VOL) );
         if(SpeechBuffer[(SpeechCounter-1)]!=254) { ISDSay(SpeechBuffer[(SpeechCounter-1)]); SpeechActive=1; }
         SpeechCounter++;
        } else {
         ISDOut(0);
         ISD_PowerDown();
         SpeechCounter=0;
        }
      }
    }
  }


 // --- For status check ---
 uint8_t IsPlaying( void )
  {
   if(SpeechCounter==0) 
    {
     return 0;
    } else {
     if(SpeechActive==1||(SpeechCounter<=8&&SpeechBuffer[(SpeechCounter-1)]!=254)) return 1; else return 0;
    }
  }


 // --- Abort playing now! ---
 void AbortPlaying( void )
  {
   if(SpeechCounter!=0||SpeechActive!=0) 
    {
     ISDOut(0);
     ISD_PowerDown();
     SpeechCounter=0;
     SpeechActive=0;
    }
  }


 // --- Whats the time baby? ---
 void SayTime( uint8_t h, uint8_t m )
  {
   uint8_t bctr=0;
   if(SpeechCounter==0)
    {
     SpeechCounter=1;
     SpeechBuffer[bctr++]=16; // "Es ist"
     if(h>20)
      {
       SpeechBuffer[bctr++]=pgm_read_byte(&voicetab_a[h%10]);
       if((h%10)!=0) SpeechBuffer[bctr++]=11; else SpeechBuffer[bctr++]=254; // need "und" or is x0?
       SpeechBuffer[bctr++]=pgm_read_byte(&voicetab_b[(h/10)-2]);
      } else {
       SpeechBuffer[bctr++]=pgm_read_byte(&voicetab_a[h]);
      }
     SpeechBuffer[bctr++]=10; // "Uhr"
     if(m>=20)
      {
       if((m%10)!=0) // not 20, 30, 40, 50? then say x "und"
        {
         SpeechBuffer[bctr++]=pgm_read_byte(&voicetab_a[m%10]);
         SpeechBuffer[bctr++]=11;
        } 
       if(m!=0) SpeechBuffer[bctr++]=pgm_read_byte(&voicetab_b[(m/10)-2]); else SpeechBuffer[bctr++]=254;
      } else {
       if(m!=0) { if(m!=1) SpeechBuffer[bctr++]=pgm_read_byte(&voicetab_a[m]); else SpeechBuffer[bctr++]=17; } else SpeechBuffer[bctr++]=254;
      }
     while(bctr<=(sizeof(SpeechBuffer)-1)) SpeechBuffer[bctr++]=254;
    }
  }

 
 // --- How many wake sounds in memory ---
 uint8_t CountSounds( void )
  {
   uint8_t cnt = eeprom_read_byte(&eeSpeechCount);
   if(cnt > WAKESOUND_PRESET)
    {
     cnt -= WAKESOUND_PRESET;
     return cnt;
    } else {
     return 0;
    }
  }


 // --- Play wake sound ---
 void PlaySound( uint8_t s )
  {
   if(IsPlaying()) AbortPlaying();
   SpeechCounter=1;
   SpeechBuffer[0]=s + WAKESOUND_PRESET;
   SpeechBuffer[1]=254;
   SpeechBuffer[2]=254;
   SpeechBuffer[3]=254;
   SpeechBuffer[4]=254;
   SpeechBuffer[5]=254;
   SpeechBuffer[6]=254;
   SpeechBuffer[7]=254;
  }


 // --- Play number (for volume preview) ---
 void PlayNumber( uint8_t n )
  {
   if(IsPlaying()) AbortPlaying();
   SpeechCounter=1;
   SpeechBuffer[0]=n;
   SpeechBuffer[1]=254;
   SpeechBuffer[2]=254;
   SpeechBuffer[3]=254;
   SpeechBuffer[4]=254;
   SpeechBuffer[5]=254;
   SpeechBuffer[6]=254;
   SpeechBuffer[7]=254;   
  }


 // --- Check for EOM marker at given memory position --- 
 uint8_t CheckEOM( uint16_t addr )
  {
   uint8_t timeout=254;
   ISD_PowerUp();
   ISDOut( (addr&0x01FF) );
   _delay_us(0.3); // tSET = 300nsec
   SpeechActive=1;
   ISD_ChipEnable();
   while(SpeechActive==1 && timeout>0)
    {
     _delay_ms(.5); // tSEG = 150ms / 254 = 0.59
     timeout--;
    }
   ISD_PowerDown();
   if(timeout>0) return 0; else return 1; // 0 = EOM found, 1 = no EOM
  }


 // --- Scan Memory for EOM markers and write beginnings to eeprom ---
 void AnalyzeSpeechISD( void )
  {
   uint8_t ctr = 0;
   uint16_t msgPtr;
   uint8_t act;
   uint8_t last = 0;
   for(msgPtr=0; msgPtr<=599; msgPtr++)
    {
     act=CheckEOM(msgPtr);
     if(last==0&&act==1)
      {
       eeprom_write_word(&eeSpeechArray[ctr], msgPtr);
       ctr++;
       if(ctr>=(sizeof(eeSpeechArray)/2)) break; // Attention: sizeof() -> size in bytes, we have integer!
      }
     last=act;
    }
  }


 // --- Set or reset recording mode, clear and save message counter ---
 // 0: disable, 1: enable
 void RecordMode( uint8_t m )
  {
   if(m==0)
    {
     // Terminate recording
     ISD_PowerDown();
     ISDOut(0);    
     ISD_resetRec();
     if( SpeechCount > 0 )
      {
       eeprom_write_byte(&eeSpeechCount, SpeechCount);
      }
    } else {
     SpeechCount = 0;
     ISD_PowerUp();
     ISD_setRec(); 
     // Record using push button mode
     ISDOut((1<<ISD_MODE_PUSH_BTN)|(1<<ISD_MODE_OP1)|(1<<ISD_MODE_OP2));
     _delay_us(0.3); // tSET = 300nsec    
    }
  }


 // --- Start or stop recording, increment message counter ---
 // 0: stop, 1: start
 uint8_t Record( uint8_t m )
  {
   if(m==0)
    {

     if((ISD_PIN_EOM & (1<<ISD_EOM)))
      {
       ISD_ChipEnable();
       return 0;
      } else {
       return 1;
      }
    } else {
     if(!(ISD_PIN_EOM & (1<<ISD_EOM)))
      {
       ISD_ChipEnable();
       SpeechCount++;
       return 0;
      } else {
       return 1;
      }    
    }  
  }
 
