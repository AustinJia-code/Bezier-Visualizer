from geometry import *

# Waypoint
class Waypoint (Locatable):
    def __init__ (self, pos: Vec3D, index: int):
        self.pos: Vec3D = pos
        self.index: int = index

    def __lt__(self, other: Vec3D):
        return self.index < other.index

    def get_pos (self) -> Vec3D:
        return self.pos

class WaypointPath:
    def __init__ (self, max_step: float, points: list[Waypoint]):
        self.max_step: float = max_step
        self.points: list[Waypoint] = points

    def deep_copy (self):
        copied_points = [Waypoint (wp.get_pos ().get_copy (), wp.index)
                         for wp in self.points]
        return WaypointPath (self.max_step, copied_points)
    
# Merges waypoint paths
def merge_waypoint_paths (paths: list[WaypointPath]) -> WaypointPath:
    all_points = []
    for path in paths:
        path = path.deep_copy ()
        all_points.extend (path.points)

    # Reassign indices in order
    for i, waypoint in enumerate (all_points):
        waypoint.index = i

    # Use the largest max step
    return WaypointPath (max (path.max_step for path in paths), all_points)