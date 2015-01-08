__author__ = 'Orthocenter'

from Plot.plot import Plot
import Utility.constants as constants
import os
import sys
import time

filter_path = "Filter/Filter"
osm_path = sys.argv[1]
osm_path_abs = os.path.abspath(osm_path)
level_filter_config_path = "Filter/level_filter_config.xml"
level_filter_config_path_abs = os.path.abspath(level_filter_config_path)
plot_config_path = "Filter/plot_config.xml"
plot_config_path_abs = os.path.abspath(plot_config_path)
dt_files_path = "TmpDt/"
dt_files_path_abs = os.path.abspath(dt_files_path) + '/'
dt_list_path = dt_files_path_abs + "dt_list"
tiles_path = "Tiles/"
tiles_path_abs = os.path.abspath(tiles_path) + '/'

min_level = str(15)
max_level = str(15)

print "Start filtering."
st_time = time.time()
cmd = " ".join([filter_path, osm_path_abs, level_filter_config_path_abs, plot_config_path_abs, dt_files_path_abs,
                min_level, max_level])
os.system(cmd)
print "Filtering time used: %f s" % (time.time() - st_time)

print "Start plotting."
st_time = time.time()
dt_files = open(dt_list_path)
for dt_file in dt_files.readlines():
    tile_coord = os.path.basename(dt_file).split(".")[0]
    dt_file_path = dt_file.rstrip("\n")
    plotter = Plot(dt_file_path)
    plotter.plot(tiles_path_abs + tile_coord + ".png", tiles_path_abs + tile_coord + "_r.png")

    os.system(" ".join(["rm", "-f", dt_file_path]))

print "Finished plotting."
print "Plotting time used: %f s" % (time.time() - st_time)
