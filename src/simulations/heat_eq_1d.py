from __future__ import division, print_function
from math import pi
import numpy as np

# constants
# dimensions
MIN_X = 0.0
MAX_X = 1.0
DIM_X = 10
# thermodynamical parameters
ALPHA = 1.0
T_0 = 300.0  # initial temperature
T_AMB = 300.0  # ambient temperature
T_SRC = 1000.0  # heating temperature
# finite differencing
NUM_STEPS = 1000
TIME_STEP = 0.01
# points
X_ARRAY = np.linspace(MIN_X, MAX_X, DIM_X)


# functions
def _deriv_matrix(num_row, num_col, k):
    m, n = num_row, num_col
    mat = np.zeros(shape=(m, n))
    for row in range(m):
        for col in range(n):
            if (row, col) in [(0, 0), (m-1, n-1)]:
                mat[row, col] = 1
            elif abs(row - col) == 1:
                mat[row, col] = k
            elif row == col:
                mat[row, col] = 1 - 2 * k
    return mat


def explicit_step(point_to_temp_map, x_array, dt, alpha, t_src, t_amb):
    """Make a step forward in time using the explicit finite difference
    method for the heat equation:
        u_i^{n+1} = u_i^n + k * (u_{i+1}^n - 2*u_i^n + u_{i-1}^n),
    where
        k = alpha * dt / dx^2
    """
    max_idx_x = len(x_array) - 1
    dx = max_idx_x / (x_array[max_idx_x] - x_array[0])
    next_point_to_temp_map = dict()
    for point, temp in point_to_temp_map.items():
        idx_x = point
        u0 = point_to_temp_map[idx_x]
        if idx_x == 0:
            u_1 = t_src
            u1 = point_to_temp_map[idx_x+1]
        elif idx_x == max_idx_x:
            u_1 = point_to_temp_map[idx_x-1]
            u1 = t_amb
        else:
            u_1 = point_to_temp_map[idx_x-1]
            u1 = point_to_temp_map[idx_x+1]
        k = alpha * dt / dx**2
        next_temp = k * u1 + (1 - 2*k) * u0 + k * u_1
        next_point_to_temp_map[idx_x] = next_temp
    return next_point_to_temp_map


def implicit_step(point_to_temp_map, x_array, dt, alpha, t_src, t_amb):
    """Make a step forward in time using the implicit finite difference
    method for the heat equation:
        k*u_{i-1}^{n+1} + (1-2k)*u_i^{n+1} + k*u_{i+1}^{n+1} = u_i^n,
    where
        k = - alpha * dt / dx^2
    """
    max_idx_x = len(x_array) - 1
    dx = max_idx_x / (x_array[max_idx_x] - x_array[0])
    # make u^n vector
    points = sorted(point_to_temp_map.values())
    n = len(points)
    u_prev = np.empty(shape=n+2)
    for point, idx in zip(points, range(n)):
        u_prev[idx+1] = point_to_temp_map[point]
    # boundary condition ghost points
    u_prev[0] = t_src
    u_prev[n+1] = t_amb
    # make matrix
    mat = _deriv_matrix(num_row=n, num_col=n+2, k=-alpha * dt / dx**2)
    # solve for u^{n+1} vector
    u_next = np.linalg.solve(mat, u_prev)
    # return point -> temp map
    return {p: t for p, t in zip(points, u_next)}


def crank_nicolson_step(point_to_temp_map, x_array, dt, alpha, t_src, t_amb):
    """Make a step forward in time using the Crank-Nicolson finite difference
    method for the heat equation:
         k*u_{i-1}^{n+1} + (1-2k)*u_i^{n+1} + k*u_{i+1}^{n+1} =
        -k*u_{i-1}^n     + (1+2k)*u_i^n     - k*u_{i+1}^n
    where
        k = (-1/2) * alpha * dt / dx^2
    """
    max_idx_x = len(x_array) - 1
    dx = max_idx_x / (x_array[max_idx_x] - x_array[0])
    # make u^n vector
    points = sorted(point_to_temp_map.values())
    n = len(points)
    u_prev = np.empty(shape=n+2)
    for point, idx in zip(points, range(n)):
        u_prev[idx+1] = point_to_temp_map[point]
    # boundary condition ghost points
    u_prev[0] = t_src
    u_prev[n+1] = t_amb
    # make matrix for u_n
    k1 = .5 * alpha * dt / dx**2
    mat1 = _deriv_matrix(num_row=n+2, num_col=n+2, k=k1)
    u_prev = np.dot(mat1, u_prev)
    k2 = -k1
    mat2 = _deriv_matrix(num_row=n, num_col=n+2, k=k2)
    # solve for u^{n+1} vector
    u_next = np.linalg.solve(mat2, u_prev)
    # return point -> temp map
    return {p: t for p, t in zip(points, u_next)}


def simulation(time_step, x_array, t_src, t_amb, t_0, alpha,
               finite_step_method=crank_nicolson_step):
    # initialize points
    point_to_temp_map = {idx_x: t_0 for idx_x in range(len(x_array))}
    # begin finite differencing
    yield point_to_temp_map
    while True:
        point_to_temp_map = finite_step_method(
            point_to_temp_map=point_to_temp_map,
            x_array=x_array, dt=time_step,
            alpha=alpha, t_src=t_src, t_amb=t_amb
        )
        yield point_to_temp_map


# script
s = simulation(
    time_step=TIME_STEP, x_array=X_ARRAY, t_src=T_SRC, t_amb=T_AMB,
    t_0=T_0, alpha=ALPHA, finite_step_method=explicit_step)

for point_to_temp_map, step in zip(s, range(NUM_STEPS)):
    for point, temp in sorted(point_to_temp_map.items()):
        print('{:8.2f}'.format(temp), end=' ')
    print()
