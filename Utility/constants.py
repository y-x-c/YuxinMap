__author__ = 'Orthocenter'

import numpy as np

sampling_factor = 2
padding = 2

tile_l = 256
tile_size = [tile_l, tile_l]
tile_shape = tile_size + [3]
u_tile_size = [_ * sampling_factor + 2 * padding for _ in tile_size]
u_tile_shape = u_tile_size + [3]

tile_size = tuple(tile_size)
tile_shape = tuple(tile_shape)
u_tile_size = tuple(u_tile_size)
u_tile_shape = tuple(u_tile_shape)
tile_dtype = np.uint8

earthRadius = 6378137 # meters
max_level = 18
max_lat = 85.05112877980659
min_lat = -max_lat

def update_tile_size(_tile_size = None):
    global tile_size, tile_shape, u_tile_shape, u_tile_size

    if _tile_size != None:
        tile_size = _tile_size
    tile_shape = tile_size + [3]
    u_tile_size = [_ * sampling_factor for _ in tile_size]
    u_tile_shape = u_tile_size + [3]

    tile_size = tuple(tile_size)
    tile_shape = tuple(tile_shape)
    u_tile_size = tuple(u_tile_size)
    u_tile_shape = tuple(u_tile_shape)