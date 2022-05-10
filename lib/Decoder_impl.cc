/* -*- c++ -*- */
/*
 * Copyright 2022 Rajpreet K.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "Decoder_impl.h"

#define MAX_PREAMBLE_ERROR_BITS 4

namespace gr {
  namespace Packet {

    Decoder::sptr
    Decoder::make(std::vector< unsigned char> preamble, int dataLength, char sourceAddr, char destAddr, std::vector<unsigned char> data, int printAfterNFrames)
    {
      return gnuradio::get_initial_sptr
        (new Decoder_impl(preamble, dataLength, sourceAddr, destAddr, data, printAfterNFrames));
    }

    /* Function to get no of set bits in binary
    representation of positive integer n */
    unsigned int countSetBits(unsigned int n)
    {
        unsigned int count = 0;
        while (n) {
            count += n & 1;
            n >>= 1;
        }
        return count;
    }

    uint8_t PackBits(const char* in)
    {
    	uint8_t out=0;
    	for(int i=0;i<8;i++)
    	{
    		out|= in[i]<<(7-i);
    	}
    	return out;
    }

    uint8_t Decoder_impl::CheckPreAmble(const char* in)
    {
    	uint16_t rxedPreamble=0;
    	uint16_t preambleMatch=0;

    	 for(int i=0; i<16; i++)
    	 {
    		 rxedPreamble = rxedPreamble<<1;
    		 rxedPreamble|= in[i];


    	 }
    	preambleMatch = rxedPreamble ^ d_preamble;
    	return countSetBits(preambleMatch);
    }

    uint16_t DataBitErrors(const char* in, const char* in2, int len)
    {
    	uint16_t errors=0;
    	for(int i = 0; i < len; i++)
    	{
    		errors+=countSetBits(in[i]^in2[i]);
    	}
    	return errors;
    }




    /*
     * The private constructor
     */
    Decoder_impl::Decoder_impl(std::vector< unsigned char> preamble, int dataLength, char sourceAddr, char destAddr, std::vector<unsigned char> data, int printAfterNFrames)
      : gr::block("Decoder",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char)))
    {
    	//Assign arguments
    	d_preamble = ((uint16_t)preamble[0])<<8;
    	d_preamble|= preamble[1];
    	d_sourceAddr = sourceAddr;
    	d_destAddr = destAddr;
    	d_dataLength = dataLength;
    	d_printAfterNFrames = printAfterNFrames;

    	d_data = data;


		  std::bitset<8> a(preamble[0]);
		  std::cout<<"preamble1: "<<a<<std::endl;

		  std::bitset<8> b(preamble[1]);
		  std::cout<<"preamble2: "<<b<<std::endl;


		  std::bitset<16> y(d_preamble);
		  std::cout<<"preamble: "<<y<<std::endl;


    	//Initialize CRC Table
        for(size_t i = 0; i < 0x100; ++i)
          crctable[i] = crc32_for_byte(i);

        state = SEARCH_PREAMBLE;


    }


    Decoder_impl::~Decoder_impl()
    {
    }

    void
    Decoder_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
    	ninput_items_required[0] = d_dataLength + 8;
    }

    int
    Decoder_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];
      char *out = (char *) output_items[0];

      uint16_t preambleMatch;
      unsigned int preambleDiff;
      unsigned int minPreambleDiff=16;
      unsigned int maxPreambleCorrIdx;
      char rxSrcAddr;
      char rxDstAddr;

      if(ninput_items[0]<(((2+2+d_dataLength+4)*8)))
    	  return 0;


      switch (state)
      {
      case SEARCH_PREAMBLE:

          int i;
    	  for(i=0; i<ninput_items[0]; i++)
    	  {

    		  rxedPreamble = rxedPreamble<<1;
    		  rxedPreamble|= in[i];


//    		  std::bitset<16> y(rxedPreamble);
//    		  std::cout<<"in: "<<(int)in[i]<<"  "<<y<<std::endl;


    		  preambleMatch = rxedPreamble ^ d_preamble;
    		  preambleDiff = countSetBits(preambleMatch);

    		  if(preambleDiff<minPreambleDiff)
    		  {
    			  minPreambleDiff=preambleDiff;
    			  maxPreambleCorrIdx=i;

    			  if(preambleDiff==0)
    			  {
    				 state= NORMAL_AFTER_PREAMBLE;
    				 std::cout<<"22222"<<std::endl;
    				 consume_each (maxPreambleCorrIdx+1);
    				 return 0;
    			  }
    		  }

    	  }

    	  if (minPreambleDiff<=MAX_PREAMBLE_ERROR_BITS)
    	  {
    		  state= NORMAL_AFTER_PREAMBLE;
    		  std::cout<<"333333333     "<<rxedPreamble<<std::endl;
    		  consume_each (maxPreambleCorrIdx+1);
    		  return 0;
    	  }
    	  else
    	  {

    		  consume_each (ninput_items[0]);
    		  return 0;
    	  }
    	  break;

      case NORMAL_AFTER_PREAMBLE:

//    	  std::cout<<std::endl;
//
//    	  for(i=0; i<ninput_items[0]; i++)
//    	      	  {
//    		  	  	  std::cout<<(int)in[i];
//    		  	  	  if((i+1)%8 ==0)
//    		  	  	  {
//
//    		  	  		  std::cout<<"  ";
//    		  	  	  }
//    	      	  }
//    	  std::cout<<std::endl;

    	  rxSrcAddr = PackBits(&in[0]);
    	  rxDstAddr = PackBits(&in[8]);


    	  for(int j=0;j<d_dataLength;j++)
    	  {
    		  out[j] = PackBits(&in[(j+2)*8]);
    	  }

    	  dataErrorSum+= DataBitErrors(out,(char*)(&d_data[0]),d_dataLength);

    	  state = NORMAL;
    	  consume_each ((2+d_dataLength+4)*8);
    	  return d_dataLength;
    	  break;

      case NORMAL:

    	  //std::cout<<ninput_items[0]<<std::endl;
    	  unsigned int preAmbleBitErrors = CheckPreAmble(&in[0]);
    	 // std::cout<<std::endl;

//    	  for(i=0; i<ninput_items[0]; i++)
//    	      	  {
//    		  	  	  std::cout<<(int)in[i];
//    		  	  	  if((i+1)%8 ==0)
//    		  	  	  {
//
//    		  	  		  std::cout<<"  ";
//    		  	  	  }
//    	      	  }
//    	  std::cout<<std::endl;
//    	  if(preAmbleBitErrors)
//    	  {
//    		  std::cout<<"PPPPPPPPPPPPPp"<<std::endl;
//    		  return -1;
//    	  }
    	  preambleErrorSum+=preAmbleBitErrors;
    	  if(preAmbleBitErrors>MAX_PREAMBLE_ERROR_BITS)
    	  {
    		  state=SEARCH_PREAMBLE;
    		  rxedPreamble=0;
    		  return 0;
    	  }
    	  rxSrcAddr = PackBits(&in[16]);
		  rxDstAddr = PackBits(&in[24]);

		  for(int j=0;j<d_dataLength;j++)
		  {
			  out[j] = PackBits(&in[(j+4)*8]);
		  }

		  dataErrorSum+= DataBitErrors(out,(char*)(&d_data[0]),d_dataLength);
    	  consume_each ((2+2+d_dataLength+4)*8);

    	  frameCount++;
    	  if(frameCount==d_printAfterNFrames)
    	  {
    		  frameCount=0;
    		  std::cout<<"Data Bit Error Rate: "<<dataErrorSum/(d_printAfterNFrames*d_dataLength*8)<<std::endl;
    		  std::cout<<"Preamble Bit Error Rate: "<<preambleErrorSum/(d_printAfterNFrames*2*8)<<std::endl;
    		  dataErrorSum=0;
    		  preambleErrorSum=0;

    	  }
    	  return d_dataLength;
    	  break;
      }

      return 0;

    }

    uint32_t Decoder_impl::crc32_for_byte(uint32_t r)
    {
      for(int j = 0; j < 8; ++j)
        r = (r & 1? 0: (uint32_t)0xEDB88320L) ^ r >> 1;
      return r ^ (uint32_t)0xFF000000L;
    }

    void Decoder_impl::crc32(const void *data, size_t n_bytes, uint32_t* crc)
    {

      for(size_t i = 0; i < n_bytes; ++i)
        *crc = crctable[(uint8_t)*crc ^ ((uint8_t*)data)[i]] ^ *crc >> 8;
    }


  } /* namespace Packet */
} /* namespace gr */

