#ifndef DAQ_HEADER_H
#define DAQ_HEADER_H

#include <vector>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <bitset>

/* DAQHeader
 *
 * Fields:
 * bits    width field
 *  0 - 31 32    MessageNumber
 * 32 - 63 32    CoarseCounter
 * 64 - 65  2    CardType
 * 66 - 77 12    CardID
 * 78 - 92 15    NumberOfWords
 * 93 - 93  1    PayloadEarly
 * 94 - 94  1    DelayedPackets
 * 95 - 95  1    Reserved
 *
 * - MessageNumber: unique message number incremented by 1 for each message
 *   (sent back as akn receipt). Messages should be sent every 100ms
 *
 * - CoarseCounter: coarse counter at start of packet generation (LSB = 0.52
 *   ms). A new packet is sent every 1.25 M coarse ticks (10 ms). max ~26 days
 *
 * - CardType: IWCD WCTE mPMT (00), Buffered ADC readout (01), FD-mPMT (10), Trigger mainboard (11)
 *
 * - CardID: unique card ID (0-4095)
 *
 * - NumberOfWords: number of words in preceeding payload
 *
 * - PayloadEarly: first bit of payload sent early due to full memory
 *
 * - DelayedPackets: the packet was delayed; more data may be waiting in the buffer
 *
 * Bits map:
 *    01234567
 *  0 mmmmmmmm          m = MessageNumber
 *  1 mmmmmmmm          c = CoarseCounter
 *  2 mmmmmmmm          T = CardType
 *  3 mmmmmmmm          I = CardID
 *  4 cccccccc          w = NumberOfWords
 *  5 cccccccc          p = PayloadEarly
 *  6 cccccccc          d = DelayedPackets
 *  7 cccccccc          R = Reserved
 *  8 TTIIIIII
 *  9 IIIIIIww
 * 10 wwwwwwww
 * 11 wwwwwpdR
 */

class DAQHeader{

public:
  DAQHeader(){std::fill(std::begin(data), std::end(data), 0);}
  
  unsigned int GetMessageNumber(){return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | (data[3]); }
  unsigned int GetCoarseCounter(){return (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | (data[7]);}
  unsigned short GetCardType(){return ((data[8] & 0b11000000) >> 6); }
  unsigned short GetCardID(){return ((data[8] & 0b00111111) << 6) | ((data[9] & 0b11111100) >> 2);}
  unsigned short GetNumberOfWords(){ return ((data[9] & 0b00000011) << 13) | (data[10] << 5) | ((data[11] & 0b11111000 ) >> 3) ;}
  bool GetPayloadEarly(){return ((data[11] & 0b00000100) >> 1 );}
  bool GetDelayedPackets(){return ((data[11] & 0b00000010) >> 1 );}
  bool GetReserved(){return (data[11] & 0b00000001);}
  unsigned short GetVersion(){return data[12] ;}

  void SetMessageNumber(unsigned int in){
    data[0] = in >> 24;
    data[1] = in >> 16;
    data[2] = in >> 8;
    data[3] = in;
  }
  void SetCoarseCounter(unsigned int in){
    data[4] = in >> 24;
    data[5] = in >> 16;
    data[6] = in >> 8;
    data[7] = in;
  }
  void SetCardType(unsigned short in){ data[8] = (data[8] & 0b00111111) | ((in & 0b00000011) << 6);}
  void SetCardID(unsigned short in){
    data[8] = (data[8] & 0b11000000) | (in >> 6) & 0b00111111;
    data[9] = (data[9] & 0b00000011) | ((in & 0b00111111) << 2);
  }
  void SetNumberOfWords(unsigned short in){
    data[9] = (data[9] & 0b11111100) | ((in >> 13) & 0b00000011);
    data[10] = in >> 5;
    data[11] = (data[11] & 0b00000111)  | ((in << 3) & 0b11111000);
   }
  void SetPayloadEarly(bool in){ data[11] = (data[11] & 0b11111011) | ((in & 0b00000001) << 2);}
  void SetDelayedPackets(bool in){ data[11] = (data[11] & 0b11111101) | ((in & 0b00000001) << 1);}
  void SetReserved(bool in){ data[11] = (data[11] & 0b11111110) | (in & 0b00000001);}
  void SetVersion(unsigned short in){ data[12] = in;}
  void Print(){
    std::cout<<" message_number = "<<GetMessageNumber()<<std::endl;
    std::cout<<" coarse_counter = "<<GetCoarseCounter()<<std::endl;
    std::cout<<" card_type = "<<GetCardType()<<std::endl;
    std::cout<<" card_id = "<<GetCardID()<<std::endl;
    std::cout<<" number_of_words = "<<GetNumberOfWords()<<std::endl;
    std::cout<<" payload_early = "<<GetPayloadEarly()<<std::endl;
    std::cout<<" delayed_packets = "<<GetDelayedPackets()<<std::endl;
    std::cout<<" reserved = "<<GetReserved()<<std::endl;
    std::cout<<" version = "<<GetVersion()<<std::endl;
  }
  
private:
  
  unsigned char data[13];

};


union UDAQHeader{

  UDAQHeader(){bits.reset();}
  DAQHeader hit;
  std::bitset<104> bits;
  

};


#endif
