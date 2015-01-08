__author__ = 'Orthocenter'

import Utility.constants as constants
import numpy as np
import cv2
import time
import Methods.methods as methods
# utilities
from Utility.get_name import get_name
from Utility.coordinate import Coordinate
import os

try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET

class Plot():
    def __init__(self, data_filename):
        self.data_filename = data_filename
        self.level = int(data_filename.split("_")[2].split(".")[0])

    def plot(self, output_path, output_path_retina):
        data = open(self.data_filename, "r")
        bg_conf = data.readline().rstrip(" \n").split(" ")
        bg_conf = dict([(bg_conf[i], bg_conf[i + 1]) for i in xrange(0, len(bg_conf), 2)])

        bg_plotter = methods.get_layer_plot_class(self.level, bg_conf)
        bottom_layer = bg_plotter.plot(data)

        line = data.readline()
        while line:
            layer_conf = line.rstrip(" \n").split(" ")
            layer_conf = dict([(layer_conf[i], layer_conf[i + 1]) for i in xrange(0, len(layer_conf), 2)])

            layer_plot = methods.get_layer_plot_class(self.level, layer_conf)
            layer_plot.plot(data, bottom_layer)

            #cv2.imwrite("output%d.png" % int(layer_conf.get("layer")), bottom_layer)

            line = data.readline()

        l = constants.padding
        r = constants.tile_l * constants.sampling_factor * constants.retina_factor + constants.padding
        bottom_layer = bottom_layer[l : r, l : r, : ]

        ret = cv2.resize(bottom_layer, constants.tile_size, interpolation=cv2.INTER_AREA)
        cv2.imwrite(output_path, ret)
        ret = cv2.resize(bottom_layer, constants.retina_tile_size, interpolation=cv2.INTER_AREA)
        cv2.imwrite(output_path_retina, ret)


if __name__ == "__main__":
    plotter = Plot("/Users/Orthocenter/Developments/YuxinMap/Utility/Filter/Filter/Output/1715_836_11.dt")

    plotter.plot("output.png")