__author__ = 'Orthocenter'

import os.path
from Utility.filter import Filter
from Utility.get_name import get_name

try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET

class Level_Filter():
    def __init__(self, OSMPath, rulesPath, level):
        self.OSMPath, self.rulesPath, self.level = OSMPath, rulesPath, level

    def filter_out(self):
        tree_elem = ET.parse(self.OSMPath)
        tree_rules = ET.parse(self.rulesPath)
        root_elem = tree_elem.getroot()
        root_rules = tree_rules.getroot()

        rules = []
        for rules_level in tree_rules.findall("elements[@level]"):
            if int(rules_level.get("level")) <= self.level:
                rules.extend(rules_level.findall("./filter"))

        discards = {}
        links = {}
        queue = []

        filter = Filter()
        for elem in root_elem:
            name = get_name(elem)
            links[name] = elem
            satisfy = False
            for rule in rules:
                if filter.check(elem, rule.find("./")):
                    satisfy = True
                    break
            if not satisfy:
                discards[name] = 1
            else:
                discards[name] = 0
                queue.append(elem)

        while(len(queue)):
            elem = queue[0]
            del queue[0]

            # remove children who don't appear in the osm file
            # add children who have a discard tag now to the queue
            for child in elem.findall("./*[@ref]"):
                name = get_name(child)
                if name not in discards:
                    elem.remove(child)
                elif discards[name]:
                    discards[name] = 0
                    queue.append(links[name])

        # apply discard
        for name in discards:
            if discards[name]:
                root_elem.remove(links[name])

        # output
        split = os.path.splitext(self.OSMPath)
        outputPath = '_filtered'.join(split)

        tree_elem.write(outputPath, encoding = "UTF-8")

if __name__ == '__main__':
    level_filter = Level_Filter("../Data/map.osm", "level_filter_config.xml", 17)
    level_filter.filter_out()