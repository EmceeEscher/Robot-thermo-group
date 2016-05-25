from __future__ import print_function, division
from math import pi
import numpy as np

MIN_R = 0.0
MAX_R = 0.1
DIM_R = 10
MIN_A = 0.0
MAX_A = 2 * pi
DIM_A = 10
MIN_Z = 0.0
MAX_Z = 0.5
DIM_Z = 10
T_ATM = 300.0
T_SRC = 1000.0
NUM_STEPS = 1000
TIME_STEP = 1.0
ALPHA = 1.0

# initialize points
dt = TIME_STEP
dr = (DIM_R - 1) / (MAX_R - MIN_R)
da = (DIM_A - 1) / (MAX_A - MIN_A)
dz = (DIM_Z - 1) / (MAX_Z - MIN_Z)
min_idx_r, max_idx_r = 0, DIM_R - 1
min_idx_a, max_idx_a = 0, DIM_A - 1
min_idx_z, max_idx_z = 0, DIM_Z - 1
r_array = np.linspace(MIN_R, MAX_R, DIM_R)
a_array = np.linspace(MIN_A, MAX_A, DIM_A, endpoint=False)
z_array = np.linspace(MIN_Z, MAX_Z, DIM_Z)
point_to_temp_map = dict()
for idx_r in range(len(r_array)):
    for idx_a in range(len(a_array)):
        for idx_z in range(len(z_array)):
            if idx_z == 0 and idx_r == 0:
                temp = T_SRC
            else:
                temp = T_ATM
            point_to_temp_map[(idx_r, idx_a, idx_z)] = temp

# begin finite differencing
for step in range(NUM_STEPS):
    # print stuff
    for point, temp in sorted(point_to_temp_map.items()):
        idx_r, idx_a, idx_z = point
        if idx_a == 0 and idx_r == 0:
            print('{:8.2f}'.format(temp), end=' ')
    print()
    # do finite differencing
    next_point_to_temp_map = dict()
    for point, temp in point_to_temp_map.items():
        idx_r, idx_a, idx_z = point
        r, a, z = r_array[idx_r], a_array[idx_a], z_array[idx_z]
        next_temp = temp
        # radial part
        if min_idx_r < idx_r < max_idx_r:
            t1 = point_to_temp_map[(idx_r+1, idx_a, idx_z)]
            t0 = temp
            t_1 = point_to_temp_map[(idx_r-1, idx_a, idx_z)]
        else:  # boundary conditions
            if min_idx_r == idx_r:
                t1 = point_to_temp_map[(idx_r+1, idx_a, idx_z)]
                t0 = t1
                t_1 = point_to_temp_map[
                    (idx_r, (idx_a+DIM_A/2) % DIM_A, idx_z)]
            else:
                t_1 = point_to_temp_map[(idx_r-1, idx_a, idx_z)]
                t0 = temp
                t1 = t0
        next_temp += ALPHA*dt/dr**2 * (t1 - 2*t0 + t_1)
        if r != 0:
            next_temp += ALPHA*dt/(r*dr) * (t1 - t0)
        # angular part
        if min_idx_a < idx_a < max_idx_a:
            t1 = point_to_temp_map[(idx_r, idx_a+1, idx_z)]
            t0 = temp
            t_1 = point_to_temp_map[(idx_r, idx_a-1, idx_z)]
        else:  # boundary conditions
            if min_idx_a == idx_a:
                t1 = point_to_temp_map[(idx_r, idx_a+1, idx_z)]
                t0 = temp
                t_1 = point_to_temp_map[(idx_r, max_idx_a, idx_z)]
            else:
                t_1 = point_to_temp_map[(idx_r, idx_a-1, idx_z)]
                t0 = temp
                t1 = point_to_temp_map[(idx_r, min_idx_a, idx_z)]
        if r != 0:
            next_temp += ALPHA*dt/(r*da)**2 * (t1 - 2*t0 + t_1)
        # axial part
        if min_idx_z < idx_z < max_idx_z:
            t1 = point_to_temp_map[(idx_r, idx_a, idx_z+1)]
            t0 = temp
            t_1 = point_to_temp_map[(idx_r, idx_a, idx_z-1)]
        else:  # boundary conditions
            if min_idx_z == idx_z:
                next_temp = T_SRC
                next_point_to_temp_map[point] = next_temp
                continue
            else:
                t_1 = point_to_temp_map[(idx_r, idx_a, idx_z-1)]
                t0 = temp
                t1 = t0
        next_temp += ALPHA*dt/dz**2 * (t1 - 2*t0 + t_1)
        # add point to map
        next_point_to_temp_map[point] = next_temp
    point_to_temp_map = next_point_to_temp_map
