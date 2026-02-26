from dataclasses import dataclass
import json
from .geometry import Vec3D
from .obstacle import Obstacle, LinearMotion, CircularMotion
from .bezier import BezierSpline
from .waypoint import WaypointPath, merge_waypoint_paths
from .RRT import RRT


@dataclass
class Scene:
    control_points: list[Vec3D]
    obstacles: list[Obstacle]
    bounds: tuple[Vec3D, Vec3D]     # (bottom-left, top-right)
    bezier_max_step: float = 2.0
    rrt_step_size: float = 1.0
    rrt_radius: float = 2.0
    rrt_seed: int | None = None

    @classmethod
    def from_file (cls, path: str) -> 'Scene':
        with open (path) as f:
            d = json.load (f)

        def vec (v): return Vec3D (*v)

        def parse_motion (o):
            m = o.get ('motion')
            if m is None:
                return None
            t = m['type']
            if t == 'linear':
                return LinearMotion (
                    points = [vec (p) for p in m['points']],
                    speed  = m.get ('speed', 1.0),
                    loop   = m.get ('loop',  'bounce'),
                )
            if t == 'circular':
                return CircularMotion (
                    orbit_center  = vec (m['orbit_center']),
                    orbit_radius  = m['orbit_radius'],
                    axis          = m['axis'],
                    speed         = m.get ('speed', 1.0),
                    phase         = m.get ('phase', 0.0),
                )
            raise ValueError (f"Unknown motion type: {t!r}")

        return cls (
            control_points  = [vec (p) for p in d['control_points']],
            obstacles       = [Obstacle (center = vec (o['center']), radius = o['radius'],
                                         motion = parse_motion (o))
                               for o in d['obstacles']],
            bounds          = (vec (d['bounds']['min']), vec (d['bounds']['max'])),
            bezier_max_step = d.get ('bezier_max_step', 2.0),
            rrt_step_size   = d.get ('rrt_step_size',   1.0),
            rrt_radius      = d.get ('rrt_radius',      2.0),
            rrt_seed        = d.get ('rrt_seed',        None),
        )

    @property
    def start (self) -> Vec3D:
        return self.control_points[0]

    def build_path (self) -> WaypointPath:
        bl, tr = self.bounds
        bezier = BezierSpline (self.control_points, max_step = self.bezier_max_step)
        segments = bezier.split_by_obstacles (self.obstacles)

        patches = []
        for i in range (len (segments) - 1):
            start = segments[i].points[-1].pos
            goal  = segments[i + 1].points[0].pos
            print(f"RRT: {start} -> {goal}")
            rrt = RRT (start = start, goal = goal,
                       step_size = self.rrt_step_size,
                       radius = self.rrt_radius,
                       bl_bound = bl, tr_bound = tr,
                       obstacles = self.obstacles,
                       seed = self.rrt_seed)
            patches.append (rrt.get_waypoint_path (max_step = self.bezier_max_step))

        if not patches:
            return segments[0]

        interleaved = [x for pair in zip (segments, patches) for x in pair]
        interleaved.append (segments[-1])
        return merge_waypoint_paths (interleaved)
