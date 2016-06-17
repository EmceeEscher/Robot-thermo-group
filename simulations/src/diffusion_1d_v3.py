from math import pi
import numpy as np
from heat_eq_1d import heat_eq_matrix
from simu1d import FiniteStepMethod, Simulation, run_simulation_f


STEFAN_BOLTZMANN = 5.67 * 10**(-8)

# dimension
MIN_X = 0.
MAX_X = 0.33
DIM_X = 10 + 1
X_ARRAY = np.linspace(MIN_X, MAX_X, DIM_X)
RADIUS = .01111
PERIMETER = 2*pi*RADIUS
AREA = pi*RADIUS**2

# thermodynamical parameters
TEMP_INITIAL = 300.
PARAMS_DICT = dict(
    u_amb=300.,
    thermal_conductivity=125.,
    specific_heat=380.,
    mass_density=8730,
    convection_coeff=1.95,
    emissivity=.01,
    perimeter=PERIMETER,
    area=AREA,
    power=10.,
    power2=-10.,
    stop_time=900.,
)

# finite differencing
TIME_STEP = 0.5
NUM_STEPS = int(2700 / TIME_STEP)
WRITE_PERIOD = 27


def _explicit_step_func_diffusion_simple(
        point_to_temp_map, time, x_array, dt, boundary_conditions,
        u_amb, thermal_conductivity, specific_heat, mass_density,
        convection_coeff, emissivity, perimeter, area, power, power2,
        stop_time,
):
    # get dx
    dx = x_array[1] - x_array[0]
    # make u^n vector
    points = sorted(point_to_temp_map.keys())
    n = len(points)
    u_prev_vect = np.array([point_to_temp_map[p] for p in points])
    u_amb_vect = u_amb * np.ones(shape=n)
    denom = specific_heat * mass_density * area
    du_rad = (
        -dt * emissivity * STEFAN_BOLTZMANN * perimeter / denom *
        (u_prev_vect**4 - u_amb_vect**4))
    du_conv = (
        -dt * convection_coeff * perimeter / denom *
        (u_prev_vect - u_amb_vect))
    k_heat = (dt/dx**2)*thermal_conductivity/(specific_heat*mass_density)
    # print(dt - dx**2/(thermal_conductivity/(specific_heat*mass_density)))
    mat_heat = heat_eq_matrix(n=n-2, k=k_heat)
    du_cond = np.dot(mat_heat, u_prev_vect)
    u_next_vect = du_rad + du_conv + du_cond
    # boundary conditions
    if time >= stop_time:
        power = power2
    u_next_vect[0] += (
        area/(perimeter*dx) * (du_rad[0] + du_conv[0]) +
        k_heat * (u_prev_vect[1] - u_prev_vect[0]) +
        dt/dx * power/denom
    )
    u_next_vect[-1] += (
        area/(perimeter*dx) * (du_rad[-1] + du_conv[-1]) +
        k_heat * (u_prev_vect[-2] - u_prev_vect[-1])
    )
    return {p: t for p, t in zip(points, u_next_vect)}


explicit_diffusion_simple = FiniteStepMethod(
    f=_explicit_step_func_diffusion_simple,
    name='Explicit for simple convection-diffusion equation'
)


if __name__ == '__main__':
    sim = Simulation(
        time_step=TIME_STEP, x_array=X_ARRAY, t_0=TEMP_INITIAL,
        finite_step_method=explicit_diffusion_simple,
        params_dict=PARAMS_DICT, boundary_conditions=None,
    )
    run_simulation_f(
        simulation=sim, num_steps=NUM_STEPS,
        fpath='../results/suicide.dat', verbose=True,
        write_period=WRITE_PERIOD,
    )
