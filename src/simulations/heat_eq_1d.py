from __future__ import division, print_function
from datetime import datetime
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


def _deriv_matrix(size, k):
    n = size
    mat = np.eye(n)
    for row in range(1, n-1):
        for col in range(n):
            if abs(row - col) == 1:
                mat[row, col] = k
            elif row == col:
                mat[row, col] = 1 - 2 * k
    return mat


def _d1_matrix(n, diff=1):
    rows = n - diff
    cols = n
    mat = np.zeros(shape=(rows, cols))
    for i in range(rows):
        for j in range(cols):
            if j - i == 0:
                mat[i, j] = -1
            elif j - i == diff:
                mat[i, j] = 1
    return mat


def _d2_matrix(n, diff=1):
    return np.dot(_d1_matrix(n=n-diff, diff=diff), _d1_matrix(n=n, diff=diff))


# boundary conditions
def _x0_d0(f_t):
    def bc(d_mat, u_vect, t):
        d_mat[0] = np.zeros_like(d_mat[0])
        d_mat[0, 0] = 1
        u_vect[0] = f_t(t)
        return d_mat, u_vect, t
    return bc


def _x0_d1(g_t):
    def bc(d_mat, u_vect, t, dx):
        d_mat[0] = np.zeros_like(d_mat[0])
        d_mat[0, 0] = 1
        u_vect[0] = u_vect[2] - 2 * dx * g_t(t)
        return d_mat, u_vect, t
    return bc


def _x1_d0(f_t):
    def bc(d_mat, u_vect, t):
        d_mat[-1] = np.zeros_like(d_mat[-1])
        d_mat[-1, -1] = 1
        u_vect[-1] = f_t(t)
        return d_mat, u_vect, t
    return bc


def _x1_d1(g_t):
    def bc(d_mat, u_vect, t, dx):
        d_mat[-1] = np.zeros_like(d_mat[-1])
        d_mat[-1, -1] = 1
        u_vect[-1] = u_vect[-3] + 2 * dx * g_t(t)
        return d_mat, u_vect, t
    return bc


# step functions
def _explicit_step_func(point_to_temp_map, x_array, dt, alpha, t_src, t_amb):
    """Make a step forward in time using the explicit finite difference
    method for the heat equation:
        u_i^{n+1} = u_i^n + k * (u_{i+1}^n - 2*u_i^n + u_{i-1}^n),
    where
        k = alpha * dt / dx^2
    """
    max_idx_x = len(x_array) - 1
    dx = max_idx_x / (x_array[max_idx_x] - x_array[0])
    # make u^n vector
    points = sorted(point_to_temp_map.keys())
    n = len(points)
    u_prev = np.zeros(shape=n+2)
    for point, idx in zip(points, range(n)):
        u_prev[idx+1] = point_to_temp_map[point]
    # boundary condition ghost points
    u_prev[0] = t_src
    u_prev[n+1] = t_amb
    # make matrix
    mat = _deriv_matrix(size=n+2, k=alpha*dt/dx**2)
    # get u^{n+1} vector
    u_next = np.dot(mat, u_prev)
    # return point -> temp map
    return {p: t for p, t in zip(points, u_next[1:-1])}


def _implicit_step_func(point_to_temp_map, x_array, dt, alpha, t_src, t_amb):
    """Make a step forward in time using the implicit finite difference
    method for the heat equation:
        k*u_{i-1}^{n+1} + (1-2k)*u_i^{n+1} + k*u_{i+1}^{n+1} = u_i^n,
    where
        k = - alpha * dt / dx^2
    """
    max_idx_x = len(x_array) - 1
    dx = max_idx_x / (x_array[max_idx_x] - x_array[0])
    # make u^n vector
    points = sorted(point_to_temp_map.keys())
    n = len(points)
    u_prev = np.zeros(shape=n+2)
    for point, idx in zip(points, range(n)):
        u_prev[idx+1] = point_to_temp_map[point]
    # boundary condition ghost points
    u_prev[0] = t_src
    u_prev[n+1] = t_amb
    # make matrix
    mat = _deriv_matrix(size=n+2, k=-alpha * dt / dx**2)
    # solve for u^{n+1} vector
    u_next = np.linalg.solve(mat, u_prev)
    # return point -> temp map
    return {p: t for p, t in zip(points, u_next[1:-1])}


