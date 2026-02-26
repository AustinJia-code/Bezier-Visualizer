from src.scene import Scene
from src.drone import Drone
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
import sys

# Scene
scene_file = 'scenes/' + (sys.argv[1] if len (sys.argv) > 1 else 'default.json')
scene = Scene.from_file (scene_file)

# Path and Drone
path  = scene.build_path ()
drone = Drone (pos = scene.start, look_r = scene.bezier_max_step * 1.5)
drone.set_path (path)
waypoint = drone.prev_waypoint

# Plot
def create_sphere (center, radius, resolution = 20):
    u = np.linspace (0, 2 * np.pi, resolution)
    v = np.linspace (0, np.pi, resolution)
    x = radius * np.outer (np.cos (u), np.sin (v)) + center.x
    y = radius * np.outer (np.sin (u), np.sin (v)) + center.y
    z = radius * np.outer (np.ones (np.size (u)), np.cos (v)) + center.z
    return x, y, z

def set_axes_equal (ax):
    x_limits = ax.get_xlim3d ()
    y_limits = ax.get_ylim3d ()
    z_limits = ax.get_zlim3d ()
    x_range, x_mid = abs (x_limits[1] - x_limits[0]), np.mean (x_limits)
    y_range, y_mid = abs (y_limits[1] - y_limits[0]), np.mean (y_limits)
    z_range, z_mid = abs (z_limits[1] - z_limits[0]), np.mean (z_limits)
    r = 0.5 * max (x_range, y_range, z_range)
    ax.set_xlim3d ([x_mid - r, x_mid + r])
    ax.set_ylim3d ([y_mid - r, y_mid + r])
    ax.set_zlim3d ([z_mid - r, z_mid + r])

fig = plt.figure ()
ax  = plt.axes (projection = '3d')

xs = [p.pos.x for p in path.points]
ys = [p.pos.y for p in path.points]
zs = [p.pos.z for p in path.points]
ax.scatter (xs, ys, zs, label = 'Path')

for obs in scene.obstacles:
    sx, sy, sz = create_sphere (obs.center, obs.radius)
    ax.plot_surface (sx, sy, sz, alpha = 0.6, color = 'red')
ax.scatter ([], [], [], color = 'red', s = 100, label = 'Obstacle')

drone_point,  = ax.plot ([drone.pos.x], [drone.pos.y], [drone.pos.z], 'bo', label = 'Drone')
target_point, = ax.plot ([waypoint.pos.x], [waypoint.pos.y], [waypoint.pos.z], 'go', label = 'Waypoint')
ax.legend ()

def update (_):
    target = drone.follow_path ()
    drone_point.set_data ([drone.pos.x], [drone.pos.y])
    drone_point.set_3d_properties ([drone.pos.z])
    target_point.set_data ([target.pos.x], [target.pos.y])
    target_point.set_3d_properties ([target.pos.z])
    return drone_point, target_point

ani = FuncAnimation (fig, update, interval = 10, blit = True)
ax.set_box_aspect ([1, 1, 1])
set_axes_equal (ax)
plt.show ()
