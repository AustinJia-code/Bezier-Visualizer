from .geometry import *

class Obstacle (Locatable):
    def __init__ (self, center: Vec3D, radius: float):
        self.center: Vec3D = center
        self.radius: float = radius
        self.radius_sq: float = radius ** 2
    
    def intersects_point (self, point: Vec3D) -> bool:
        return self.center.dist_sq (point) <= self.radius_sq

    def get_pos (self) -> Vec3D:
        return self.center