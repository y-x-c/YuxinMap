__author__ = 'Orthocenter'

from OSM_Interface.level_filter import Level_Filter
from Plot.plot import Plot
import time

level = 17

level_filter = Level_Filter("Data/map.osm", "OSM_Interface/level_filter_config.xml", level) # Century Park
#level_filter = Level_Filter("Data/map.osm", "OSM_Interface/level_filter_config.xml", 3) # zhangjiang
#level_filter = Level_Filter("Data/6,3,3.osm", "OSM_Interface/level_filter_config.xml", 3) # level 3
print "Filtering..."
level_filter.filter_out()
#109780, 53550
plot = Plot(level, "Data/map_filtered.osm", "Plot/plot_config.xml", -1, -1)# Century Park
print "Plotting..."
start = time.clock()
#plot = Plot(3, "Data/6,3,3_coastline.osm", "Plot/plot_config.xml", 6, 3)# zhangjiang

plot.plot()

print "Plotted"
print "Time used:", time.clock() - start