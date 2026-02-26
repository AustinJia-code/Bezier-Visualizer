from .geometry import Vec3D, Locatable
from abc import ABC, abstractmethod
import math
import numpy as np


class Motion (ABC):
    @abstractmethod
    def position (self, t: float) -> Vec3D: ...


class LinearMotion (Motion):
    """Moves along a polyline at a fixed speed, bouncing or cycling."""

    def __init__ (self, points: list[Vec3D], speed: float = 1.0, loop: str = 'bounce'):
        self.points = points
        self.speed  = speed
        self.loop   = loop  # 'bounce' | 'cycle'

        self.segments = list (zip (points, points[1:]))
        self.lengths  = [a.dist (b) for a, b in self.segments]
        self.total    = sum (self.lengths)

    def position (self, t: float) -> Vec3D:
        if self.loop == 'bounce':
            period = 2 * self.total / self.speed
            d = (t % period) * self.speed
            if d > self.total:
                d = 2 * self.total - d
        else:  # cycle
            d = (t * self.speed) % self.total

        for (a, b), length in zip (self.segments, self.lengths):
            if d <= length:
                alpha = d / length if length > 0 else 0
                return a + (b - a) * alpha
            d -= length

        return self.points[-1]


class CircularMotion (Motion):
    """Orbits around a center point on the plane perpendicular to `axis`."""

    def __init__ (self, orbit_center: Vec3D, orbit_radius: float,
                  axis: list[float], speed: float = 1.0, phase: float = 0.0):
        self.orbit_center  = orbit_center
        self.orbit_radius  = orbit_radius
        self.speed         = speed
        self.phase         = phase

        # Build orthonormal basis for the rotation plane
        ax  = np.array (axis, dtype = float)
        ax /= np.linalg.norm (ax)
        ref = np.array ([1.0, 0.0, 0.0]) if abs (ax[0]) < 0.9 else np.array ([0.0, 1.0, 0.0])
        u   = ref - np.dot (ref, ax) * ax
        u  /= np.linalg.norm (u)
        self.u = u
        self.v = np.cross (ax, u)

    def position (self, t: float) -> Vec3D:
        angle  = t * self.speed + self.phase
        offset = self.orbit_radius * (math.cos (angle) * self.u + math.sin (angle) * self.v)
        return Vec3D (
            self.orbit_center.x + float (offset[0]),
            self.orbit_center.y + float (offset[1]),
            self.orbit_center.z + float (offset[2]),
        )


class Obstacle (Locatable):
    def __init__ (self, center: Vec3D, radius: float, motion: Motion | None = None):
        self.center:    Vec3D         = center
        self.radius:    float         = radius
        self.radius_sq: float         = radius ** 2
        self.motion:    Motion | None = motion

    def update (self, t: float) -> None:
        if self.motion is not None:
            self.center = self.motion.position (t)

    def intersects_point (self, point: Vec3D) -> bool:
        return self.center.dist_sq (point) <= self.radius_sq

    def get_pos (self) -> Vec3D:
        return self.center
