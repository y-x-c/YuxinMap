__author__ = 'Orthocenter'

from Plotter.plot import Plot
import Utility.constants as constants
import os
import sys
import time

preprocessor_path = "Preprocessor/Preprocessor"
osm_path = sys.argv[1]
osm_path_abs = os.path.abspath(osm_path)
level_filter_config_path = "Configs/level_filter_config.xml"
level_filter_config_path_abs = os.path.abspath(level_filter_config_path)
plot_config_path = "Configs/plot_config.xml"
plot_config_path_abs = os.path.abspath(plot_config_path)
dt_files_path = "TmpDt/"
dt_files_path_abs = os.path.abspath(dt_files_path) + '/'
dt_list_path = dt_files_path_abs + "dt_list"
tiles_path = "Tiles/"
tiles_path_abs = os.path.abspath(tiles_path) + '/'
showNames_path = "ShowNames/ShowNames"
showNames_path_abs = os.path.abspath(showNames_path)
showNames_outputPath = "Names/"
showNames_outputPath_abs = os.path.abspath(showNames_outputPath) + '/'

min_level = str(5)
max_level = str(17)

print "Start preprocessing."
st_time = time.time()
cmd = " ".join([preprocessor_path, osm_path_abs, level_filter_config_path_abs, plot_config_path_abs, dt_files_path_abs,
                min_level, max_level])
os.system(cmd)
print "Preprocessing time used: %f s" % (time.time() - st_time)

print "Start plotting."
st_time = time.time()
dt_files = open(dt_list_path)
for dt_file in dt_files.readlines():
    tile_coord = os.path.basename(dt_file).split(".")[0]
    dt_file_path = dt_file.rstrip("\n")
    plotter = Plot(dt_file_path)
    plotter.plot(tiles_path_abs + tile_coord + ".png", tiles_path_abs + tile_coord + "_r.png")

    os.system(" ".join(["rm", "-f", dt_file_path]))
    os.system(" ".join(["rm", "-f", dt_files_path_abs + tile_coord + '_bg.png']))

print "Finished plotting."
print "Plotting time used: %f s" % (time.time() - st_time)

print "Generating name files."
st_time = time.time()
cmd = " ".join([showNames_path_abs, osm_path_abs, level_filter_config_path_abs, showNames_outputPath_abs, min_level, max_level])
os.system(cmd)
print "Generating time used: %f s" % (time.time() - st_time)
