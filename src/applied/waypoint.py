from geometry import *

# Waypoint
class Waypoint (Locatable):
    def __init__ (self, pos, index):
        self.pos = pos                                      # type: Vec3D
        self.index = index                                  # type: num

    def __lt__(self, other):
        return self.index < other.index

    def get_pos (self) -> Vec3D:
        return self.pos

class WaypointPath:
    def __init__ (self, max_step, points):
        self.max_step = max_step                            # type: num
        self.points = points                                # type: list[Waypoint]

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
    return WaypointPath (min (path.max_step for path in paths), all_points)