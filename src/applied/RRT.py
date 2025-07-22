from geometry import *
from waypoint import *
from obstacle import *
import math
import numpy as np
import random

class RRTNode ():
    def __init__ (self, position: Vec3D):
        self.position:Vec3D = position
        self.parent: RRTNode = None
        self.cost: float = 0.0

class RRT ():
    def __init__ (self, start: Vec3D, goal: Vec3D, step_size: float, 
                  radius: float, bl_bound: Vec3D, tr_bound: Vec3D, 
                  obstacles: list[Obstacle], max_iter: int = 500):
        self.start: RRTNode = RRTNode (start)
        self.goal: RRTNode = RRTNode (goal)
        self.step_size: float = step_size
        self.radius: float = radius
        self.bl_bound: Vec3D = bl_bound
        self.tr_bound: Vec3D = tr_bound
        self.obstacles: list[Obstacle] = obstacles
        self.nodes: list[RRTNode] = [self.start]
        self.max_iter: int = max_iter

    # Get random point in world bounds
    def get_random_point (self) -> Vec3D:
        return Vec3D (random.uniform (self.bl_bound[0], self.tr_bound[0]),
                      random.uniform (self.bl_bound[1], self.tr_bound[1]),
                      random.uniform (self.bl_bound[2], self.tr_bound[2]))

    # Get nearest node to point
    def get_nearest_node (self, point) -> RRTNode:
        return min (self.nodes, key = lambda node: node.position.dist_sq (point))

    # Get neighbourhood for rerouting
    def get_neighbors (self, point) -> list[RRTNode]:
        n = len (self.nodes)
        # Adaptive radius
        r = min (self.radius, self.step_size * math.sqrt ((math.log (n + 1)) / (n + 1)))
        return [node for node in self.nodes if node.position.dist_sq (point) <= r ** 2]
    
    # Rewire with new node
    def rewire (self, new_node: RRTNode, neighbors: list[RRTNode]) -> None:
        for node in neighbors:
            if node == new_node.parent:
                continue
            
            if not self.path_is_obstructed (new_node.position, node.position):
                new_cost = new_node.cost + new_node.position.dist (node.position)
                if new_cost < node.cost:
                    node.parent = new_node
                    node.cost = new_cost

    # Return if point is in object
    def point_is_obstructed (self, point: Vec3D) -> bool:
        for obstacle in self.obstacles:
            if obstacle.intersects_point (point):
                return True
            
        return False
    
    # Return if linear path from a to b is clear
    def path_is_obstructed (self, a: Vec3D, b: Vec3D, step: float = 0.2) -> bool:
        dir = b - a
        len = dir.length ()
        dir.normalize ()

        for t in np.arange (0, len, step):
            p = a + t * dir
            if self.point_is_obstructed (p):
                return True
        
        return self.point_is_obstructed (p)

    # Get real sample position from random
    def steer (self, from_node: RRTNode, to_point: Vec3D) -> Vec3D:
        direction = to_point - from_node.position
        dist = direction.get_copy ().length ()

        if dist == 0:
            return from_node.position.get_copy ()
        
        direction.normalize ()
        dist = min (dist, self.step_size)
        return from_node.position + direction * dist
    
    # Get resulting path
    def get_path (self, node: RRTNode) -> list[Vec3D]:
        path = []
        while node is not None:
            path.append (node.position)
            node = node.parent
        
        # Reverse
        return path[::-1]

    # RRT* Algorithm
    def plan (self) -> list[Vec3D]:
        for _ in range (self.max_iter):
            sample = self.get_random_point ()
            nearest_node = self.get_nearest_node (sample)
            new_pos = self.steer (nearest_node, sample)

            if self.path_is_obstructed (nearest_node.position, new_pos):
                continue

            new_node = RRTNode (new_pos)
            neighbor_nodes = self.get_neighbors (new_pos)

            # Choose parent with lowest cost
            best_parent = nearest_node
            min_cost = nearest_node.cost + nearest_node.position.dist (new_pos)

            for node in neighbor_nodes:
                if not self.path_is_obstructed (node.position, new_pos):
                    cost = node.cost + node.position.dist (new_pos)
                    if cost < min_cost:
                        best_parent = node
                        min_cost = cost

            new_node.parent = best_parent
            new_node.cost = min_cost
            self.nodes.append (new_node)

            self.rewire (new_node, neighbor_nodes)

            # Connect goal if close and collision-free
            if new_node.position.dist (self.goal.position) < self.step_size:
                if not self.path_is_obstructed (new_node.position, self.goal.position):
                    self.goal.parent = new_node
                    self.goal.cost = new_node.cost + new_node.position.dist (self.goal.position)
                    self.nodes.append (self.goal)
                    return self.get_path (self.goal)

        # If goal not connected, return path to nearest
        closest = self.get_nearest_node (self.goal.position)
        return self.get_path (closest)
        
    # Convert RRT path to waypoints
    def get_waypoint_path (self, max_step: float) -> WaypointPath:       
        # build waypoint path
        vec_path = self.plan ()
        waypoints = []
        index = 0
        
        for i in range (len (vec_path) - 1):
            start = vec_path[i]
            end = vec_path[i + 1]
            segment = end - start
            length = segment.length ()
            steps = max (1, int (math.ceil (length / max_step)))
            
            for j in range (steps):
                t = j / steps
                point = start + segment * t
                waypoints.append (Waypoint (point, index))
                index += 1

        # Ensure final point is included
        waypoints.append (Waypoint(vec_path[-1], index))

        return WaypointPath (max_step, waypoints)