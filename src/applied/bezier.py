from geometry import Vec3D, Locatable
from waypoint import *
from obstacle import *

# Cubic Bezier curve
class BezierCurve:
    def __init__ (self, control_points, max_step = 2, index_offset = 0):
        self.control_points = control_points                # type: Vec3D[]
        self.max_step = max_step                            # type: num
        self.points = self.build_points (index_offset)      # type: Waypoint[]

    # t in [0, 1]
    def vec_at_t (self, t) -> Vec3D:
        cps = self.control_points
        return (cps[0] * (1 - t)**3 +
                cps[1] * 3 * (1 - t)**2 * t +
                cps[2] * 3 * (1 - t) * t**2 +
                cps[3] * t**3)

    def build_points (self, index_offset) -> list[Waypoint]:
        # Recursively subsamples between two ts
        def subsample (t0, t1, v0 = None, v1 = None):
            subsamples = {}
            # Calculate vectors if needed
            if (v0 is None):
                v0 = self.vec_at_t (t0)
                subsamples[t0] = v0
            if (v1 is None):
                v1 = self.vec_at_t (t1)
                subsamples[t1] = v1

            # Base case, dist is good
            if (v0.dist_sq (v1) < self.max_step ** 2):
                return subsamples
            
            # Recurse
            t_mid = (t0 + t1) / 2
            v_mid = self.vec_at_t (t_mid);         
            subsamples[t_mid] = v_mid

            subsamples.update (subsample (t0, t_mid, v0, v_mid))
            subsamples.update (subsample (t_mid, t1, v_mid, v1))

            return subsamples            

        t_to_vec = subsample (0, 1)
        sorted_ts = sorted (t_to_vec.keys ())

        # Build final waypoints
        points = []
        for i, t in enumerate (sorted_ts):
            points.append (Waypoint (t_to_vec[t], index_offset + i))

        return points


# C1 continuous Bezier spline
class BezierSpline:
    def __init__ (self, control_points, max_step = 2):
        # Format: [p0, c1, c2, p1, c2, p2, c2, p3...]
        # p will be intersected, c2 will always be mirrored over pNext as c1Next
        self.control_points = control_points                # type: list[Vec3D]
        self.max_step = max_step                            # type: num
        self.points = self.build_spline ()                  # type: list[Waypoint]
        self.waypoint_path = None                           # type: WaypointPath

    def build_spline (self) -> list[Waypoint]:
        cps = self.control_points
        assert len (cps) > 4 and (len (cps) % 2 == 0), "CP count must be 2n and > 4"

        points = []
        segment_count = len (cps) // 3

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

            curve = BezierCurve ([p0, c1, c2, p1], self.max_step, len (points))

            if points:
                # Drop the first point to avoid duplication at joins
                points.extend (curve.points[1:])
            else:
                points.extend (curve.points)

        return points
    
    def split_by_obstacles (self, obstacles: list[Obstacle]) -> list[WaypointPath]:
        if self.waypoint_path is None:
            self.get_waypoint_path ()
        
        paths = []
        cur_path = WaypointPath (self.max_step, [])
        for waypoint in self.waypoint_path.points:
            obstructed = False
            for obstacle in obstacles:
                # if obstacle intersects and cur path isn't empty, add it to list
                if (obstacle.intersects_point (waypoint.pos)):
                    if (len (cur_path.points) > 0):
                      paths.append (cur_path)
                      cur_path = WaypointPath (self.max_step, [])
                    obstructed = True
                    break
            if not obstructed:
              cur_path.points.append (waypoint)

        if len (cur_path.points) > 0:
            paths.append (cur_path)

        return paths

    def get_waypoint_path (self) -> WaypointPath:
        if self.waypoint_path is None:
            self.waypoint_path = WaypointPath (self.max_step, self.points)
            
        return self.waypoint_path