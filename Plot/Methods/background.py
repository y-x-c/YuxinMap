__author__ = 'Orthocenter'

import cv2
import numpy as np
from Utility.color import Color
import Utility.constants as constants

class Background():
    def __init__(self, config):
        self.config = config

    def plot(self):
        color = Color()
        color.setHex(self.config.get("color"))
        color = color.getBGR()

        canvas = np.empty(constants.u_tile_shape, constants.tile_dtype)

        canvas[:, :, :] = color

        return canvas