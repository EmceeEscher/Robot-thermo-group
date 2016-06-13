import numpy as np
from math import pi
from simu1d import FiniteStepMethod, run_simulation_f, d1_matrix, Simulation
from bc_1d import get_bc_neumann
from heat_eq_1d import heat_eq_matrix


# constants
STEFAN_BOLTZMANN = 5.67 * 10**(-8)
# dimensions
MIN_X = 0.0
MAX_X = .33
DIM_X = 33*2 + 1
# thermodynamical parameters
T_0 = 300.0  # initial temperature
T_AMB = 300.0  # ambient temperature
DT_SRC = 0.036  # rate of change of heating temperature
THERMAL_CONDUCTIVITY = 125.0
SPECIFIC_HEAT = 380.0
MASS_DENSITY = 8730.0
POROSITY_AIR = .01
VELOCITY_AIR = .01
# EMISSIVITY = 0.6
EMISSIVITY = .01
RADIUS = .01111
PERIMETER = 2*pi*RADIUS
AREA = pi*RADIUS**2
STOP_TIME = 900.0
PARAMS_DICT = {
    'thermal_conductivity': THERMAL_CONDUCTIVITY,
    'specific_heat': SPECIFIC_HEAT,
    'mass_density': MASS_DENSITY,
    'porosity_air': POROSITY_AIR,
    'velocity_air': VELOCITY_AIR,
    'emissivity': EMISSIVITY,
    'perimeter': PERIMETER,
    'area': AREA,
    'u_amb': T_AMB,
}
# finite differencing
TIME_STEP = 0.5
NUM_STEPS = int(2700 / TIME_STEP)
WRITE_PERIOD = 270
# points
X_ARRAY = np.linspace(MIN_X, MAX_X, DIM_X)


def conv_matrix(n, k):
    # TODO: docstring
    zerow = np.zeros(shape=(1, n+2))
    return k * np.r_[zerow, d1_matrix(n=n+2, diff=2), zerow]


def _implicit_mod_step_func_diffusion(
        point_to_temp_map, time, x_array, dt, boundary_conditions,
        thermal_conductivity, specific_heat, mass_density,
        porosity_air, velocity_air, emissivity, perimeter, area,
        u_amb, source_func=lambda x, t: 0,
):
    # get simplified parameters
    alpha = thermal_conductivity / (specific_heat * mass_density)
    beta = porosity_air * velocity_air
    gamma = (emissivity * STEFAN_BOLTZMANN * perimeter /
             (mass_density * area * specific_heat))
    theta = 1 / (specific_heat * mass_density)
    # get dx
    max_idx_x = len(x_array) - 1
    dx = max_idx_x / (x_array[max_idx_x] - x_array[0])
    # make u^n vector, u_amb^n vector, Q^n vector
    points = sorted(point_to_temp_map.keys())
    n = len(points)
    u_prev = np.zeros(shape=n+2)
    u_amb_vect = np.zeros(shape=n+2)
    q_vect = np.zeros(shape=n+2)
    for point, idx in zip(points, range(n)):
        u_prev[idx+1] = point_to_temp_map[point]
        u_amb_vect[idx+1] = u_amb
        q_vect[idx+1] = source_func(x_array[point], time)
    u_vector = u_prev - gamma*dt*(u_prev**4-u_amb_vect**4) + theta*dt*q_vect
    # make matrix
    mat_heat = heat_eq_matrix(n=n, k=-alpha * dt / dx ** 2)
    mat_conv = conv_matrix(n=n, k=beta*dt/(2*dx))
    mat = mat_heat + mat_conv
    # implement boundary conditions
    mat, u_prev, time = boundary_conditions(mat, u_vector, time)
    # solve for u^{n+1} vector
    tmat = np.transpose(mat)
    u_next = np.linalg.solve(np.dot(tmat, mat), np.dot(tmat, u_prev))
    # u_next = np.linalg.solve(mat, u_prev)
    # return point -> temp map
    return {p: t for p, t in zip(points, u_next[1:-1])}


implicit_mod_diffusion = FiniteStepMethod(
    f=_implicit_mod_step_func_diffusion,
    name='Modified implicit for convection-diffusion equation',
)


# script
if __name__ == '__main__':
    sim = Simulation(
        time_step=TIME_STEP, x_array=X_ARRAY, t_0=T_0,
        finite_step_method=implicit_mod_diffusion, params_dict=PARAMS_DICT,
        boundary_conditions=get_bc_neumann(
            x0=DT_SRC, x1=None, dx=(MAX_X+MIN_X)/(DIM_X+1)
        )
    )
    run_simulation_f(
        simulation=sim, num_steps=NUM_STEPS,
        fpath='../results/diffusion_try.dat', verbose=True,
        write_period=WRITE_PERIOD,
    )
