from itertools import count
from math import pi
import numpy as np
from simu1d import FiniteStepMethod, Simulation, run_simulation_f
from simu1d import d2_matrix


K_SB = 5.67 * 10 ** (-8)

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
    u_amb=300.,       # ambient temperature for convection
    k=125.,           # thermal conductivity of rod
    c=380.,           # specific heat of rod
    rho=8730,         # mass density of rod
    k_c=1.95,         # convection coefficient
    emiss=.01,        # emissivity
    power_heat=10.,   # power input before stop time
    power_cool=-10.,  # power input after stop time
    stop_time=900.,   # stop time
    perim=PERIMETER,
    area=AREA,
)

# finite differencing
TIME_STEP = 0.5
NUM_STEPS = int(2700 / TIME_STEP)
WRITE_PERIOD = 27


def _du_cond(u_prev, dx, k, c, rho):
    """Gets the contribution to u_next from conduction. This is
        k/(c*rho) * u''
    A central second derivative is used.
    :param u_prev: temperature vector from previous step
    :param dx: step in x
    :param k: thermal conductivity of rod
    :param c: specific heat of rod
    :param rho: mass density of rod
    """
    n = len(u_prev)
    mat = np.zeros(shape=(n, n))
    # make d2 matrix and add to mat
    d2 = d2_matrix(n=n, diff=1) / (dx**2)
    mat[1:-1] = d2
    # handle end conditions (top)
    d2_top = np.zeros_like(mat[0])
    d2_top[0] = -1 / dx**2
    d2_top[1] = 1 / dx**2
    mat[0] = d2_top
    # handle end conditions (bottom)
    d2_bot = np.zeros_like(mat[-1])
    d2_bot[-1] = -1 / dx**2
    d2_bot[-2] = 1 / dx**2
    mat[-1] = d2_bot
    # apply matrix to previous vector
    return k/c/rho * np.dot(mat, u_prev)


def _du_conv(u_prev, u_amb, k_c, rho, c, ds_vect, dv_vect):
    """Gets the contribution to u_next from convection. This is
        -k_c * ds / (rho * c * dv) * (u - u_amb),
    where ds is the differential surface element and dv is the differential
    volume element.
    :param u_prev: temperature vector from previous step
    :param u_amb: ambient temperature outside the rod
    :param k_c: convection coefficient
    :param rho: mass density of rod material
    :param c: specific heat of rod material
    :param ds_vect: vector that gives the exposed differential surface area for
    each x point
    :param dv_vect: vector that gives the differential volume for each
    x point
    """
    du_conv = np.zeros_like(u_prev)
    for u, ds, dv, i in zip(u_prev, ds_vect, dv_vect, count()):
        if dv > 0:
            du_conv[i] = -k_c*ds/rho/c/dv * (u - u_amb)
    return du_conv


def _du_rad(u_prev, u_amb, emiss, rho, c, ds_vect, dv_vect):
    """Gets the contribution to u_next from radiation from ambient air. This is
        -(emiss*K_SB*ds)/(rho*c*dv) * (u^4 - u_amb^4)
    :param u_prev: temperature vector from previous step
    :param u_amb: effective ambient radiation temperature
    :param emiss: emissivity of rod material
    :param rho: mass density of rod material
    :param c: specific heat of rod material
    :param ds_vect: vector that gives the exposed differential surface area
    for each x point
    :param dv_vect: vector that gives the shell differential volume for each
    x point
    """
    du_rad = np.zeros_like(u_prev)
    for u, ds, dv, i in zip(u_prev, ds_vect, dv_vect, count()):
        if dv > 0:
            du_rad[i] = -emiss*K_SB*ds/rho/c/dv * (u**4 - u_amb**4)
    return du_rad


def _du_power(power_in, rho, c, dv_vect):
    """Gets the contribution to u_next from input power. This is
        power / (rho * c * dV),
    for the point on the heated end
    :param power_in: power input
    :param rho: mass density of rod material
    :param c: specific heat of rod material
    :param dv_vect: vector containing differential volumes for each x point
    """
    du_power = np.zeros_like(dv_vect)
    du_power[0] = power_in/rho/c/dv_vect[0]
    return du_power


def _explicit_step_func_diffusion_simple(
        point_to_temp_map, time, x_array, dt, boundary_conditions,
        u_amb, k, c, rho, k_c, emiss, perim, area,
        power_heat, power_cool, stop_time,
):
    """Given the point -> temp map for the current time, make a step by dt to
    return the next point -> temp map
    :param point_to_temp_map: map from x index -> temperature
    :param time: current time
    :param x_array: array of actual x points
    :param dt: time step
    :param boundary_conditions: boundary conditions
    (not used here: included as an arg for duck-typing)
    :param u_amb: ambient temperature for convection and radiation
    :param k: thermal conductivity of rod materia
    :param c: specific heat of rod material
    :param rho: mass density of rod material
    :param k_c: convection constant
    :param emiss: emissivity of the rod material
    :param perim: perimeter of the rod (2*pi*R)
    :param area: cross-sectional area of the rod (pi*R**2)
    :param power_heat: power input during heating (t <= t_stop)
    :param power_cool: power input during cooling (t < t_stop)
    :param stop_time: time to change from cooling to heating
    """
    # get dx
    dx = x_array[1] - x_array[0]

    # make u_prev (previous temperature) vector
    points = sorted(point_to_temp_map.keys())
    u_prev_vect = np.array([point_to_temp_map[p] for p in points])

    # make ds and dv vectors
    ds_vect = perim*dx * np.ones_like(u_prev_vect)
    ds_vect[0] += area
    ds_vect[-1] += area
    dv_vect = area*dx * np.ones_like(u_prev_vect)

    # get contributions of conduction, convection, and radiation
    if time < stop_time:
        power_in = power_heat
    else:
        power_in = power_cool
    du_cond = _du_cond(u_prev=u_prev_vect, dx=dx, k=k, c=c, rho=rho)
    du_conv = _du_conv(
        u_prev=u_prev_vect, u_amb=u_amb, k_c=k_c, rho=rho, c=c,
        ds_vect=ds_vect, dv_vect=dv_vect)
    du_rad = _du_rad(
        u_prev=u_prev_vect, u_amb=u_amb, emiss=emiss, rho=rho, c=c,
        ds_vect=ds_vect, dv_vect=dv_vect)
    du_power = _du_power(power_in=power_in, rho=rho, c=c, dv_vect=dv_vect)

    # make u_next vector
    u_next_vect = np.copy(u_prev_vect)
    for du in [du_cond, du_conv, du_rad, du_power]:
        u_next_vect += dt * du

    # make and return next point -> temp map
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
