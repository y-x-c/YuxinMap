__author__ = 'Orthocenter'

import cv2
import numpy as np
import methods
import Utility.constants as constants
from layer_plot_method import Layer_Plot_Method

class Transparency(Layer_Plot_Method):
    def __init__(self, params, layer_config):
        Layer_Plot_Method.__init__(self, params, layer_config)

    def blend(self, bottom_layer, canvas):
        alpha = float(self.config.get("alpha"))

        ret = cv2.addWeighted(bottom_layer, 1 - alpha, canvas, alpha, 0)
        return ret

    def plot(self, elems, bottom_layer):
        canvas = np.zeros(constants.u_tile_shape, constants.tile_dtype)

        for elem in elems:
            plotter = methods.get_plot_class(self.params, elem)
            canvas, dummy = plotter.plot(canvas, None)

        bottom_layer = self.blend(bottom_layer, canvas)
        return bottom_layer