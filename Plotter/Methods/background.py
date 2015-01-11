__author__ = 'Orthocenter'

import cv2
import numpy as np
from layer_plot_method import Layer_Plot_Method
from Utility.color import Color
import Utility.constants as constants
from opaque import Opaque

class Background(Opaque):
    def __init__(self, level, config):
        #Opaque.__init__(self, level, config)
        self.level = level
        self.config = config

    def plot(self, data):
        bg_path = data.readline().rstrip(" \n")
        canvas = cv2.imread(bg_path)

        #color = Color()
        #color.setRGB(0, 0, 0)
        #color = color.getBGR()

        #canvas = np.empty(constants.u_tile_shape, constants.tile_dtype)

        #canvas[:, :, :] = color

        #Opaque.plot(self, data, canvas)

        return canvas