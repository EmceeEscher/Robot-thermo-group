import numpy as np
from simu1d import FiniteStepMethod, Simulation, run_simulation_f
from heat_eq_1d import heat_eq_matrix
from diffusion_1d import STEFAN_BOLTZMANN, PERIMETER, AREA
from bc_1d import get_bc_simple_heat_diffusion, get_bc_simple_heat_diffusion2


# dimensions
MIN_X = 0.
MAX_X = 1.
DIM_X = 10 + 1

# thermodynamical parameters
T_0 = 300.
T_AMB = 300.
PARAMS_DICT = dict(
    u_amb=300.,
    thermal_conductivity=125.,
    specific_heat=380.,
    mass_density=8730,
    convection_coeff=.05,
    emissivity=.01,
    perimeter=PERIMETER,
    area=AREA,
    power=1000.,
    stop_time=900.,
)

# Finite differencing
TIME_STEP = 0.5
NUM_STEPS = int(2700 / TIME_STEP)
WRITE_PERIOD = 27

# Points
X_ARRAY = np.linspace(MIN_X, MAX_X, DIM_X)


def _get_power_fn(power, stop_time):
    return lambda t: power if t < stop_time else 0


def _implicit_mod_step_func_diffusion_simple(
        point_to_temp_map, time, x_array, dt, boundary_conditions,
        u_amb, thermal_conductivity, specific_heat, mass_density,
        convection_coeff, emissivity, perimeter, area, power, stop_time,
):
    # get dx
    dx = (x_array[-1] - x_array[0]) / (len(x_array) - 1)
    # make u^n vector
    points = sorted(point_to_temp_map.keys())
    n = len(points)
    u_prev = np.zeros(shape=n+2)
    u_amb_vect = np.zeros(shape=n+2)
    for point, idx in zip(points, range(n)):
        u_prev[idx+1] = point_to_temp_map[point]
        u_amb_vect[idx+1] = u_amb
    denom = specific_heat*mass_density*area
    u_vect = (
        u_prev +
        dt*convection_coeff*perimeter/denom * u_amb_vect -
        dt*emissivity*STEFAN_BOLTZMANN*perimeter/denom *
        (u_prev**4-u_amb_vect**4)
    )
    mat_heat = heat_eq_matrix(
        n=n, k=-(dt/dx**2)*thermal_conductivity/(specific_heat*mass_density))
    mat_conv = convection_coeff*perimeter*dt/denom * np.eye(n+2)
    # implement boundary conditions
    if boundary_conditions is None:
        boundary_conditions = get_bc_simple_heat_diffusion(
            power_fn=_get_power_fn(power=power, stop_time=stop_time),
            dt=dt, dx=dx, denom=denom, k_c=convection_coeff,
            area=area, perimeter=perimeter, u_amb=u_amb_vect,
        )
    mat, u_prev, time = boundary_conditions(mat_heat+mat_conv, u_vect, time)
    # solve for u^{n+1} vector
    # tmat = np.transpose(mat)
    # u_next = np.linalg.solve(np.dot(tmat, mat), np.dot(tmat, u_vect))
    u_next = np.linalg.solve(mat, u_vect)
    return {p: t for p, t in zip(points, u_next[1:-1])}


def _implicit_mod2_step_func_diffusion_simple(
        point_to_temp_map, time, x_array, dt, boundary_conditions,
        u_amb, thermal_conductivity, specific_heat, mass_density,
        convection_coeff, emissivity, perimeter, area, power, stop_time,
):
    # get dx
    dx = (x_array[-1] - x_array[0]) / (len(x_array) - 1)
    # make u^n vector
    points = sorted(point_to_temp_map.keys())
    n = len(points)
    u_prev = np.zeros(shape=n+2)
    u_amb_vect = np.zeros(shape=n+2)
    for point, idx in zip(points, range(n)):
        u_prev[idx+1] = point_to_temp_map[point]
        u_amb_vect[idx+1] = u_amb
    denom = specific_heat*mass_density*area
    u_vect = (
        u_prev +
        dt*convection_coeff*perimeter/denom * (u_prev-u_amb_vect) -
        dt*emissivity*STEFAN_BOLTZMANN*perimeter/denom *
        (u_prev**4-u_amb_vect**4)
    )
    mat_heat = heat_eq_matrix(
        n=n, k=-(dt/dx**2)*thermal_conductivity/(specific_heat*mass_density))
    # implement boundary conditions
    if boundary_conditions is None:
        boundary_conditions = get_bc_simple_heat_diffusion2(
            power_fn=_get_power_fn(power=power, stop_time=stop_time),
            dt=dt, dx=dx, denom=denom, k_c=convection_coeff,
            area=area, perimeter=perimeter,
            u_prev=u_prev, u_amb=u_amb_vect,
        )
    mat, u_vect, time = boundary_conditions(mat_heat, u_vect, time)
    # solve for u^{n+1} vector
    # tmat = np.transpose(mat)
    # u_next = np.linalg.solve(np.dot(tmat, mat), np.dot(tmat, u_vect))
    u_next = np.linalg.solve(mat, u_vect)
    # return point -> temp map
    return {p: t for p, t in zip(points, u_next[1:-1])}


implicit_mod_diffusion_simple = FiniteStepMethod(
    f=_implicit_mod_step_func_diffusion_simple,
    name='Modified implicit for simple convection-diffusion equation',
)


implicit_mod2_diffusion_simple = FiniteStepMethod(
    f=_implicit_mod2_step_func_diffusion_simple,
    name='Modified implicit for simple convection-diffusion equation',
)


if __name__ == '__main__':
    sim = Simulation(
        time_step=TIME_STEP, x_array=X_ARRAY, t_0=T_0,
        finite_step_method=implicit_mod_diffusion_simple,
        params_dict=PARAMS_DICT, boundary_conditions=None,
    )
    run_simulation_f(
        simulation=sim, num_steps=NUM_STEPS,
        fpath='../results/diff_simp_try.dat', verbose=True,
        write_period=WRITE_PERIOD,
    )
