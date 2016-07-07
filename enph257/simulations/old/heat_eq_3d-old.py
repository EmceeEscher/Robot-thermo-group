#!/bin/python
"""simulate.py
An attempt at a 3-dimensional heat flow simulation in cylindrical-polar
coordinates
"""
from __future__ import print_function, division
from os import path
from math import pi
from matplotlib import pyplot as plt
from matplotlib import animation, colors, cm
from mpl_toolkits.mplot3d import axes3d as p3
import numpy as np

# constants
# radial dimensions (meters)
MIN_R = 0.0
MAX_R = 0.1
DIM_R = 10
# angular dimensions (radians)
MIN_A = 0.0
MAX_A = 2 * pi
DIM_A = 10
# axial dimensions (meters)
MIN_Z = 0.0
MAX_Z = 0.5
DIM_Z = 10
# thermodynamical parameters
T_ATM = 300.0
T_SRC = 1000.0
ALPHA = 1.0
# finite differencing
NUM_STEPS = 1000
TIME_STEP = 1.0  # seconds
# points
R_ARRAY = np.linspace(MIN_R, MAX_R, DIM_R)
A_ARRAY = np.linspace(MIN_A, MAX_A, DIM_A, endpoint=False)
Z_ARRAY = np.linspace(MIN_Z, MAX_Z, DIM_Z)


# functions
def explicit_step(point_to_temp_map, r_array, a_array, z_array, dt,
                  alpha, t_src):
    """Step forward in the cylindrical polar heat equation
        u_t = alpha * laplacian(u)
    :param point_to_temp_map: map from cylindrical point indices to temperature
        (idx_r, idx_a, idx_z) -> temp,
    :param alpha: constant of proportionality in heat equation
    :param t_src: source temperature
    :param r_array: array of r values (acts as a map idx_r -> r)
    """
    max_idx_r = len(r_array) - 1
    max_idx_a = len(a_array) - 1
    max_idx_z = len(z_array) - 1
    dr = max_idx_r / (r_array[-1] - r_array[0])
    da = max_idx_a / (a_array[-1] - a_array[0])
    dz = max_idx_z / (z_array[-1] - z_array[0])
    dim_a = max_idx_a
    next_point_to_temp_map = dict()
    for point, temp in point_to_temp_map.items():
        idx_r, idx_a, idx_z = point
        r = r_array[idx_r]
        next_temp = temp
        # radial part
        if 0 < idx_r < max_idx_r:
            t1 = point_to_temp_map[(idx_r+1, idx_a, idx_z)]
            t0 = temp
            t_1 = point_to_temp_map[(idx_r-1, idx_a, idx_z)]
        else:  # boundary conditions
            if 0 == idx_r:
                t1 = point_to_temp_map[(idx_r+1, idx_a, idx_z)]
                t0 = t1
                t_1 = point_to_temp_map[
                    (idx_r, (idx_a+dim_a/2) % dim_a, idx_z)]
            else:
                t_1 = point_to_temp_map[(idx_r-1, idx_a, idx_z)]
                t0 = temp
                t1 = t0
        next_temp += alpha*dt/dr**2 * (t1 - 2*t0 + t_1)
        if r != 0:
            next_temp += alpha*dt/(r*dr) * (t1 - t0)
        # angular part
        if 0 < idx_a < max_idx_a:
            t1 = point_to_temp_map[(idx_r, idx_a+1, idx_z)]
            t0 = temp
            t_1 = point_to_temp_map[(idx_r, idx_a-1, idx_z)]
        else:  # boundary conditions
            if 0 == idx_a:
                t1 = point_to_temp_map[(idx_r, idx_a+1, idx_z)]
                t0 = temp
                t_1 = point_to_temp_map[(idx_r, max_idx_a, idx_z)]
            else:
                t_1 = point_to_temp_map[(idx_r, idx_a-1, idx_z)]
                t0 = temp
                t1 = point_to_temp_map[(idx_r, 0, idx_z)]
        if r != 0:
            next_temp += alpha*dt/(r*da)**2 * (t1 - 2*t0 + t_1)
        # axial part
        if 0 < idx_z < max_idx_z:
            t1 = point_to_temp_map[(idx_r, idx_a, idx_z+1)]
            t0 = temp
            t_1 = point_to_temp_map[(idx_r, idx_a, idx_z-1)]
        else:  # boundary conditions
            if 0 == idx_z:
                next_temp = t_src
                next_point_to_temp_map[point] = next_temp
                continue
            else:
                t_1 = point_to_temp_map[(idx_r, idx_a, idx_z-1)]
                t0 = temp
                t1 = t0
        next_temp += alpha*dt/dz**2 * (t1 - 2*t0 + t_1)
        # add point to map
        next_point_to_temp_map[point] = next_temp
    return next_point_to_temp_map


