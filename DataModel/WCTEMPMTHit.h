#ifndef WCTE_MPMT_HIT_H
#define WCTE_MPMT_HIT_H

#include <vector>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <bitset>

/* WCTEMPMTHit:
 *
 * Fields:
 * bits    width field
 *  0 -  1  2    Header
 *  2 -  5  4    EventType
 *  6 - 10  5    Channel
 * 11 - 15  5    Flags
 * 16 - 47 32    CoarseCounter
 * 48 - 63 16    FineTime
 * 64 - 79 16    Charge
 * 80 - 87  8    QualityFactor
 *
 * - Header: packet ID (0b01)
 *
 * - EventType:
 *   -   0: normal PMT
 *   -   1: pedestal
 *   -   2: LED
 *   -   3: calibration
 *   - 0xF: PPS
 *
 * - Channel: sub-PMT ID (0-32)
 *
 * - Flags (bits 4..0): packet enable (4 bit), compression enable (3 bit),
 *   waveform enable (2 bit)
 *
 * - CoarseCounter: lower 32 bits of coarse counter - when combined with the 16
 *   upper bits from the packet header, the 48-bit coarse counter is the coarse
 *   time estimate for the hit. LSB is 8 ns. Max ~34.45 s.
 *
 * - FineTime: time estimate within one 8 ns coarse time bin. The full range of
 *   fine time is [0,8) ns. LSB is 1/8192 ns. The sum of the 48-bit coarse
 *   counter and fine time is time relative to run start.
 *
 * - Charge: ADC counts
 *
 * - QualityFactor: quality factor for the time estimate
 *
 * Bits map:
 *    01234567
 *  0 hhEEEEcc          h = Header
 *  1 cccpzwRR          E = EventType
 *  2 TTTTTTTT          c = Channel
 *  3 TTTTTTTT          p = packet enable      (Flags & 0b10000)
 *  4 TTTTTTTT          z = compression enable (Flags & 0b01000)
 *  5 TTTTTTTT          w = waveform enable    (Flags & 0b00100)
 *  6 tttttttt          R = reserved           (Flags & 0b00011)
 *  7 tttttttt          T = CoarseCounter
 *  8 CCCCCCCC          t = FineTime
 *  9 CCCCCCCC          C = Charge
 * 10 QQQQQQQQ          Q = QualityFactor
 */

class WCTEMPMTHit{

public:
  WCTEMPMTHit(){std::fill(std::begin(data), std::end(data), 0);}
  
  unsigned short GetCardID(){return card_id;}
  unsigned short GetHeader(){return (data[0] & 0b11000000) >> 6; }
  unsigned short GetEventType(){return (data[0] & 0b00111100) >> 2;}
  unsigned short GetChannel(){return ((data[0] & 0b00000011) << 3) | ((data[1] & 0b11100000) >> 5); }
  unsigned short GetFlags(){return (data[1] & 0b00011111);}
  unsigned int GetCoarseCounter(){ return (data[2] << 24) | (data[3] << 16) | (data[4] << 8 ) | (data[5]) ;}
  unsigned short GetFineTime(){return (data[6] << 8) | (data[7]);}
  unsigned short GetCharge(){return (data[8] << 8 ) | data[9] ;}
  unsigned short GetQualityFactor(){return data[10];}

  void SetCardID(unsigned short in){ card_id=in;}
  void SetHeader(unsigned short in){ data[0] = (data[0] & 0b00111111) | ((in & 0b00000011) << 6);}
  void SetEventType(unsigned short in){ data[0] = (data[0] & 0b11000011) | (( in & 0b00001111) << 2) ;}
  void SetChannel(unsigned short in){
    data[0] = (data[0] & 0b11111100) | ((in & 0b00011000) >> 3);
    data[1] = (data[1] & 0b00011111)  | ((in & 0b00000111) << 5);
   }
  void SetFlags(unsigned short in){ data[1] = (data[1] & 0b11100000) | (in & 0b00011111);}
  void SetCoarseCounter(unsigned int in){
    data[2] = in >> 24;
    data[3] = in >> 16;
    data[4] = in >> 8;
    data[5] = in;
  }
  void SetFineTime(unsigned short in){
    data[6] = in >> 8;
    data[7] = in;
  }
  void SetCharge(unsigned short in){
    data[8] = in >> 8;
    data[9] = in;
  }
  void SetQualityFactor(unsigned short in){ data[10] = in;}
  void Print(){
    std::cout<<" header = "<<GetHeader()<<std::endl;
    std::cout<<" event_type = "<<GetEventType()<<std::endl;
    std::cout<<" channel = "<<GetChannel()<<std::endl;
    std::cout<<" flags = "<<GetFlags()<<std::endl;
    std::cout<<" coarse_counter = "<<GetCoarseCounter()<<std::endl;
    std::cout<<" fine_time = "<<GetFineTime()<<std::endl;
    std::cout<<" charge = "<<GetCharge()<<std::endl;
    std::cout<<" quality_factor = "<<GetQualityFactor()<<std::endl;
  }
  
private:

  unsigned short card_id;
  unsigned char data[11];

};


union UWCTEMPMTHit{

  UWCTEMPMTHit(){bits.reset();}
  WCTEMPMTHit hit;
  std::bitset<88> bits;
  

};


#endif
