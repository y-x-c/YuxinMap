__author__ = 'Orthocenter'

import cv2
import numpy as np
import Utility.constants as constants
from plot_method import Plot_Method
from Utility.color import Color

class Line(Plot_Method):
    def __init__(self, level, conf):
        Plot_Method.__init__(self, level, conf)
        color = Color()
        color.setHex("#C1CAD0")
        self.thinLineColor = color.getBGR()

    def plot(self, data, bottom_layer, black_bg):
        np_pts = self.get_np_pts(data)
        color = self.get_color()

        # if tn_level is not defined, then regard thickness of this line is invariable
        tn_level = int(self.conf.get("tn_level", self.level))
        if(self.level <= tn_level):
            zoomOutRatio = float(self.conf.get("zoomOutRatio", 2))
            thickness = int(self.conf.get("thickness")) * (zoomOutRatio ** (self.level - tn_level) * constants.sampling_factor)
        else:
            zoomInRatio = float(self.conf.get("zoomOutRatio", 2))
            thickness = int(self.conf.get("thickness")) * (zoomInRatio ** (self.level - tn_level) * constants.sampling_factor)

        if thickness <= 3:
            thickness = 1
            if color == (255, 255, 255):
                color = self.thinLineColor

        if not bool(self.conf.get("hollow", False)):
            cv2.polylines(bottom_layer, np_pts, False, color, int(thickness), cv2.CV_AA)
        if black_bg != None and thickness > 3:
            cv2.polylines(black_bg, np_pts, False, color, int(thickness), cv2.CV_AA)

        return bottom_layer, black_bg