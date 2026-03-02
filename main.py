from src.scene import Scene
from src.drone import Drone
from src.obstacle import Obstacle
from src.RRT import RRT
from src.geometry import Vec3D
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
import threading
import queue
import time
import sys

# Scene
scene_file = 'scenes/' + (sys.argv[1] if len (sys.argv) > 1 else 'default.json')
scene = Scene.from_file (scene_file)

# Path and Drone
path  = scene.build_path ()
drone = Drone (pos = scene.start, look_r = scene.bezier_max_step * 1.5)
drone.set_path (path)
waypoint = drone.prev_waypoint

# Replanner
replan_queue: queue.Queue = queue.Queue (maxsize = 1)

# Shared path reference for worker reads, main thread writes
current_path = path

def path_is_threatened (wp_path, from_index, obstacles):
    """Return True if any waypoint at or after from_index intersects an obstacle."""
    for wp in wp_path.points:
        if wp.index >= from_index:
            for obs in obstacles:
                if obs.intersects_point (wp.pos):
                    return True
    return False

def replan_worker ():
    goal = scene.control_points[-1]
    bl, tr = scene.bounds

    while True:
        time.sleep (scene.replan_interval)

        # Snapshot current state so the RRT run is consistent
        start     = drone.pos.get_copy ()
        obs_snap  = [Obstacle (center = obs.center.get_copy (), radius = obs.radius)
                     for obs in scene.obstacles]

        # Only replan if the remaining path is actually threatened
        from_index = drone.prev_waypoint.index if drone.prev_waypoint is not None else 0
        if not path_is_threatened (current_path, from_index, obs_snap):
            continue

        rrt = RRT (start = start, goal = goal,
                   step_size  = scene.rrt_step_size,
                   radius     = scene.rrt_radius,
                   bl_bound   = bl, tr_bound = tr,
                   obstacles  = obs_snap,
                   seed       = scene.rrt_seed)
        new_path = rrt.get_waypoint_path (max_step = scene.bezier_max_step)

        # Discard if main thread hasn't consumed the previous one yet
        try:
            replan_queue.put_nowait (new_path)
        except queue.Full:
            pass

if scene.replan_interval > 0:
    threading.Thread (target = replan_worker, daemon = True).start ()

# Plotting
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

def make_path_scatter (p):
    xs = [wp.pos.x for wp in p.points]
    ys = [wp.pos.y for wp in p.points]
    zs = [wp.pos.z for wp in p.points]
    return ax.scatter (xs, ys, zs, color = 'steelblue', s = 10, label = 'Path')

path_scatter = [make_path_scatter (path)]

obs_surfaces = []
for obs in scene.obstacles:
    sx, sy, sz = create_sphere (obs.center, obs.radius)
    obs_surfaces.append (ax.plot_surface (sx, sy, sz, alpha = 0.6, color = 'red'))
ax.scatter ([], [], [], color = 'red', s = 100, label = 'Obstacle')

drone_point,  = ax.plot ([drone.pos.x], [drone.pos.y], [drone.pos.z], 'bo', ms = 8, label = 'Drone')
target_point, = ax.plot ([waypoint.pos.x], [waypoint.pos.y], [waypoint.pos.z], 'go', ms = 6, label = 'Waypoint')
ax.legend ()

t0 = time.time ()

# Reset
def reset_scene (event = None):
    if event is not None and event.key != 'r':
        return

    global t0, current_path
    t0 = time.time ()

    # Reset obstacles
    for obs in scene.obstacles:
        obs.update (0)

    # Re-build initial path
    new_path = scene.build_path ()
    current_path = new_path

    # Reset drone
    drone.pos = scene.start.get_copy ()
    drone.vel = Vec3D (0, 0, 0)
    drone.accel = Vec3D (0, 0, 0)
    drone.prev_time = None
    drone.set_path (new_path)

    # Update visuals
    path_scatter[0].remove ()
    path_scatter[0] = make_path_scatter (new_path)

    for i, obs in enumerate (scene.obstacles):
        obs_surfaces[i].remove ()
        sx, sy, sz = create_sphere (obs.center, obs.radius)
        obs_surfaces[i] = ax.plot_surface (sx, sy, sz, alpha = 0.6, color = 'red')

    # Clear replan queue
    while not replan_queue.empty ():
        try:
            replan_queue.get_nowait ()
        except queue.Empty:
            break

fig.canvas.mpl_connect ('key_press_event', reset_scene)

# Animation
def update (_):
    t = time.time () - t0

    # Move obstacles
    for i, obs in enumerate (scene.obstacles):
        if obs.motion is not None:
            obs.update (t)
            obs_surfaces[i].remove ()
            sx, sy, sz = create_sphere (obs.center, obs.radius)
            obs_surfaces[i] = ax.plot_surface (sx, sy, sz, alpha = 0.6, color = 'red')

    # Swap to freshly planned path when available
    try:
        new_path = replan_queue.get_nowait ()
        global current_path
        current_path = new_path
        drone.update_path (new_path)
        path_scatter[0].remove ()
        path_scatter[0] = make_path_scatter (new_path)
    except queue.Empty:
        pass

    target = drone.follow_path ()
    drone_point.set_data ([drone.pos.x], [drone.pos.y])
    drone_point.set_3d_properties ([drone.pos.z])
    target_point.set_data ([target.pos.x], [target.pos.y])
    target_point.set_3d_properties ([target.pos.z])

ani = FuncAnimation (fig, update, interval = 10)
ax.set_box_aspect ([1, 1, 1])
set_axes_equal (ax)
plt.show ()
