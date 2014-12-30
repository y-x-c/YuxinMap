__author__ = 'Orthocenter'

import unittest
from coordinate import *
import coordinate

class CoordinateTestCase(unittest.TestCase):
    def test_set_WGS84(self):
        # Tian-an-men Square
        coor0 = Coordinate()
        coor0.set_WGS84(lon = 116.39748, lat = 39.90872, level = 0)
        [tx, ty] = coor0.get_tile_coor()
        self.assertEqual([tx, ty], [0, 0])
        [px, py] = coor0.get_pixel_coor()
        [x, y] = coor0.get_coor_in_tile()
        self.assertEqual([tx * tileSize + x, ty * tileSize + y], [int(px), int(py)])

        coor1 = Coordinate()
        coor1.set_WGS84(lon = 116.39748, lat = 39.90872, level = 1)
        [tx, ty] = coor1.get_tile_coor()
        self.assertEqual([tx, ty], [1, 0])
        [px, py] = coor1.get_pixel_coor()
        [x, y] = coor1.get_coor_in_tile()
        self.assertEqual([tx * tileSize + x, ty * tileSize + y], [int(px), int(py)])

        coor2 = Coordinate()
        coor2.set_WGS84(lon = 116.39748, lat = 39.90872, level = 2)
        [tx, ty] = coor2.get_tile_coor()
        self.assertEqual([tx, ty], [3, 1])
        [px, py] = coor2.get_pixel_coor()
        [x, y] = coor2.get_coor_in_tile()
        self.assertEqual([tx * tileSize + x, ty * tileSize + y], [int(px), int(py)])

        coor16 = Coordinate()
        coor16.set_WGS84(lon = 116.39748, lat = 39.90872, level = 16)
        [tx, ty] = coor16.get_tile_coor()
        self.assertEqual([tx, ty], [53957, 24832])
        [px, py] = coor16.get_pixel_coor()
        [x, y] = coor16.get_coor_in_tile()
        self.assertEqual([tx * tileSize + x, ty * tileSize + y], [int(px), int(py)])

        coor14 = Coordinate()
        coor14.set_WGS84(lon = 116.39748, lat = 39.90872, level = 14)
        [tx, ty] = coor14.get_tile_coor()
        self.assertEqual([tx, ty], [13489, 6208])
        [px, py] = coor14.get_pixel_coor()
        [x, y] = coor14.get_coor_in_tile()
        self.assertEqual([tx * tileSize + x, ty * tileSize + y], [int(px), int(py)])

    def test_set_tile_coor(self):
        coor0 = Coordinate()
        coor0.set_tile_coor(0, 0, 0)
        [lat, lon] = coor0.get_WGS84()
        self.assertAlmostEqual(lat, coordinate.maxLat, 1)
        self.assertAlmostEqual(lon, -180, 1)

        coor1 = Coordinate()
        coor1.set_tile_coor(212, 233, 10)
        [lat, lon] = coor1.get_WGS84()
        coor1_ = Coordinate()
        coor1_.set_WGS84(lat, lon, 10)

        self.assertEqual(coor1.get_WGS84(), coor1_.get_WGS84())
        self.assertEqual(coor1.get_tile_coor(), coor1_.get_tile_coor())
        self.assertEqual(coor1.get_coor_in_tile(), coor1_.get_coor_in_tile())
        self.assertAlmostEqual(coor1.get_pixel_coor()[0], coor1_.get_pixel_coor()[0], 1)
        self.assertAlmostEqual(coor1.get_pixel_coor()[1], coor1_.get_pixel_coor()[1], 1)

    def test_set_coor_in_tile(self):
        coor14 = Coordinate()
        coor14.set_WGS84(lon = 116.39748, lat = 39.90872, level = 14)
        [tx, ty] = coor14.get_tile_coor()
        [x, y] = coor14.get_coor_in_tile()

        coor14_ = Coordinate()
        coor14_.set_coor_in_tile(x, y, 14, tx, ty)
        self.assertAlmostEqual(coor14.get_WGS84()[0], coor14_.get_WGS84()[0], 3)
        self.assertAlmostEqual(coor14.get_WGS84()[1], coor14_.get_WGS84()[1], 3)
        self.assertEqual(coor14.get_tile_coor(), coor14_.get_tile_coor())
        self.assertEqual(coor14.get_coor_in_tile(), coor14_.get_coor_in_tile())
        self.assertAlmostEqual(coor14.get_pixel_coor()[0], coor14_.get_pixel_coor()[0], delta = 1)
        self.assertAlmostEqual(coor14.get_pixel_coor()[1], coor14_.get_pixel_coor()[1], delta = 1)

if __name__ == '__main__':
    unittest.main()