def simulation(
        time_step, r_array, a_array, z_array, t_src, t_atm, alpha,
        finite_step_method=explicit_step,
):
    """Returns a generator object that will step the cylindrical-polar
    point -> temp map accoring to finite_difference_step, yielding this map
    after each step.
    The point -> temp map has the form
        (idx_r, idx_a, idx_z) -> temp,
    where
        r = r_array[idx_r]
        a = a_array[idx_a]
        z = z_array[idx_z]
    :param time_step: time difference, dt
    :param r_array: array of r (radial) points
    :param a_array: array of a (angular) points [0, 2pi)
    :param z_array: array of z (axial) points
    :param t_src: source temperature
    :param t_atm: atmospheric (ambient) temperature
    :param alpha: constant of proportionality for the Laplacian
    :return generator object, which yields the point -> temp array after
    each step
    """
    # initialize points
    point_to_temp_map = dict()
    for idx_r in range(len(r_array)):
        for idx_a in range(len(a_array)):
            for idx_z in range(len(z_array)):
                if idx_z == 0 and idx_r == 0:
                    temp = t_src
                else:
                    temp = t_atm
                point_to_temp_map[(idx_r, idx_a, idx_z)] = temp
    # begin finite differencing
    yield point_to_temp_map
    while True:
        # do finite difference
        point_to_temp_map = finite_step_method(
            point_to_temp_map=point_to_temp_map,
            r_array=r_array, a_array=a_array, z_array=z_array, dt=time_step,
            alpha=alpha, t_src=t_src,
        )
        yield point_to_temp_map


# # do animation or something
# def update_lines(num, lines, temp_array, scalar_map):
#     temp_list = temp_array[num]
#     for line, temp in zip(lines, temp_list):
#         line.set_color(scalar_map.to_rgba(temp))
#     return lines
#
# NSKIP = 10
# TEMP_MIN = 200
# TEMP_MAX = 1000
# fig = plt.figure()
# ax = p3.Axes3D(fig)
# c_norm = colors.Normalize(vmin=TEMP_MIN, vmax=TEMP_MAX)
# scalar_map = cm.ScalarMappable(norm=c_norm, cmap='rainbow')
#
# data_points = sorted(point_to_temp_map.keys())
# temp_array = list()
# i = 0
# for point_to_temp_map in point_to_temp_map_list:
#     if i % NSKIP == 0:
#         temp_array.append(
#             [temp for point, temp in sorted(point_to_temp_map.items())])
#     i += 1
# lines = list()
# all_pts_list = list()
# for point, temp in zip(data_points, temp_array[0]):
#     idx_r, idx_a, idx_z = point
#     r, a, z = r_array[idx_r], a_array[idx_a], z_array[idx_z]
#     x = r * np.cos(a)
#     y = r * np.sin(a)
#     all_pts_list.append((x, y, z))
#     x_arr = np.array([x])
#     y_arr = np.array([y])
#     z_arr = np.array([z])
#     lines.append(
#         ax.plot(x_arr, y_arr, z_arr, 'o', color=scalar_map.to_rgba(temp))[0])
#
# ani = animation.FuncAnimation(
#     fig=fig, func=update_lines, frames=len(temp_array),
#     fargs=(lines, temp_array, scalar_map), interval=50, blit=False,
#     repeat=False,
# )
# plt.show()
