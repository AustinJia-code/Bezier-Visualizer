from geometry import Vec3D, KDTree, Locatable
from bezier import BezierSpline
from waypoint import *
import time

class PID:
    def __init__ (self, kp, ki, kd, output_limits = (0, 1)):
        self.kp = kp
        self.ki = ki
        self.kd = kd

        self.output_limits = output_limits

        self.prev_error = 0
        self.integral = 0
        self.prev_time = None

    def reset (self):
        self.prev_error = 0
        self.integral = 0
        self.prev_time = None

    def update (self, error, current_time):
        dt = current_time - self.prev_time if self.prev_time is not None else 0
        de = error - self.prev_error if self.prev_time is not None else 0

        if dt > 0:
            self.integral += error * dt
            derivative = de / dt
        else:
            derivative = 0

        output = self.kp * error + self.ki * self.integral + self.kd * derivative

        min_out, max_out = self.output_limits
        if min_out is not None:
            output = max (min_out, output)
        if max_out is not None:
            output = min (max_out, output)

        self.prev_error = error
        self.prev_time = current_time

        return output


class ChainedPID3D:
    def __init__ (self, pos_gains, vel_gains, pos_limits, vel_limits):
        # pos_gains, vel_gains: tuples (Kp, Ki, Kd)
        # pos_limits, vel_limits: tuples (min, max) output limits per PID
        self.pid_pos = [PID (*pos_gains, output_limits = pos_limits) for _ in range(3)]
        self.pid_vel = [PID (*vel_gains, output_limits = vel_limits) for _ in range(3)]

    def reset (self):
        for p, v in zip (self.pid_pos, self.pid_vel):
            p.reset ()
            v.reset ()

    def update (self, position, velocity, target_position, current_time):
        target_velocity = []
        for i in range (3):
            pos_error = target_position[i] - position[i]
            v_cmd = self.pid_pos [i].update (pos_error, current_time)
            target_velocity.append (v_cmd)

        acceleration_cmd = []
        for i in range (3):
            vel_error = target_velocity[i] - velocity[i]
            a_cmd = self.pid_vel[i].update (vel_error, current_time)
            acceleration_cmd.append (a_cmd)

        return acceleration_cmd


class Drone (Locatable):
    def __init__ (self, pos, min_vel = (-2, -2, -1), max_vel = (2, 2, 1),
                  min_accel = (-2, -2, -1), max_accel = (2, 2, 1), look_r = 1.5):
        self.pos = pos                  # type Vec3D
        self.min_vel = min_vel          # type tuple(3)
        self.max_vel = max_vel          # type tuple(3)
        self.min_accel = min_accel      # type tuple(3)
        self.max_accel = max_accel      # type tuple(3)
        self.look_r = look_r            # type num

        self.vel = Vec3D (0, 0, 0)
        self.accel = Vec3D (0, 0, 0)

        self.prev_time = None
      
    def get_pos (self) -> Vec3D:
        return self.pos

    def set_path (self, waypoint_path, pos_gains = (2, 0, 0.5), vel_gains = (4, 0, 0.2),
                  pos_limits = None, vel_limits = (-10, 10)):
        waypoints = waypoint_path.points
        assert self.look_r >= waypoint_path.max_step, "Lookahead radius must be greater than path max step"

        if pos_limits is None:
            pos_limits = (-self.look_r, self.look_r)

        # Build KDTree
        self.waypoint_tree = KDTree (waypoints)
        self.prev_waypoint = waypoints [0]

        self.pid_3d = ChainedPID3D (
            pos_gains = pos_gains,
            vel_gains = vel_gains,
            pos_limits = pos_limits,
            vel_limits = vel_limits,
        )

    # Must call set_path beforehand, returns the target point
    def follow_path (self) -> Vec3D:
        if (self.pid_3d is None):
            raise ValueError ("Path not set")
      
        # Search waypoint_tree for largest intersected waypoint index
        # PID to that
        # If no target found, go to prev known waypoint (defaults to cp0)
        targets = self.waypoint_tree.search_radius (self.pos, self.look_r)
        targets.sort (reverse = True)
        if (len (targets) > 0):
            target = targets[0]
            self.prev_waypoint = target
        else:
            target = self.prev_waypoint

        current_time = time.time ()
        accel_cmd = self.pid_3d.update (self.pos.to_tuple (), self.vel.to_tuple (),
                                        target.pos.to_tuple(), current_time)

        self.kinematics (Vec3D (*accel_cmd), current_time)

        return target

    def kinematics (self, accel, current_time):
        if (self.prev_time is None):
            self.prev_time = current_time

        dt = current_time - self.prev_time
        self.pos += self.vel * dt + 0.5 * self.accel * dt ** 2
        self.vel += self.accel * dt
        self.accel = accel

        self.prev_time = current_time