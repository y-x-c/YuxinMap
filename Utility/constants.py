__author__ = 'Orthocenter'

import numpy as np

sampling_factor = 2

tile_size = [5068, 5068]
tile_shape = tile_size + [3]
u_tile_size = [_ * 2 for _ in tile_size]
u_tile_shape = u_tile_size + [3]

tile_size = tuple(tile_size)
tile_shape = tuple(tile_shape)
u_tile_size = tuple(u_tile_size)
u_tile_shape = tuple(u_tile_shape)
tile_dtype = np.uint8

def update():
    global tile_size, tile_shape, u_tile_shape, u_tile_size

    tile_shape = tile_size + [3]
    u_tile_size = [_ * 2 for _ in tile_size]
    u_tile_shape = u_tile_size + [3]

    tile_size = tuple(tile_size)
    tile_shape = tuple(tile_shape)
    u_tile_size = tuple(u_tile_size)
    u_tile_shape = tuple(u_tile_shape)