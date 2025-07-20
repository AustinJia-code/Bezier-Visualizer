from geometry import *

class Obstacle:
    def __init__ (self, center, radius):
        self.center = center                    # type: Vec3D
        self.radius = radius                    # type: num