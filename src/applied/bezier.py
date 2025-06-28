from geometry import Vec3D, Locatable

# Waypoint
class Waypoint (Locatable):
    def __init__ (self, pos, index):
        self.pos = pos                                      # type Vec3D
        self.index = index                                  # type num

    def __lt__(self, other):
        return self.index < other.index

    def get_pos (self) -> Vec3D:
        return self.pos


# Cubic Bezier curve
class BezierCurve:
    def __init__ (self, control_points, steps = 20, index_offset = 0):
        self.control_points = control_points                # type: Vec3D[]
        self.steps = steps                                  # type: int
        self.points = self.build_points (index_offset)      # type: Waypoint[]

    def build_points (self, index_offset) -> list[Waypoint]:
        # Build points
        points = []
        for i in range (self.steps + 1):
            t = i / self.steps
            # Cubic Bezier formula
            p = (self.control_points[0] * (1 - t)**3 +
                 self.control_points[1] * 3 * (1 - t)**2 * t +
                 self.control_points[2] * 3 * (1 - t) * t**2 +
                 self.control_points[3] * t**3)
            points.append (Waypoint (p, i + index_offset))

        return points


# C1 continuous Bezier spline
class BezierSpline:
    def __init__ (self, control_points, steps = 20):
        # Format: [p0, c1, c2, p1, c2, p2, c2, p3...]
        # p will be intersected, c2 will always be mirrored over pNext as c1Next
        self.control_points = control_points                # type: Vec3D[]
        self.steps = steps                                  # type: num
        self.points = self.build_spline ()                  # type: Waypoint[]

    def build_spline (self) -> list[Waypoint]:
        cps = self.control_points
        assert len(cps) > 4 and (len(cps) % 2 == 0), "CP count must be 2n and > 4"

        points = []
        segment_count = len(cps) // 3

        for i in range (segment_count):
            # C1 Mirroring
            if (i == 0):
                p0 = cps[0]
                c1 = cps[1]
                c2 = cps[2]
                p1 = cps[3]
            else:
                p0 = cps[i * 2 + 1]
                c2 = cps[i * 2 + 2]
                p1 = cps[i * 2 + 3]

                prev_c2 = cps[i * 2]
                c1 = p0 + (p0 - prev_c2)

            curve = BezierCurve ([p0, c1, c2, p1], self.steps, i * self.steps)

            if points:
                # Drop the first point to avoid duplication at joins
                points.extend (curve.points[1:])
            else:
                points.extend (curve.points)

        return points