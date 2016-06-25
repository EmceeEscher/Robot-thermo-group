from datetime import datetime
from math import ceil, floor
from os import path, makedirs
from scipy.optimize import least_squares
import numpy as np
from simu1d import Simulation, run_simulation_opt
from opt_diffusion_1d import make_params_dict, iteration
from opt_diffusion_1d import get_exp_time_temp_arrays
from diffusion_1d import PERIMETER, AREA, MIN_X, MAX_X
from diffusion_1d_v2 import implicit_mod_diffusion_simple
from diffusion_1d_v2 import implicit_mod2_diffusion_simple
from diffusion_1d_v3 import explicit_diffusion_simple


# input files
DATA_FPATHS = [
    '../../data/temperature data/June 8/Run1_tc1_v2.dat',
    '../../data/temperature data/June 8/Run1_tc2_v2.dat',
    '../../data/temperature data/June 8/Run1_tc3_v2.dat',
    '../../data/temperature data/June 8/Run1_tc4_v2.dat',
]
# output files
OPT_FPATH = '../results/opt_test-params-newdata-heatingonly.dat'
SIM_FPATH = '../results/opt_test-sim-newdata-heatingonly.dat'

EXP_X_ARRAY = np.array(sorted([.33 - .01555 - .0725*n for n in range(4)]))

METHOD = explicit_diffusion_simple
HEATING_ONLY = True

TIME_STEP = .1
DIM_X = 66 + 1

U_0 = 303.
U_AMB = 297.
THERMAL_CONDUCTIVITY = 125.
SPECIFIC_HEAT = 380.
MASS_DENSITY = 8730.
CONVECTION_COEFF = 1.95
EMISSIVITY = .01
POWER = 10.
POWER2 = -10.
STOP_TIME = 1085.

ALL_PARAMS_DICT = dict(
    u_0=U_0,
    u_amb=U_AMB,
    thermal_conductivity=THERMAL_CONDUCTIVITY,
    specific_heat=SPECIFIC_HEAT,
    mass_density=MASS_DENSITY,
    convection_coeff=CONVECTION_COEFF,
    emissivity=EMISSIVITY,
    power=POWER,
    power2=POWER2,
    stop_time=STOP_TIME,
    perimeter=PERIMETER,
    area=AREA,
)

PARAMS_GUESS_DICT = dict(
    u_0=U_0,
    u_amb=U_AMB,
    thermal_conductivity=THERMAL_CONDUCTIVITY,
    specific_heat=SPECIFIC_HEAT,
    mass_density=MASS_DENSITY,
    convection_coeff=CONVECTION_COEFF,
    emissivity=EMISSIVITY,
    power=POWER,
    power2=POWER2,
    # stop_time=STOP_TIME,
)

PARAMS_BOUNDS_DICT = dict(
    u_0=(.95*U_0, 1.05*U_0),
    u_amb=(.9*U_AMB, 1.1*U_AMB),
    thermal_conductivity=(.8*THERMAL_CONDUCTIVITY, 1.2*THERMAL_CONDUCTIVITY),
    specific_heat=(.95*SPECIFIC_HEAT, 1.05*SPECIFIC_HEAT),
    mass_density=(.95*MASS_DENSITY, 1.05*MASS_DENSITY),
    convection_coeff=(0., 1000.),
    emissivity=(0., 1.),
    power=(0., 1000.),
    power2=(-1000., 1000.)
    # stop_time=(STOP_TIME-10, STOP_TIME+10),
)

ALL_PARAMS_DICT.update(PARAMS_GUESS_DICT)


