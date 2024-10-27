#ifndef WCTE_MPMT_LED_H
#define WCTE_MPMT_LED_H

#include <vector>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <bitset>

/* WCTEMPMTLED:
 *
 * Fields:
 * bits     width field
 *  0 -  1  2     Header
 *  2 -  5  4     EventType
 *  6 -  8  3     LED
 *  9 -  9  1     Gain
 * 10 - 19 10     DACSetting
 * 20 - 21  2     Type
 * 22 - 35 14     SequenceNumber
 * 36 - 67 32     CoarseCounter
 * 68 - 71  4     Reserved
 *
 * - Header: packet ID (0b01)
 *
 * - EventType: LED (2)
 *
 * - LED: which of three LEDs fired (3 bits)
 *
 * - Gain: gain DAC setting (1 bit) 1 = high gain (more photons) or 0 = low
 *   gain (less photons)
 *
 * - DACSetting: set the value of 10 bit DAC
 *
 * - Type: type of LED flash
 *   - 00: software initiated sequence
 *   - 01: hardware triggered
 *   - 10, 11: for future extensions
 *
 * - SequenceNumber: what flash are we in LED flashing sequence (supports up to
 *   16384 flashes in sequence)
 *
 * - CoarseCounter: lower 32 bits of coarse counter - when combined with the 16
 *   upper bits from the packet header, the 48-bit coarse counter is the coarse
 *   time for LED flash. LSB is 8 ns
 *
 * - Reserved: not currently in use
 *
 * Bits map:
 *    01234567
 *  0 hhEEEELL          h = Header
 *  1 LGDDDDDD          E = EventType
 *  2 DDDDttNN          L = LED
 *  3 NNNNNNNN          G = Gain
 *  4 NNNNTTTT          D = DACSetting
 *  5 TTTTTTTT          t = Type
 *  6 TTTTTTTT          N = SequenceNumber
 *  7 TTTTTTTT          T = CoarseCounter
 *  8 TTTTRRRR          R = Reserved
 */

class WCTEMPMTLED{

public:
  WCTEMPMTLED(){std::fill(std::begin(data), std::end(data), 0);}
  
  unsigned short GetHeader(){return (data[0] & 0b11000000) >> 6; }
  unsigned short GetEventType(){return (data[0] & 0b00111100) >> 2;}
  unsigned short GetLED(){return ((data[0] & 0b00000011) << 1) | ((data[1] & 0b10000000) >> 7); }
  bool GetGain(){return ((data[1] & 0b01000000) >> 6);}
  unsigned short GetDACSetting(){return ((data[1] & 0b00111111)  << 4 ) | ((data[2] & 0b11110000) >>4 ) ;}
  unsigned short GetType(){return ((data[2] & 0b00001100)  >> 2 );}
  unsigned short GetSequenceNumber(){return ((data[2] & 0b00000011)  << 12 ) | (data[3] << 4 ) | ((data[4] & 0b11110000 ) >> 4 ) ;}
  unsigned int GetCoarseCounter(){ return ((data[4] & 0b00001111) << 28) | (data[5] << 20) | (data[6] << 12 ) | (data[7] << 4) | ((data[8] & 0b11110000 ) >> 4 ) ;}
  unsigned short GetReserved(){return (data[8] & 0b00001111);}
  
  void SetHeader(unsigned short in){ data[0] = (data[0] & 0b00111111) | ((in & 0b00000011) << 6);}
  void SetEventType(unsigned short in){ data[0] = (data[0] & 0b11000011) | (( in & 0b00001111) << 2) ;}
  void SetLED(unsigned short in){
    data[0] = (data[0] & 0b11111100) | ((in & 0b00000110) >> 1);
    data[1] = (data[1] & 0b01111111)  | ((in & 0b00000001) << 7);
   }
  void SetGain(bool in){ data[1] = (data[1] & 0b10111111) | ((in & 0b00000001) << 6) ;}
  void SetDACSetting(unsigned short in){
    data[1] = (data[1] & 0b11000000) | ((in >> 4 ) & 0b00111111) ;
    data[2] = (data[2] & 0b00001111)  | ((in & 0b00001111) << 4 );
  }
  void SetType(unsigned short in){ data[2] = (data[2] & 0b11110011) | ((in & 0b00000011) << 2) ;}
  void SetSequenceNumber(unsigned short in){
    data[2] = (data[2] & 0b11111100) | ((in >> 12 ) & 0b00000011) ;
    data[3] = (in >> 4);
    data[4] = (data[4] & 0b00001111)  | ((in & 0b00001111) << 4 );
  }
  void SetCoarseCounter(unsigned int in){
    data[4] = (data[4] & 0b11110000)  | ((in >> 28 ) & 0b00001111);
    data[5] = in >> 20;
    data[6] = in >> 12;
    data[7] = in >> 4;
    data[8] = (data[8] & 0b00001111)  | ((in & 0b00001111) << 4 );
  }
  void SetReserved(unsigned short in){ data[8] = (data[8] & 0b11110000) | (in & 0b00001111) ;}

  void Print(){
    std::cout<<" header = "<<GetHeader()<<std::endl;
    std::cout<<" event_type = "<<GetEventType()<<std::endl;
    std::cout<<" led = "<<GetLED()<<std::endl;
    std::cout<<" gain = "<<GetGain()<<std::endl;
    std::cout<<" dac_setting = "<<GetDACSetting()<<std::endl;
    std::cout<<" type = "<<GetType()<<std::endl;
    std::cout<<" sequence_number = "<<GetSequenceNumber()<<std::endl;
    std::cout<<" coarse_counter = "<<GetCoarseCounter()<<std::endl;
    std::cout<<" reserved = "<<GetReserved()<<std::endl;
  }
  
private:
  
  unsigned char data[9];

};


union UWCTEMPMTLED{

  UWCTEMPMTLED(){bits.reset();}
  WCTEMPMTLED led;
  std::bitset<73> bits;
  

};


#endif
