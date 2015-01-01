__author__ = 'Orthocenter'

import cv2
import numpy as np
import Utility.constants as constants
from plot_method import Plot_Method
from Utility.color import Color

class Line(Plot_Method):
    def __init__(self, level, conf):
        Plot_Method.__init__(self, level, conf)

    def plot(self, data, bottom_layer, black_bg):
        np_pts = self.get_np_pts(data)
        color = self.get_color()

        # if tn_level is not defined, then regard thickness of this line is invariable
        tn_level = int(self.conf.get("tn_level", self.level))
        thickness = int(self.conf.get("thickness")) * (2 ** (self.level - tn_level) * constants.sampling_factor)
        #if thickness <= 5:
            #thickness = 1
        cv2.polylines(bottom_layer, np_pts, False, color, int(thickness), cv2.CV_AA)
        if black_bg != None:
            cv2.polylines(black_bg, np_pts, False, color, int(thickness), cv2.CV_AA)

        return bottom_layer, black_bg