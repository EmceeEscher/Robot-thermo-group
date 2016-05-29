from __future__ import print_function, division
import numpy as np


class Cylinder:
    def __init__(self, radius, length, num_r_pts, num_a_pts, num_z_pts):
        self.max_r = radius
        self.max_a = np.pi
        self.max_z = length
        self.dim_r = num_r_pts
        self.dim_a = num_a_pts
        self.dim_z = num_z_pts
        self.d_r = (self.dim_r - 1) / self.max_r
        self.d_a = (self.dim_a - 1) / self.max_a
        self.d_z = (self.dim_z - 1) / self.max_z
        self._idx_to_r = np.linspace(0, self.max_r, num=self.dim_r)
        self._idx_to_a = np.linspace(0, self.max_a, num=self.dim_a)
        self._idx_to_z = np.linspace(0, self.max_z, num=self.dim_z)

    def indices_to_point(self, idx_r, idx_a, idx_z):
        return (self._idx_to_r[idx_r],
                self._idx_to_a[idx_a],
                self._idx_to_z[idx_z])

    def point_indices(self):
        for idx_z in range(self.dim_z):
            yield (0, 0, idx_z)
        for idx_r in range(1, self.dim_r):
            for idx_a in range(self.dim_a):
                for idx_z in range(self.dim_z):
                    yield (idx_r, idx_a, idx_z)

    def points(self):
        for indices in self.point_indices():
            yield self.indices_to_point(*indices)


# solve heat equation
LENGTH = 0.3
RADIUS = 0.02
ALPHA = 1000
NUM_PTS_R = 100
NUM_PTS_A = 100
NUM_PTS_Z = 100
T_AMBIENT = 300
T_HEAT = 400
D_TIME = 0.1
NUM_STEPS = 600
HEAT_LOSS_FACTOR = 0.2
# get cylinder shape
cylinder = Cylinder(
    radius=RADIUS, length=LENGTH,
    num_r_pts=NUM_PTS_R, num_a_pts=NUM_PTS_A, num_z_pts=NUM_PTS_Z)
# initial condition: Ambient temperature, except for heated end (z=0)
point_to_temperature = dict()
for indices in cylinder.point_indices():
    if indices[2] == 0:
        point_to_temperature[indices] = T_HEAT
    else:
        point_to_temperature[indices] = T_AMBIENT
# make step -> temperatures map
temperatures = list()
temperatures.append(point_to_temperature)
# begin finite difference
for step in range(NUM_STEPS):
    next_point_to_temperature = dict()
    for idx, temp in point_to_temperature.items():
        ii_r, ii_a, ii_z = idx
        r, a, z = cylinder.indices_to_point(*idx)
        dr, da, dz = cylinder.d_r, cylinder.d_a, cylinder.d_z
        tr0 = temp
        next_temp = temp
        # radial part
        if 0 < ii_r < NUM_PTS_R - 1:
            tr1 = point_to_temperature[ii_r+1, ii_a, ii_z]
            if ii_r != 1:
                tr_1 = point_to_temperature[ii_r-1, ii_a, ii_z]
            else:
                tr_1 = point_to_temperature[ii_r-1, 0, ii_z]
            next_temp += ALPHA**2 * D_TIME * (
                1/(r*dr) * (tr1 - tr0) + 1/(dr**2) * (tr1 - 2*tr0 + tr_1))
        elif ii_r == 0:  # center
            tr1 = point_to_temperature[ii_r+1, ii_a, ii_z]
            tr_1 = point_to_temperature[ii_r+1, NUM_PTS_A - 1 - ii_a, ii_z]
            next_temp += ALPHA**2 * D_TIME * (
                1/(dr**2) * (tr1 - 2*tr0 + tr_1))
        else:  # surface
            tr_1 = point_to_temperature[ii_r-1, ii_a, ii_z]
            tr1 = tr_1  # constant slope wrt r at surface
            next_temp -= HEAT_LOSS_FACTOR * (tr0 - T_AMBIENT)
            next_temp += ALPHA**2 * D_TIME * (
                1/(r*dr) * (tr1 - tr0) + 1/(dr**2) * (tr1 - 2*tr0 + tr_1))
        # angular part
        if ii_r > 0:  # no angular contribution at center
            if ii_a == 0:
                tr1 = point_to_temperature[ii_r, ii_a + 1, ii_z]
                tr_1 = tr1
            elif ii_a == NUM_PTS_A - 1:
                tr_1 = point_to_temperature[ii_r, ii_a - 1, ii_z]
                tr1 = tr_1
            else:
                tr1 = point_to_temperature[ii_r, ii_a + 1, ii_z]
                tr_1 = point_to_temperature[ii_r, ii_a - 1, ii_z]
            next_temp += ALPHA**2 * D_TIME / (r * da)**2 * (tr1 - 2*tr0 + tr_1)
        # axial part
        if 0 < ii_z < NUM_PTS_Z - 1:
            tr1 = point_to_temperature[ii_r, ii_a, ii_z + 1]
            tr_1 = point_to_temperature[ii_r, ii_a, ii_z - 1]
        elif ii_z == 0:
            next_point_to_temperature[idx] = T_HEAT
            continue
        else:  # other end
            tr_1 = point_to_temperature[ii_r, ii_a, ii_z - 1]
            tr1 = tr_1
            next_temp -= HEAT_LOSS_FACTOR * (tr0 - T_AMBIENT)
        next_temp += ALPHA**2 * D_TIME/(dz**2) * (tr1 - 2*tr0 + tr_1)
        next_point_to_temperature[idx] = next_temp
    temperatures.append(next_point_to_temperature)
    point_to_temperature = next_point_to_temperature
    print('{} {} {} {} {}'.format(
        point_to_temperature[(NUM_PTS_R-1, 0, 0)],
        point_to_temperature[(NUM_PTS_R-1, 0, 1)],
        point_to_temperature[(NUM_PTS_R-1, 0, 2)],
        point_to_temperature[(NUM_PTS_R-1, 0, 3)],
        point_to_temperature[(NUM_PTS_R-1, 0, 4)],
    ))
