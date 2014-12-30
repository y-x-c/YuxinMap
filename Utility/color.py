__author__ = 'Orthocenter'



class Color:
    def __init__(self):
        self.b = self.g = self.r = 0

    def setHex(self, c):
        if c[0] is "#":
            c = c[1: ]
        self.r, self.g, self.b = [int(c[i * 2: (i + 1) * 2], base = 16) for i in xrange(0, 3)]

    def setBGR(self, b, g, r):
        self.b, self.g, self.r = b, g, r

    def setRGB(self, r, g, b):
        self.r, self.g, self.b = r, g, b

    def getBGR(self):
        return self.b, self.g, self.r

    def getHex(self):
        rgb = (self.r, self.g, self.b)
        return '#' + "".join([hex(c)[2: 4].upper() for c in rgb])
