__author__ = 'Orthocenter'

import cv2
import numpy as np
import Utility.constants as constants
import methods
from layer_plot_method import Layer_Plot_Method
from Utility.color import Color

class Opaque(Layer_Plot_Method):
    def __init__(self, params, layer_config):
        Layer_Plot_Method.__init__(self, params, layer_config)

    def draw_contours(self, bottom_layer, black_bg):
        canny = cv2.Canny(black_bg, 0, 100)
        contours, hierarchy = cv2.findContours(canny, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)

        color = Color()
        color.setHex(self.config.get("ccolor", "#FFFFFF"))
        color = color.getBGR()
        thickness = int(self.config.get("ctn", 0))

        cv2.drawContours(bottom_layer, contours, -1, color, thickness, cv2.CV_AA)

        return bottom_layer

    def blend(self, bottom_layer, black_bg):
        if self.config.get("contours", None) == "yes":
            #bottom_layer = self.draw_contours(bottom_layer, black_bg)
            pass

        # another version, has some problem
        # if self.config.get("contours", None) == "yes":
        #     canvas = self.draw_contours(canvas)
        #
        # canvas_gray = cv2.cvtColor(canvas, cv2.COLOR_BGR2GRAY)
        #
        # canvas_gray = np.float32(canvas_gray)
        # max = canvas_gray.max()
        # if max == 0:
        #     return bottom_layer
        # mask = canvas_gray / max
        # mask **= 10
        # mask_inv = 1. - mask
        #
        # bg = np.empty_like(bottom_layer)
        # fg = np.empty_like(canvas)
        # for i in xrange(3):
        #     bg[:, :, i] = np.multiply(bottom_layer[:, :, i], mask_inv)
        #     fg[:, :, i] = np.multiply(canvas[:, :, i], mask)
        # ret = bg + fg

        # edge feather version
        # canvas_gray = np.float32(canvas_gray)
        # canvas_gray *= 1. / 255
        #
        # retval, mask = cv2.threshold(canvas_gray, 0.5, 1.0, cv2.THRESH_BINARY)
        # #mask = cv2.GaussianBlur(mask, (5, 5), 1.0)
        # mask_inv = 1. - mask
        #
        # bottom_layer_bg = np.empty_like(bottom_layer)
        # canvas_fg = np.empty_like(canvas)
        # for i in xrange(3):
        #     bottom_layer_bg[:, :, i] = np.multiply(bottom_layer[:, :, i], mask_inv)
        #     canvas_fg[:, :, i] = np.multiply(canvas[:, :, i], mask)
        #
        # ret = cv2.add(bottom_layer_bg, canvas_fg)

        return bottom_layer

    def plot(self, data, bottom_layer):
        black_bg = np.zeros(constants.u_tile_shape, constants.tile_dtype)

        num = int(data.readline())

        for elem_id in xrange(0, num):
            line = data.readline()
            layer_conf = line.rstrip(" \n").split(" ")
            layer_conf = dict([(layer_conf[i], layer_conf[i + 1]) for i in xrange(0, len(layer_conf), 2)])

            plotter = methods.get_plot_class(self.level, layer_conf)
            bottom_layer, black_bg = plotter.plot(data, bottom_layer, black_bg)

        bottom_layer = self.blend(bottom_layer, black_bg)
        return bottom_layer
