__author__ = 'Orthocenter'

import Utility.constants as constants
import numpy as np
import cv2
import time
import Methods.methods as methods
# utilities
from Utility.filter import Filter
from Utility.get_name import get_name
from Utility.coordinate import Coordinate
import os

try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET

class Plot():
    def __init__(self, level, OSM_file, plot_config, tx, ty):
        self.level, self.OSM_file, self.plot_config = level, OSM_file, plot_config
        self.tree_config = ET.parse(self.plot_config)

        self.tree_osm = ET.parse(self.OSM_file)

        self.root_osm = self.tree_osm.getroot()
        self.root_config = self.tree_config.getroot()
        self.root_methods = self.root_config.find("./elements")
        self.root_layers = self.root_config.find("./layers")
        self.tx = tx
        self.ty = ty

    def set_methods(self):
        for method in self.root_methods:
            for rule in method.findall("./filter"):
                filter = Filter()
                ret = filter.filter(self.root_osm, rule.find('./'))

                # set attributes
                for elem in ret:
                    for k, v in method.items():
                        elem.set(k, v)

        # output
        # split = os.path.splitext(self.OSM_file)
        # output_path = '_methods'.join(split)
        # self.tree_osm.write(output_path, encoding = "UTF-8")

    def set_layers(self):
        self.layer_configs = {}
        for layer_config in self.root_layers:
            if layer_config.get("layer") == "background":
                self.bg_config = layer_config
            else:
                layer_id = layer_config.get("layer")
                self.layer_configs[float(layer_id)] = layer_config

                for rule in layer_config.findall("./filter"):
                    filter = Filter()
                    ret = filter.filter(self.root_osm, rule.find('./'))

                    # set layer_id
                    for elem in ret:
                        found_layer = elem.find("./tag[@k='layer']")
                        if found_layer == None:
                            tag = ET.SubElement(elem, "tag")
                            tag.set("k", "layer")
                            tag.set("v", layer_id)
                        elif rule.get("force") == "yes":
                            found_layer.set("v", layer_id)

        # output
        # split = os.path.splitext(self.OSM_file)
        # output_path = '_layers'.join(split)
        # self.tree_osm.write(output_path, encoding="UTF-8")

    def convert_coordinates(self):
        if self.tx == -1:
            left = float(self.root_osm.find("./bounds").get("minlon"))
            right = float(self.root_osm.find("./bounds").get("maxlon"))

            coord = Coordinate()
            coord.set_WGS84(0, left, self.level)
            self.tx = coord.get_tile_coor()[0]

        if self.ty == -1:
            top = float(self.root_osm.find("./bounds").get("maxlat"))
            bottom = float(self.root_osm.find("./bounds").get("minlat"))

            coord = Coordinate()
            coord.set_WGS84(top, 0, self.level)
            self.ty = coord.get_tile_coor()[1]

        for node in self.root_osm.findall("./node"):
            coord = Coordinate()
            lat = float(node.get("lat"))
            lon = float(node.get("lon"))
            coord.set_WGS84(lat, lon, self.level, self.tx, self.ty)

            x, y = coord.get_coor_in_tile()

            node.set("x", x * constants.sampling_factor)
            node.set("y", y * constants.sampling_factor)

    def gen_links(self):
        self.links = {}
        for elem in self.root_osm:
            self.links[get_name(elem)] = elem



    def plot(self):
        self.set_methods()
        self.set_layers()
        self.convert_coordinates()
        self.gen_links()

        elems_at_layer = {}

        for elem in self.root_osm:
            if elem.get("method", None):
                #start = time.clock()
                tlayer = elem.find("./tag[@k='layer']")
                #print "finding time used: %fms" % ((time.clock() - start) * 1000)
                if tlayer is not None:
                    layer_id = float(tlayer.get("v", 0))
                else:
                    layer_id = 0.0

                elems_at_layer.setdefault(layer_id, []).append(elem)

        bg_plotter = methods.get_layer_plot_class(self, self.bg_config)
        bottom_layer = bg_plotter.plot()

        for layer_id in sorted(elems_at_layer):
            layer_plot = methods.get_layer_plot_class(self, self.layer_configs[float(layer_id)])
            layer_plot.plot(elems_at_layer[layer_id], bottom_layer)

            cv2.imwrite("output%d.png" % layer_id, bottom_layer)

        ret = cv2.resize(bottom_layer, constants.tile_size, interpolation=cv2.INTER_AREA)
        #cv2.imshow("result", ret)
        cv2.imwrite("output.png", ret)
        #cv2.waitKey(0)


if __name__ == "__main__":
    plotter = Plot(17, "../Data/map_filtered.osm", "plot_config.xml", 109804, 53570)

    plotter.plot()