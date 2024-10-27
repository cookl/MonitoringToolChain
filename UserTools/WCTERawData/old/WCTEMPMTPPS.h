#ifndef WCTE_MPMT_PPS_H
#define WCTE_MPMT_PPS_H

#include <vector>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <bitset>

/* WCTEMPMTPPS:
 *
 * Fields:
 * bits     width field
 *  0 -   1  2    Header
 *  2 -   5  4    EventType
 *  6 -   7  2    Reserved
 *  8 -  71 64    PreviousPPSCoarseCounter
 * 72 - 135 64    CurrentPPSCoarseCounter
 *
 * - Header: packet ID (0b01)
 *
 * - EventType: PPS (0xF)
 *
 * - Reserved:
 *   - lower bit set to 1 iff this PPS was used to reset the coarse counter
 *   - higher bit reserved for future use
 *
 * - PrevousPPSCoarseCounter: full coarse counter value at previous PPS
 *
 * - CurrentPPSCoarseCounter: full coarse counter value at current PPS. If this
 *   is the designated PPS at run start, report value immediately prior to
 *   coarse counter being set to zero.
 *
 * Full course counter values: if only 48 bits are available, pad upper 16 bits
 * with zeroes
 *
 * Bits map:
 *    01234567          h = Header
 *  0 hhEEEERr          E = EventType
 *  1 PPPPPPPP          R = reserved                 (Reserved & 0b10)
 *  2 PPPPPPPP          r = reset the coarse counter (Reserved & 0b01)
 *  3 PPPPPPPP          P = PreviousPPSCoarseCounter
 *  4 PPPPPPPP          C = CurrentPPSCoarseCounter
 *  5 PPPPPPPP
 *  6 PPPPPPPP
 *  7 PPPPPPPP
 *  8 PPPPPPPP
 *  9 CCCCCCCC
 * 10 CCCCCCCC
 * 11 CCCCCCCC
 * 12 CCCCCCCC
 * 13 CCCCCCCC
 * 14 CCCCCCCC
 * 15 CCCCCCCC
 * 16 CCCCCCCC
 */

class WCTEMPMTPPS{

public:
  WCTEMPMTPPS(){std::fill(std::begin(data), std::end(data), 0);}
  
  unsigned short GetHeader(){return (data[0] & 0b11000000) >> 6; }
  unsigned short GetEventType(){return (data[0] & 0b00111100) >> 2;}
  unsigned short GetReserved(){return (data[0] & 0b00000011); }
  unsigned long GetPreviousPPSCoarseCounter(){ return (((unsigned long)data[1]) << 56) | (((unsigned long)data[2]) << 48) | (((unsigned long)data[3]) << 40) | (((unsigned long)data[4]) << 32) | (data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8]; }
  unsigned long GetCurrentPPSCoarseCounter(){return (((unsigned long)data[9]) << 56) | (((unsigned long)data[10]) << 48) | (((unsigned long)data[11]) << 40) | (((unsigned long)data[12]) << 32) | (data[13] << 24) | (data[14] << 16) | (data[15] << 8) | data[16];  }
 
  void SetHeader(unsigned short in){ data[0] = (data[0] & 0b00111111) | ((in & 0b00000011) << 6);}
  void SetEventType(unsigned short in){ data[0] = (data[0] & 0b11000011) | (( in & 0b00001111) << 2) ;}
  void SetReserved(unsigned short in){ data[0] = (data[0] & 0b11111100) | (in & 0b00000011);}
  void SetPreviousPPSCoarseCounter(unsigned long in){
    data[1] = in >> 56;
    data[2] = in >> 48;
    data[3] = in >> 40;
    data[4] = in >> 32;
    data[5] = in >> 24;
    data[6] = in >> 16;
    data[7] = in >> 8;
    data[8] = in;
  }
  void SetCurrentPPSCoarseCounter(unsigned long in){
    data[9] = in >> 56;
    data[10] = in >> 48;
    data[11] = in >> 40;
    data[12] = in >> 32;
    data[13] = in >> 24;
    data[14] = in >> 16;
    data[15] = in >> 8;
    data[16] = in;
  }
  
  void Print(){
    std::cout<<" header = "<<GetHeader()<<std::endl;
    std::cout<<" event_type = "<<GetEventType()<<std::endl;
    std::cout<<" reserved = "<<GetReserved()<<std::endl;
    std::cout<<" previous_PPS_coarse_counter = "<<GetPreviousPPSCoarseCounter()<<std::endl;
    std::cout<<" current_PPS_coarse_counter = "<<GetCurrentPPSCoarseCounter()<<std::endl;
    
  }
  
private:
  
  unsigned char data[17];

};


union UWCTEMPMTPPS{

  UWCTEMPMTPPS(){bits.reset();}
  WCTEMPMTPPS pps;
  std::bitset<136> bits;
  

};


#endif
