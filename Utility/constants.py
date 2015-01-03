__author__ = 'Orthocenter'

import numpy as np

sampling_factor = 2
retina_factor = 2
padding = 2

tile_l = 256
tile_size = [tile_l, tile_l]
tile_shape = tile_size + [3]
retina_tile_size = [tile_l * retina_factor, tile_l * retina_factor]
retina_tile_shape = retina_tile_size + [3]
u_tile_size = [_ * sampling_factor * retina_factor + 2 * padding for _ in tile_size]
u_tile_shape = u_tile_size + [3]

tile_size = tuple(tile_size)
tile_shape = tuple(tile_shape)
retina_tile_size = tuple(retina_tile_size)
retina_tile_shape = tuple(retina_tile_shape)
u_tile_size = tuple(u_tile_size)
u_tile_shape = tuple(u_tile_shape)
tile_dtype = np.uint8

earthRadius = 6378137 # meters
max_level = 18
max_lat = 85.05112877980659
min_lat = -max_lat