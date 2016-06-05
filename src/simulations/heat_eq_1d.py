from __future__ import division, print_function
from datetime import datetime
from bc_1d import *


# constants
# dimensions
MIN_X = 0.0
MAX_X = 1.0
DIM_X = 11
# thermodynamical parameters
ALPHA = 1.0
T_0 = 300.0  # initial temperature
T_AMB = 300.0  # ambient temperature
T_SRC = 1000.0  # heating temperature
# finite differencing
NUM_STEPS = 100000
TIME_STEP = 0.01
WRITE_PERIOD = 100
# points
X_ARRAY = np.linspace(MIN_X, MAX_X, DIM_X)


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


def _heat_eq_matrix(n, k):
    zerow = np.zeros(shape=(1, n+2))
    return np.eye(n+2) + k * np.r_[zerow, _d2_matrix(n+2), zerow]


# step functions
def _explicit_step_func(
        point_to_temp_map, time, x_array, dt, alpha, boundary_conditions,
):
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
    # make matrix
    mat = _heat_eq_matrix(n=n, k=alpha*dt/dx**2)
    # implement boundary conditions
    mat, u_prev, time = boundary_conditions(d_mat=mat, u_vect=u_prev, t=time)
    # get u^{n+1} vector
    u_next = np.dot(mat, u_prev)
    # return point -> temp map
    return {p: t for p, t in zip(points, u_next[1:-1])}


def _implicit_step_func(
        point_to_temp_map, time, x_array, dt, alpha, boundary_conditions,
):
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
    # make matrix
    mat = _heat_eq_matrix(n=n, k=-alpha * dt / dx**2)
    # implement boundary conditions
    mat, u_prev, time = boundary_conditions(d_mat=mat, u_vect=u_prev, t=time)
    # solve for u^{n+1} vector
    u_next = np.linalg.solve(mat, u_prev)
    # return point -> temp map
    return {p: t for p, t in zip(points, u_next[1:-1])}


def _crank_nicolson_step_func(
        point_to_temp_map, time, x_array, dt, alpha, boundary_conditions,
):
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
    # make matrix for u^n
    k1 = .5 * alpha * dt / dx**2
    mat1 = _heat_eq_matrix(n=n, k=k1)
    # make matrix for u^{n+1}
    k2 = -k1
    mat2 = _heat_eq_matrix(n=n, k=k2)
    # implement boundary conditions
    mat, u_prev, time = boundary_conditions(d_mat=mat2, u_vect=u_prev, t=time)
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
def simulation(
        time_step, x_array, t_0, alpha, finite_step_method,
        boundary_conditions,
):
    # initialize points
    point_to_temp_map = {idx_x: t_0 for idx_x in range(len(x_array))}
    time = 0
    # begin finite differencing
    yield point_to_temp_map
    while True:
        point_to_temp_map = finite_step_method(
            point_to_temp_map=point_to_temp_map,
            time=time, x_array=x_array, dt=time_step,
            alpha=alpha, boundary_conditions=boundary_conditions,
        )
        time += time_step
        yield point_to_temp_map


def run_simulation(
        fpath, verbose, write_period,
        dim_x, min_x, max_x, alpha, t_0,
        num_steps, time_step, finite_step_method, boundary_conditions,
):
    x_array = np.linspace(min_x, max_x, dim_x)
    s = simulation(
        time_step=time_step, x_array=x_array,
        t_0=t_0, alpha=alpha, finite_step_method=finite_step_method,
        boundary_conditions=boundary_conditions,
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
        '\n',
        ' Boundary conditions:\n',
        '   method=       {}\n'.format(boundary_conditions.name),
        '   x0_order=     {}\n'.format(boundary_conditions.x0_order),
        '   x1_order=     {}\n'.format(boundary_conditions.x1_order),
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
            if step % write_period == 0:
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
        fpath='./try.dat', verbose=True, write_period=WRITE_PERIOD,
        dim_x=DIM_X, min_x=MIN_X, max_x=MAX_X, alpha=ALPHA,
        t_0=T_0, num_steps=NUM_STEPS, time_step=TIME_STEP,
        finite_step_method=implicit,
        boundary_conditions=get_bc_dirichlet(x0=T_SRC, x1=T_AMB)
    )
