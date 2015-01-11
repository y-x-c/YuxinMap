__author__ = 'Orthocenter'

import cv2
import numpy as np
from plot_method import Plot_Method

class Fill_Poly(Plot_Method):
    def __init__(self, level, elem):
        Plot_Method.__init__(self, level, elem)

    def plot(self, data, bottom_layer, black_bg):
        np_pts = self.get_np_pts(data)
        color = self.get_color()

        cv2.fillPoly(bottom_layer, np_pts, color, cv2.CV_AA)
        if black_bg != None:
            cv2.fillPoly(black_bg, np_pts, color, cv2.CV_AA)

        return bottom_layer, black_bg