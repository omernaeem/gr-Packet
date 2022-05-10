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
#include "Encoder_impl.h"

namespace gr {
  namespace Packet {


    Encoder::sptr
    Encoder::make(std::vector< unsigned char> preamble, int dataLength, char sourceAddr, char destAddr)
    {
      return gnuradio::get_initial_sptr
        (new Encoder_impl(preamble, dataLength, sourceAddr, destAddr));
    }

    /*Function to generate CRC Lookup Table*/
    uint32_t Encoder_impl::crc32_for_byte(uint32_t r)
    {
      for(int j = 0; j < 8; ++j)
        r = (r & 1? 0: (uint32_t)0xEDB88320L) ^ r >> 1;
      return r ^ (uint32_t)0xFF000000L;
    }

    void Encoder_impl::crc32(const void *data, size_t n_bytes, uint32_t* crc)
    {

      for(size_t i = 0; i < n_bytes; ++i)
        *crc = crctable[(uint8_t)*crc ^ ((uint8_t*)data)[i]] ^ *crc >> 8;
    }

    Encoder_impl::Encoder_impl(std::vector< unsigned char> preamble, int dataLength, char sourceAddr, char destAddr)
      : gr::block("Encoder",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char)))
    {

    	//Assign arguments
    	d_preamble[0] = preamble[0];
    	d_preamble[1] = preamble[1];
    	d_sourceAddr = sourceAddr;
    	d_destAddr = destAddr;
    	d_dataLength = dataLength;

    	//Initialize CRC Table
        for(size_t i = 0; i < 0x100; ++i)
          crctable[i] = crc32_for_byte(i);

    }


    Encoder_impl::~Encoder_impl()
    {
    }

    void
    Encoder_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
    	ninput_items_required[0] = d_dataLength;
    }

    int
    Encoder_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];
      char *out = (char *) output_items[0];

      //If input data is less than data length, wait for more data
      if(ninput_items[0]<d_dataLength)
    	  return 0;

      //Create Output Frame

      out[0] = d_preamble[0];
      out[1] = d_preamble[1];
      out[2] = d_sourceAddr;
      out[3] = d_destAddr;
      memcpy(&out[4],in,d_dataLength);

      //Generate CRC
      uint32_t crcVal;
      crc32(in,d_dataLength,&crcVal);
      //Append 32 bit CRC at end
      memcpy(&out[4+d_dataLength],&crcVal,4);

      // Tell runtime system how many input items we consumed on input stream.
      consume_each (d_dataLength);

      // Tell runtime system how many output items we produced.
      return d_dataLength + 8;
    }

  } /* namespace Packet */
} /* namespace gr */

