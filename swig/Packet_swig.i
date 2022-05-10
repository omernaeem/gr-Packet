/* -*- c++ -*- */

#define PACKET_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "Packet_swig_doc.i"

%{
#include "Packet/Encoder.h"
#include "Packet/Decoder.h"
%}

%include "Packet/Encoder.h"
GR_SWIG_BLOCK_MAGIC2(Packet, Encoder);
%include "Packet/Decoder.h"
GR_SWIG_BLOCK_MAGIC2(Packet, Decoder);
