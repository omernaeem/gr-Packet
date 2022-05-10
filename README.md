<p align="center">
<img src="https://github.com/gnuradio/gnuradio/blob/main/docs/gnuradio.png" width="75%" />
</p>

## GNURadio Packet

### Brief

GNURadio Packet, is a custom block made for assisting in research. It provides two blocks

Encoder Block: This block can take input from any byte source(Example flowgraph uses a vector source with random data). It encodes the data according to the following frame format:

   2 Bytes Preamble...1 Byte Source Address...1 Byte Destination Address...Data (N bytes)...CRC32

Decoder Block: This block receives the encoded unpacked bits, synchronizes the data using preamble and outputs packed data. This block also prints two metrics:

   Preamble Bit Error Rate
   Data Bit Error Rate

The speed of printing the metrics can be set by adjusting "Print After N Frames" argument in decoder block. Larger values will result in slower printing.

### Example Flowgraph

The example flowgraph, uses a random data fixed vector as input, this input is encoded and then BPSK modulated. The modulated stream is then demodulated and passed to decoder block, which decodes the stream.
Both input and output data is displayed using QT Time Sink

To change the length of data packet adjust the Parameter InputVector

   np.random.randint(1,255,50)

Currently the following value sets data length to 50.



