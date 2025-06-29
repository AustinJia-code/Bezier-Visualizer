from bezier import BezierSpline
from geometry import Vec3D
from drone import Drone
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import time

# Build path
points = []
points.append (Vec3D (-5, -5, -5))
points.append (Vec3D (-3, 0, 0))
points.append (Vec3D (3, 0, 0))
points.append (Vec3D (5, 5, 5))
points.append (Vec3D (10, 5, 5))
points.append (Vec3D (12, 7, 7))

path = BezierSpline (points, max_step = 2)

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
ax.scatter (xs, ys, zs, label='Bezier Spline')

# Control points
cx = [p.x for p in points]
cy = [p.y for p in points]
cz = [p.z for p in points]
ax.scatter(cx, cy, cz, color='red', label='Control Points')

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
plt.show()