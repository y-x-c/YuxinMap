__author__ = 'Orthocenter'

from math import *

earthRadius = 6378137 # meters
tileSize = 256 # px

maxLevel = 18

maxLat = 85.05112877980659
minLat = -maxLat

def check_tile_coor(tx, ty, level):
    if level > maxLevel or level < 0:
        raise CoordinateError("Invalid level.")
    n = 2 ** level
    if tx < 0 or tx >= n:
        raise CoordinateError("Invalid tile X.")
    if ty < 0 or ty >= n:
        raise CoordinateError("Invalid tile Y.")

def check_pixel_coor(px, py, level):
    if level > maxLevel or level < 0:
        raise CoordinateError("Invalid level.")
    n = 2 ** level
    size = n * 256
    if px < 0 or px >= size:
        raise CoordinateError("Invalid pixel X.")
    if py < 0 or py >= size:
        raise CoordinateError("Invalid pixel Y.")

def check_WGS84(lat, lon, level):
    if level > maxLevel or level < 0:
        raise CoordinateError("Invalid level.")
    if lat > maxLat or lat < minLat:
        raise CoordinateError("Invalid latitude.")
    if lon > 180 or lon < -180:
        raise CoordinateError("Invalid longitude.")

def check_coor_in_tile(x, y, level, tx, ty):
    if level > maxLevel:
        raise CoordinateError("Invalid level.")

    check_tile_coor(tx, ty, level)

    if x >= tileSize or x < 0:
        raise CoordinateError("Invalid X.")
    if y >= tileSize or y < 0:
        raise CoordinateError("Invalid Y.")


class CoordinateError(Exception):
    def __init__(self, arg):
        self.args = arg

class Coordinate:
    def __init__(self):
        pass

    def set_WGS84(self, lat, lon, level, tx = None, ty = None):
        check_WGS84(lat, lon, level)

        self.lat = lat;
        self.lon = lon;
        self.level = level;

        sinLat = sin(lat * pi / 180)
        n = 2 ** level
        self.px = (lon + 180.0) / (360.0) * tileSize * n
        self.py = (0.5 - log((1 + sinLat) / (1 - sinLat), e) / (4 * pi)) * tileSize * n

        if tx == None:
            self.tx = int(self.px / tileSize)
        else:
            self.tx = tx

        if ty == None:
            self.ty = int(self.py / tileSize)
        else:
            self.ty = ty

        self.x = int(self.px - self.tx * tileSize)
        self.y = int(self.py - self.ty * tileSize)

    def set_tile_coor(self, tx, ty, level):
        check_tile_coor(tx, ty, level)

        self.tx = tx
        self.ty = ty
        self.level = level

        self.x = 0
        self.y = 0

        n = 2 ** level
        self.px = float(tx * tileSize)
        self.py = float(ty * tileSize)

        self.lon = float(tx) / n * 360.0 - 180.0
        self.lat = atan(sinh(pi * (1 - 2.0 * ty / n))) * 180 / pi

    def set_coor_in_tile(self, x, y, level, tx, ty):
        check_coor_in_tile(x, y, level, tx, ty)

        self.tx = tx
        self.ty = ty
        self.x = x
        self.y = y
        self.level = level

        n = 2 ** level
        self.px = float(tx * tileSize + x)
        self.py = float(ty * tileSize + y)

        self.lon = self.px / tileSize / n * 360.0 - 180.0
        self.lat = atan(sinh(pi * (1 - 2.0 * self.py / tileSize / n))) * 180 / pi

    def get_tile_coor(self):
        return [self.tx, self.ty]

    def get_pixel_coor(self):
        return [self.px, self.py]

    def get_coor_in_tile(self):
        return [self.x, self.y]

    def get_WGS84(self):
        return [self.lat, self.lon]

if __name__ == '__main__':
    # cor = Coordinate()
    # cor.set_WGS84(31.19673, 121.58762, 17)
    # print cor.get_tile_coor()

    left_tx = 6
    top_ty = 3
    cor = Coordinate()
    cor.set_tile_coor(left_tx, top_ty, 3)
    a = cor.get_WGS84()
    print a
    cor.set_tile_coor(left_tx + 1, top_ty + 1, 3)
    b = cor.get_WGS84()
    print b

    print ",".join(str(_) for _ in [a[1], b[0], b[1], a[0]])