def _crank_nicolson_step_func(
        point_to_temp_map, x_array, dt, alpha, t_src, t_amb):
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
    points = sorted(point_to_temp_map.keys())
    n = len(points)
    u_prev = np.zeros(shape=n+2)
    for point, idx in zip(points, range(n)):
        u_prev[idx+1] = point_to_temp_map[point]
    # boundary condition ghost points
    u_prev[0] = t_src
    u_prev[n+1] = t_amb
    # make matrix for u^n
    k1 = .5 * alpha * dt / dx**2
    mat1 = _deriv_matrix(size=n+2, k=k1)
    # make matrix for u^{n+1}
    k2 = -k1
    mat2 = _deriv_matrix(size=n+2, k=k2)
    # solve for u^{n+1} vector
    u_next = np.linalg.solve(mat2, np.dot(mat1, u_prev))
    # return point -> temp map
    return {p: t for p, t in zip(points, u_next[1:-1])}


class FiniteStepMethod:
    def __init__(self, f, name):
        self._f = f
        self.name = name

    def __call__(self, *args, **kwargs):
        return self._f(*args, **kwargs)


explicit = FiniteStepMethod(f=_explicit_step_func, name='Explicit')

implicit = FiniteStepMethod(f=_implicit_step_func, name='Implicit')

crank_nicolson = FiniteStepMethod(
    f=_crank_nicolson_step_func, name='Crank-Nicolson')


# functions
def simulation(time_step, x_array, t_src, t_amb, t_0, alpha,
               finite_step_method=crank_nicolson):
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


def run_simulation(
        fpath, verbose,
        dim_x, min_x, max_x, alpha, t_0, t_amb, t_src,
        num_steps, time_step, finite_step_method
):
    x_array = np.linspace(min_x, max_x, dim_x)
    s = simulation(
        time_step=time_step, x_array=x_array,
        t_src=t_src, t_amb=t_amb, t_0=t_0, alpha=alpha,
        finite_step_method=finite_step_method
    )
    lines = [
        ' {}\n'.format(str(datetime.now())),
        '\n',
        ' Simulation of heat flow through 1-dimensional rod\n',
        '\n',
        ' Dimensions\n',
        '   dim_x=        {}\n'.format(dim_x),
        '   min_x=        {}\n'.format(min_x),
        '   max_x=        {}\n'.format(max_x),
        '\n',
        ' Thermodynamical parameters\n',
        '   alpha=        {}\n'.format(alpha),
        '   t_0=          {}\n'.format(t_0),
        '   t_amb=        {}\n'.format(t_amb),
        '   t_src=        {}\n'.format(t_src),
        '\n',
        ' Finite differencing\n',
        '   num_steps=    {}\n'.format(num_steps),
        '   time_step=    {}\n'.format(time_step),
        '   method=       {}\n'.format(finite_step_method.name),
        '\n',
    ]
    if verbose:
        print(''.join(lines), end='\n')
    with open(fpath, 'w') as f:
        for line in lines:
            f.write('#' + line)
        for point_to_temp_map, step in zip(s, range(num_steps+1)):
            line = 'Step= {}\n'.format(step)
            f.write(line)
            if verbose:
                print(line, end='')
            for point, temp in point_to_temp_map.items():
                line = '{:8}  {:16.8f}\n'.format(point, temp)
                f.write(line)
                if verbose:
                    print(line, end='')


# script
if __name__ == '__main__':
    run_simulation(
        fpath='./try.dat', verbose=True,
        dim_x=DIM_X, min_x=MIN_X, max_x=MAX_X, alpha=ALPHA,
        t_0=T_0, t_amb=T_AMB, t_src=T_SRC,
        num_steps=NUM_STEPS, time_step=TIME_STEP,
        finite_step_method=crank_nicolson
    )
