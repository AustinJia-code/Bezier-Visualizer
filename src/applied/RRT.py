from geometry import *
from waypoint import *
from obstacle import *
import random

class RRTNode ():
    def __init__ (self, position):
        self.position = position                        # type: Vec3D
        self.parent = None                              # type: RRTNode
        self.cost = 0.0                                 # type: num

class RRT ():
    def __init__ (self, start, goal, step_size, radius, bl_bound, tr_bound, obstacles):
        self.start = start                              # type: Vec3D
        self.goal = goal                                # type: Vec3D
        self.step_size = step_size                      # type: num
        self.radius = radius                            # type: num
        self.bl_bound = bl_bound                        # type: Vec3D
        self.tr_bound = tr_bound                        # type: Vec3D
        self.obstacles = obstacles                      # type: list[Obstacle]
        self.nodes = [self.start]                       # type: list[RRTNode]

        # Final output
        self.waypoint_path = self.get_waypoint_path ()  # type: WaypointPath

    # Get random point in world bounds
    def get_random_point (self):
        return Vec3D (random.uniform (self.bl_bound[0], self.tr_bound[0]),
                      random.uniform (self.bl_bound[1], self.tr_bound[1]),
                      random.uniform (self.bl_bound[2], self.tr_bound[2]))

    # Get nearest node to point
    def nearest (self, point):
        return min (self.nodes, key = lambda node: node.position.dist_sq (point))

    # RRT* Algorithm
    def plan (self):
        pass
        

    # Convert RRT path to waypoints
    def get_waypoint_path (self, max_waypoint_step) -> WaypointPath:
        if self.waypoint_path is not None:
            return self.waypoint_path
        
        # build waypoint path
        self.plan ()
        
        # Build points based on max waypoint step


        return WaypointPath ()