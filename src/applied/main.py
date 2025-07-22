from bezier import BezierSpline
from geometry import Vec3D
from drone import Drone
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from waypoint import *
from RRT import *
import time

def create_sphere(center, radius, resolution = 20):
    """Create sphere coordinates for plotting"""
    u = np.linspace(0, 2 * np.pi, resolution)
    v = np.linspace(0, np.pi, resolution)
    x = radius * np.outer(np.cos(u), np.sin(v)) + center.x
    y = radius * np.outer(np.sin(u), np.sin(v)) + center.y
    z = radius * np.outer(np.ones(np.size(u)), np.cos(v)) + center.z
    return x, y, z

def set_axes_equal (ax):
    '''Set 3D plot axes to equal scale.'''
    x_limits = ax.get_xlim3d()
    y_limits = ax.get_ylim3d()
    z_limits = ax.get_zlim3d()

    x_range = abs(x_limits[1] - x_limits[0])
    x_middle = np.mean(x_limits)
    y_range = abs(y_limits[1] - y_limits[0])
    y_middle = np.mean(y_limits)
    z_range = abs(z_limits[1] - z_limits[0])
    z_middle = np.mean(z_limits)

    plot_radius = 0.5 * max([x_range, y_range, z_range])

    ax.set_xlim3d([x_middle - plot_radius, x_middle + plot_radius])
    ax.set_ylim3d([y_middle - plot_radius, y_middle + plot_radius])
    ax.set_zlim3d([z_middle - plot_radius, z_middle + plot_radius])

obstacles = [Obstacle (center = Vec3D (4, 4, 4), radius = 2)]

# Build bezier path
points = []
points.append (Vec3D (-5, -5, -5))
points.append (Vec3D (-3, 0, 0))
points.append (Vec3D (3, 0, 0))
points.append (Vec3D (5, 5, 5))
points.append (Vec3D (10, 5, 5))
points.append (Vec3D (12, 7, 7))

bezier = BezierSpline (points, max_step = 2)
paths: list[WaypointPath] = bezier.split_by_obstacles (obstacles)

# Build RRT* path
# rrt = RRT (start = Vec3D (-5, -5, -5), goal = Vec3D (12, 7, 7), step_size = 0.5, 
#                   radius = 2, bl_bound = Vec3D (-5, -5, -5), tr_bound =  Vec3D (12, 7, 7), 
#                   obstacles = obstacles)
# path = rrt.get_waypoint_path (max_step = 2)

patches: list[WaypointPath] = []
for i in range (len (paths) - 1):
    start = paths[i].points[-1].pos
    goal = paths[i + 1].points[0].pos
    print (str (start))
    rrt = RRT (start = start, goal = goal, step_size = 0.5, 
                  radius = 2, bl_bound = Vec3D (-5, -5, -5), tr_bound =  Vec3D (12, 7, 7), 
                  obstacles = obstacles)
    patches.append (rrt.get_waypoint_path (max_step = 2))

if len (patches) == 0:
    path = paths[0]
else:
    interleaved = [x for pair in zip (paths, patches) for x in pair]
    interleaved.append (paths[-1])
    path = merge_waypoint_paths (interleaved)

# Init drone
drone = Drone (pos = Vec3D (-5, -5, -5), look_r = path.max_step * 1.5)
drone.set_path (path)
waypoint = drone.prev_waypoint

# Draw
fig = plt.figure ()
ax = plt.axes (projection='3d')

# Extract coordinates
xs = [p.pos.x for p in path.points]
ys = [p.pos.y for p in path.points]
zs = [p.pos.z for p in path.points]
ax.scatter (xs, ys, zs, label = 'Path')

# Bezier Control points
# cx = [p.x for p in points]
# cy = [p.y for p in points]
# cz = [p.z for p in points]
# ax.scatter(cx, cy, cz, color = 'red', label = 'Control Points')

# Draw obstacles
for i, obstacle in enumerate(obstacles):
    x_sphere, y_sphere, z_sphere = create_sphere(obstacle.center, obstacle.radius)
    # Only label the first obstacle for legend
    label = 'Obstacle' if i == 0 else None
    ax.plot_surface (x_sphere, y_sphere, z_sphere, alpha = 0.6, color='red')

# Add a dummy point for legend entry
ax.scatter ([], [], [], color='red', s = 100, label = 'Obstacle')

# Drone
drone_point, = ax.plot([drone.pos.x], [drone.pos.y], [drone.pos.z], 'bo', label = 'Drone')
target_point, = ax.plot([waypoint.pos.x], [waypoint.pos.y], [waypoint.pos.z], 'go', label = 'Waypoint')

ax.legend()

def update (frame):
    target = drone.follow_path ()
    drone_point.set_data ([drone.pos.x], [drone.pos.y])
    drone_point.set_3d_properties ([drone.pos.z])

    target_point.set_data ([target.pos.x], [target.pos.y])
    target_point.set_3d_properties ([target.pos.z])

    return drone_point, target_point

# Create animation, interval in ms (10 ms = 100 fps)
ani = FuncAnimation (fig, update, interval = 10, blit = True)
ax.set_box_aspect([1,1,1])  # Requires Matplotlib ≥ 3.3
set_axes_equal (ax)
plt.show ()