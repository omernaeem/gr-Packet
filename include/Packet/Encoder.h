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

#ifndef INCLUDED_PACKET_ENCODER_H
#define INCLUDED_PACKET_ENCODER_H

#include <Packet/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace Packet {

    /*!
     * \brief <+description of block+>
     * \ingroup Packet
     *
     */
    class PACKET_API Encoder : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<Encoder> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of Packet::Encoder.
       *
       * To avoid accidental use of raw pointers, Packet::Encoder's
       * constructor is in a private implementation
       * class. Packet::Encoder::make is the public interface for
       * creating new instances.
       */
      static sptr make(std::vector< unsigned char> preamble, int dataLength, char sourceAddr, char destAddr);
    };

  } // namespace Packet
} // namespace gr

#endif /* INCLUDED_PACKET_ENCODER_H */

