__author__ = 'Orthocenter'

import cv2
import numpy as np
from plot_method import Plot_Method
from Utility.color import Color

class Line(Plot_Method):
    def __init__(self, level, links, elem):
        Plot_Method.__init__(self, level, links, elem)

    def plot(self, canvas):
        np_pts = self.get_np_pts()
        color = self.get_color()

        # if tn_level is not defined, then regard thickness of this line is invariable
        tn_level = int(self.elem.get("tn_level", self.level))
        thickness = int(self.elem.get("thickness")) * (2 ** (self.level - tn_level))
        cv2.polylines(canvas, np_pts, False, color, int(thickness), cv2.CV_AA)

        return canvas