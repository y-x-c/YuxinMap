__author__ = 'Orthocenter'

import numpy as np
from Utility.get_name import get_name
from Utility.color import Color
import Utility.constants as constants

class Plot_Method():
    def __init__(self, level, conf):
        self.level = level
        self.conf = conf

    def get_np_pts(self, data):
        pts = []
        coords = data.readline().rstrip(" \n").split(" ")

        for i in xrange(0, len(coords), 2):
            x_y = [int(coords[i + _]) + constants.padding for _ in range(0, 2)]
            pts.append(x_y)

        np_pts = [np.array(pts, np.int32)]
        np.reshape(np_pts, (-1, 1, 2))

        return np_pts

    def get_color(self):
        color = Color()
        color.setHex(self.conf.get("color"))
        color = color.getBGR()

        return color

    def plot(self):
        pass
