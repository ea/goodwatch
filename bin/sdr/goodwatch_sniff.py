#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Goodwatch Sniff
# Generated: Sun Nov 18 17:15:36 2018
##################################################

from gnuradio import analog
from gnuradio import blocks
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import math
import osmosdr
import time


class goodwatch_sniff(gr.top_block):

    def __init__(self, center_frequency=433e6, correction=-31e3):
        gr.top_block.__init__(self, "Goodwatch Sniff")

        ##################################################
        # Parameters
        ##################################################
        self.center_frequency = center_frequency
        self.correction = correction

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 1.8e6
        self.decimation = decimation = 5
        self.baud_rate = baud_rate = 1.19948e3
        self.access_code = access_code = '11010011100100011101001110010001'

        ##################################################
        # Blocks
        ##################################################
        self.osmosdr_source_0 = osmosdr.source( args="numchan=" + str(1) + " " + "" )
        self.osmosdr_source_0.set_sample_rate(samp_rate)
        self.osmosdr_source_0.set_center_freq(center_frequency+correction, 0)
        self.osmosdr_source_0.set_freq_corr(0, 0)
        self.osmosdr_source_0.set_dc_offset_mode(0, 0)
        self.osmosdr_source_0.set_iq_balance_mode(0, 0)
        self.osmosdr_source_0.set_gain_mode(False, 0)
        self.osmosdr_source_0.set_gain(20, 0)
        self.osmosdr_source_0.set_if_gain(0, 0)
        self.osmosdr_source_0.set_bb_gain(0, 0)
        self.osmosdr_source_0.set_antenna("", 0)
        self.osmosdr_source_0.set_bandwidth(50000, 0)
          
        self.low_pass_filter_0 = filter.fir_filter_ccf(decimation, firdes.low_pass(
        	1, samp_rate, 12000, 3e3, firdes.WIN_HAMMING, 6.76))
        self.digital_correlate_access_code_bb_0 = digital.correlate_access_code_bb(access_code, 1)
        self.digital_clock_recovery_mm_xx_0 = digital.clock_recovery_mm_ff(samp_rate/decimation/baud_rate, 0.25*0.175*0.175, 0.5, 0.175, 0.005)
        self.digital_binary_slicer_fb_0 = digital.binary_slicer_fb()
        self.blocks_moving_average_xx_0 = blocks.moving_average_ff(int(samp_rate/decimation/baud_rate), 1/(1.0*samp_rate/baud_rate), 4000)
        self.blocks_file_sink_0 = blocks.file_sink(gr.sizeof_char*1, "test.bin", False)
        self.blocks_file_sink_0.set_unbuffered(False)
        self.analog_quadrature_demod_cf_0 = analog.quadrature_demod_cf(1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_quadrature_demod_cf_0, 0), (self.blocks_moving_average_xx_0, 0))    
        self.connect((self.blocks_moving_average_xx_0, 0), (self.digital_clock_recovery_mm_xx_0, 0))    
        self.connect((self.digital_binary_slicer_fb_0, 0), (self.digital_correlate_access_code_bb_0, 0))    
        self.connect((self.digital_clock_recovery_mm_xx_0, 0), (self.digital_binary_slicer_fb_0, 0))    
        self.connect((self.digital_correlate_access_code_bb_0, 0), (self.blocks_file_sink_0, 0))    
        self.connect((self.low_pass_filter_0, 0), (self.analog_quadrature_demod_cf_0, 0))    
        self.connect((self.osmosdr_source_0, 0), (self.low_pass_filter_0, 0))    

    def get_center_frequency(self):
        return self.center_frequency

    def set_center_frequency(self, center_frequency):
        self.center_frequency = center_frequency
        self.osmosdr_source_0.set_center_freq(self.center_frequency+self.correction, 0)

    def get_correction(self):
        return self.correction

    def set_correction(self, correction):
        self.correction = correction
        self.osmosdr_source_0.set_center_freq(self.center_frequency+self.correction, 0)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_moving_average_xx_0.set_length_and_scale(int(self.samp_rate/self.decimation/self.baud_rate), 1/(1.0*self.samp_rate/self.baud_rate))
        self.digital_clock_recovery_mm_xx_0.set_omega(self.samp_rate/self.decimation/self.baud_rate)
        self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.samp_rate, 12000, 3e3, firdes.WIN_HAMMING, 6.76))
        self.osmosdr_source_0.set_sample_rate(self.samp_rate)

    def get_decimation(self):
        return self.decimation

    def set_decimation(self, decimation):
        self.decimation = decimation
        self.blocks_moving_average_xx_0.set_length_and_scale(int(self.samp_rate/self.decimation/self.baud_rate), 1/(1.0*self.samp_rate/self.baud_rate))
        self.digital_clock_recovery_mm_xx_0.set_omega(self.samp_rate/self.decimation/self.baud_rate)

    def get_baud_rate(self):
        return self.baud_rate

    def set_baud_rate(self, baud_rate):
        self.baud_rate = baud_rate
        self.blocks_moving_average_xx_0.set_length_and_scale(int(self.samp_rate/self.decimation/self.baud_rate), 1/(1.0*self.samp_rate/self.baud_rate))
        self.digital_clock_recovery_mm_xx_0.set_omega(self.samp_rate/self.decimation/self.baud_rate)

    def get_access_code(self):
        return self.access_code

    def set_access_code(self, access_code):
        self.access_code = access_code


def argument_parser():
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option(
        "-f", "--center-frequency", dest="center_frequency", type="eng_float", default=eng_notation.num_to_str(433e6),
        help="Set center_frequency [default=%default]")
    parser.add_option(
        "-c", "--correction", dest="correction", type="eng_float", default=eng_notation.num_to_str(-31e3),
        help="Set correction [default=%default]")
    return parser


def main(top_block_cls=goodwatch_sniff, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    tb = top_block_cls(center_frequency=options.center_frequency, correction=options.correction)
    tb.start()
    try:
        raw_input('Press Enter to quit: ')
    except EOFError:
        pass
    tb.stop()
    tb.wait()


if __name__ == '__main__':
    main()