def _lsq_func_simp(
        params_arr, const_params_dict, variable_params_keys,
        exp_time_array, exp_x_array, exp_temp_array,
        x_array, num_steps, time_step,
        finite_step_method, sim_fpath, iteration_fn,
):
    params_dict = make_params_dict(
        params_arr=params_arr, const_params_dict=const_params_dict,
        variable_params_keys=variable_params_keys
    )
    t_0 = params_dict['u_0']
    print('Iteration {:3}'.format(next(iteration_fn)))
    for k, v in sorted(params_dict.items()):
        if k in variable_params_keys:
            print('  {:24}:  {}'.format(k, v))
    del params_dict['u_0']
    sim = Simulation(
        time_step=time_step, x_array=x_array, t_0=t_0,
        finite_step_method=finite_step_method, params_dict=params_dict,
        boundary_conditions=None,
    )
    sim_temp_array = run_simulation_opt(
        simulation=sim, num_steps=num_steps, fpath=sim_fpath,
        exp_time_array=exp_time_array, exp_x_array=exp_x_array,
    )
    residuals = (sim_temp_array.flatten() - exp_temp_array.flatten())
    # return residuals
    m, n = sim_temp_array.shape
    # diff_arr = residuals * np.array(
    #     [np.exp(-floor(i/n)/(m-1)) for i in range(m*n)])
    diff_arr = residuals
    print('  Sum of squares per point (res) = {}'.format(
        np.dot(residuals, residuals)/(m*n)))
    print('  Sum of squares per point (opt) = {}'.format(
        np.dot(diff_arr, diff_arr)/(m*n)))
    return diff_arr


def optimize_diffusion_simp_parameters_with_bounds(
        params_guess_dict, params_bounds_dict, const_params_dict,
        exp_time_array, exp_x_array, exp_temp_array,
        x_array, num_steps, time_step, finite_step_method, sim_fpath,
        lsq_fn=_lsq_func_simp,
):
    params_guess = np.array([v for k, v in sorted(params_guess_dict.items())])
    if params_bounds_dict is None:
        bounds = None
    else:
        pgi = sorted(params_bounds_dict.items())
        lower_bounds = np.array([v[0] for k, v in pgi])
        upper_bounds = np.array([v[1] for k, v in pgi])
        bounds = (lower_bounds, upper_bounds)
    iter_fn = iteration()
    return least_squares(
        fun=lsq_fn, x0=params_guess, bounds=bounds, verbose=2,
        args=(
            const_params_dict, params_guess_dict.keys(),
            exp_time_array, exp_x_array, exp_temp_array,
            x_array, num_steps, time_step,
            finite_step_method, sim_fpath, iter_fn
        ),
    )


# script
if __name__ == '__main__':
    # make results directories
    opt_dpath = path.split(OPT_FPATH)[0]
    sim_dpath = path.split(SIM_FPATH)[0]
    if not path.exists(opt_dpath):
        makedirs(opt_dpath)
    if not path.exists(sim_dpath):
        makedirs(sim_dpath)
    # determine which parameters to hold constant
    const_keys = filter(
        lambda k: k not in PARAMS_GUESS_DICT, ALL_PARAMS_DICT.keys())
    const_params_dict0 = {k: ALL_PARAMS_DICT[k] for k in const_keys}
    # get experimental arrays
    exp_time_array0, exp_temp_array0 = get_exp_time_temp_arrays(
        dat_fpaths_list=DATA_FPATHS
    )
    # find index of last time before stop time
    if HEATING_ONLY:
        for time, ii in zip(exp_time_array0, range(len(exp_time_array0))):
            if time > STOP_TIME:
                exp_time_array0 = exp_time_array0[:ii]
                exp_temp_array0 = exp_temp_array0[:ii]
                break
    # get num_steps
    num_steps0 = ceil(exp_time_array0[-1] / TIME_STEP)
    print(num_steps0)
    # run optimization
    result = optimize_diffusion_simp_parameters_with_bounds(
        params_guess_dict=PARAMS_GUESS_DICT,
        params_bounds_dict=PARAMS_BOUNDS_DICT,
        const_params_dict=const_params_dict0,
        exp_time_array=exp_time_array0,
        exp_x_array=EXP_X_ARRAY,
        exp_temp_array=exp_temp_array0,
        x_array=np.linspace(MIN_X, MAX_X, DIM_X),
        num_steps=num_steps0, time_step=TIME_STEP,
        finite_step_method=METHOD,
        sim_fpath=SIM_FPATH,
    )
    # write results file
    params_array = result.x
    with open(OPT_FPATH, 'w') as fw:
        fw.write(str(datetime.now()) + '\n')
        fw.write('\n')
        fw.write('Optimization of 1-dimensional parameters\n')
        fw.write('\n')
        fw.write('Optimized parameters:\n')
        for param, val in zip(sorted(PARAMS_GUESS_DICT.keys()), params_array):
            fw.write('  {:24}= {}\n'.format(param, val))
        fw.write('\n')
        for name, item in result.items():
            if name == 'x':
                continue
            fw.write('{}:\n'.format(name))
            fw.write('{}\n'.format(item))
            fw.write('\n')
