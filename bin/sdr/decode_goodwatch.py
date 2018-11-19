import struct
#borrowed from Michael Ossmann, till i figure out how to do it form gnuradio directly
maxbytes = 32
data = open('test.bin').read()
symbols = struct.unpack('1B'*len(data), data)
pkts = []
whole_msg = ""
# extract an integer value from bitstream
def extract(start, len, bits):
        val = 0
        for i in range(start, start + len):
                val <<= 1
                val += (bits[i] & 1)
        return val

# make a big integer out of a packet's bits and display it in hex
def decode_packet(start):
	global whole_msg
        pkts.append(extract(start, maxbytes*8, symbols))
        hexformat = "%0" + str(maxbytes * 2) + "x"
        msg = hexformat % pkts[len(pkts) - 1]
	whole_msg+=msg.decode("hex")

# look for correlations flagged by correlate_access_code_bb (second bit set)
for i in range(len(symbols) - maxbytes*8):
        if symbols[i] & 2:
                print
                decode_packet(i)
print whole_msg
