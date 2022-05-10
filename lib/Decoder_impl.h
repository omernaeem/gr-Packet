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

#ifndef INCLUDED_PACKET_DECODER_IMPL_H
#define INCLUDED_PACKET_DECODER_IMPL_H

#include <Packet/Decoder.h>

namespace gr {
  namespace Packet {

    class Decoder_impl : public Decoder
    {
     private:

        int d_dataLength;
        uint16_t d_preamble;
        char d_sourceAddr;
        char d_destAddr;
        std::vector<unsigned char> d_data;

        float preambleErrorSum=0;
        float dataErrorSum=0;
        int d_printAfterNFrames;

        int frameCount=0;

        uint16_t rxedPreamble=0;
        uint32_t crctable[0x100];

        typedef enum state_t {SEARCH_PREAMBLE,NORMAL_AFTER_PREAMBLE, NORMAL}state_t;

        state_t state;



     public:
      Decoder_impl(std::vector< unsigned char> preamble, int dataLength, char sourceAddr, char destAddr, std::vector<unsigned char> data, int printAfterNFrames);
      ~Decoder_impl();


      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

      uint32_t crc32_for_byte(uint32_t r);
      void crc32(const void *data, size_t n_bytes, uint32_t* crc);
      uint8_t CheckPreAmble(const char* in);

    };

  } // namespace Packet
} // namespace gr

#endif /* INCLUDED_PACKET_DECODER_IMPL_H */

