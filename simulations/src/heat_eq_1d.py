import numpy as np
from bc_1d import get_bc_dirichlet
from simu1d import d2_matrix, FiniteStepMethod, run_simulation_f

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
WRITE_PERIOD = 1000
# points
X_ARRAY = np.linspace(MIN_X, MAX_X, DIM_X)


def heat_eq_matrix(n, k):
    # TODO: docstring
    zerow = np.zeros(shape=(1, n+2))
    return np.eye(n+2) + k * np.r_[zerow, d2_matrix(n + 2), zerow]


# step functions
def _explicit_step_func_heat_eq(
        point_to_temp_map, time, x_array, dt, boundary_conditions, alpha,
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
    mat = heat_eq_matrix(n=n, k=alpha * dt / dx ** 2)
    # implement boundary conditions
    mat, u_prev, time = boundary_conditions(d_mat=mat, u_vect=u_prev, t=time)
    # get u^{n+1} vector
    u_next = np.dot(mat, u_prev)
    # return point -> temp map
    return {p: t for p, t in zip(points, u_next[1:-1])}


def _implicit_step_func_heat_eq(
        point_to_temp_map, time, x_array, dt, boundary_conditions, alpha,
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
    mat = heat_eq_matrix(n=n, k=-alpha * dt / dx ** 2)
    # implement boundary conditions
    mat, u_prev, time = boundary_conditions(d_mat=mat, u_vect=u_prev, t=time)
    # solve for u^{n+1} vector
    u_next = np.linalg.solve(mat, u_prev)
    # return point -> temp map
    return {p: t for p, t in zip(points, u_next[1:-1])}


def _crank_nicolson_step_func_heat_eq(
        point_to_temp_map, time, x_array, dt, boundary_conditions, alpha,
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
    mat1 = heat_eq_matrix(n=n, k=k1)
    # make matrix for u^{n+1}
    k2 = -k1
    mat2 = heat_eq_matrix(n=n, k=k2)
    # implement boundary conditions
    mat, u_prev, time = boundary_conditions(d_mat=mat2, u_vect=u_prev, t=time)
    # solve for u^{n+1} vector
    u_next = np.linalg.solve(mat2, np.dot(mat1, u_prev))
    # return point -> temp map
    return {p: t for p, t in zip(points, u_next[1:-1])}


explicit_heat_eq = FiniteStepMethod(
    f=_explicit_step_func_heat_eq, name='Explicit')

implicit_heat_eq = FiniteStepMethod(
    f=_implicit_step_func_heat_eq, name='Implicit')

crank_nicolson_heat_eq = FiniteStepMethod(
    f=_crank_nicolson_step_func_heat_eq, name='Crank-Nicolson')


# script
if __name__ == '__main__':
    run_simulation_f(
        fpath='../results/try.dat', verbose=True, write_period=WRITE_PERIOD,
        dim_x=DIM_X, min_x=MIN_X, max_x=MAX_X,
        t_0=T_0, num_steps=NUM_STEPS, time_step=TIME_STEP,
        finite_step_method=implicit_heat_eq,
        boundary_conditions=get_bc_dirichlet(x0=T_SRC, x1=T_AMB),
        params_dict={'alpha': ALPHA}
    )
