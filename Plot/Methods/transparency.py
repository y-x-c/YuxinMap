__author__ = 'Orthocenter'

import cv2
import numpy as np
from blend_method import Blend_Method

class Transparency(Blend_Method):
    def __init__(self, layer_config):
        Blend_Method.__init__(self, layer_config)

    def blend(self, bottom_layer, canvas):
        alpha = float(self.config.get("alpha"))

        ret = cv2.addWeighted(bottom_layer, 1 - alpha, canvas, alpha, 0)
        return ret

