__author__ = 'Orthocenter'

import cv2
import numpy as np
from plot_method import Plot_Method

class Fill_Poly(Plot_Method):
    def __init__(self, level, links, elem):
        Plot_Method.__init__(self, level, links, elem)

    def plot(self, canvas):
        np_pts = self.get_np_pts()
        color = self.get_color()

        cv2.fillPoly(canvas, np_pts, color, cv2.CV_AA)

        return canvas