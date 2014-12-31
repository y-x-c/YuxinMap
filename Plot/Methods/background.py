__author__ = 'Orthocenter'

import cv2
import numpy as np
from layer_plot_method import Layer_Plot_Method
from Utility.color import Color
import Utility.constants as constants

class Background(Layer_Plot_Method):
    def __init__(self, params, config):
        Layer_Plot_Method.__init__(self, params, config)

    def plot(self):
        color = Color()
        color.setHex(self.config.get("color"))
        color = color.getBGR()

        canvas = np.empty(constants.u_tile_shape, constants.tile_dtype)

        canvas[:, :, :] = color

        return canvas