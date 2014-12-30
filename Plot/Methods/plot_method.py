__author__ = 'Orthocenter'

import numpy as np
from Utility.get_name import get_name
from Utility.color import Color

class Plot_Method():
    def __init__(self, level, links, elem):
        self.level = level
        self.links = links
        self.elem = elem

    def get_np_pts(self):
        pts = []
        for nd in self.elem.findall("./nd"):
            name = get_name(nd)
            node = self.links[name]
            x_y = [int(node.get(cor)) for cor in ('x', 'y')]
            pts.append(x_y)

        np_pts = [np.array(pts, np.int32)]
        np.reshape(np_pts, (-1, 1, 2))

        return np_pts

    def get_color(self):
        color = Color()
        color.setHex(self.elem.get("color"))
        color = color.getBGR()

        return color

    def plot(self, canvas):
        return None
