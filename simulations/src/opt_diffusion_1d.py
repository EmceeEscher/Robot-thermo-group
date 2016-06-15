from datetime import datetime
from math import ceil
from os import path, makedirs
import numpy as np
from scipy.optimize import least_squares
from simu1d import run_simulation_opt, Simulation
from diffusion_1d import PARAMS_DICT
from diffusion_1d import DIM_X, MIN_X, MAX_X, T_0, DT_SRC, T_AMB
from diffusion_1d import THERMAL_CONDUCTIVITY
from diffusion_1d import SPECIFIC_HEAT
from diffusion_1d import MASS_DENSITY
from diffusion_1d import POROSITY_AIR
from diffusion_1d import VELOCITY_AIR
from diffusion_1d import EMISSIVITY
from diffusion_1d import STOP_TIME
from diffusion_1d import TIME_STEP
from diffusion_1d import implicit_mod_diffusion
from bc_1d import x0_d1_discontinuous
from bc_1d import BoundaryConditions


# input files
DATA_FPATHS = [
    '../../data/temperature data/June 6/Run1_tc1.dat',
    '../../data/temperature data/June 6/Run1_tc2.dat',
    '../../data/temperature data/June 6/Run1_tc3.dat',
    '../../data/temperature data/June 6/Run1_tc4.dat',
]
# output files
OPT_FPATH = '../results/opt_test-params.dat'
SIM_FPATH = '../results/opt_test-sim.dat'

EXP_X_ARRAY = np.array(sorted([.33 - .01555 - .0725*n for n in range(4)]))

params_guess_dict0 = {
    'thermal_conductivity': THERMAL_CONDUCTIVITY,
    'specific_heat': SPECIFIC_HEAT,
    'mass_density': MASS_DENSITY,
    'porosity_air': POROSITY_AIR,
    'velocity_air': VELOCITY_AIR,
    'emissivity': EMISSIVITY,
    'u_0': T_0,
    'u_amb': T_AMB,
    'du_src': DT_SRC,
}

params_bounds_dict0 = {
    'thermal_conductivity':
        (.5*THERMAL_CONDUCTIVITY, 1.5*THERMAL_CONDUCTIVITY),
    'specific_heat':
        (.9*SPECIFIC_HEAT, 1.1*SPECIFIC_HEAT),
    'mass_density':
        (.9*MASS_DENSITY, 1.1*MASS_DENSITY),
    'porosity_air':
        (0., 1.),
    'velocity_air':
        (0., 10.),
    'emissivity':
        (0., 1.),
    'u_0':
        (.9*T_0, 1.1*T_0),
    'u_amb':
        (.9*T_AMB, 1.1*T_AMB),
    'du_src':
        (0, np.inf),
}


def iteration():
    i = 0
    while True:
        yield i
        i += 1


def make_params_dict(params_arr, variable_params_keys, const_params_dict):
    d = dict(const_params_dict)
    d.update({k: v for k, v in zip(sorted(variable_params_keys), params_arr)})
    return d


def _lsq_func(
        params_arr, const_params_dict, variable_params_keys,
        exp_time_array, exp_x_array, exp_temp_array,
        x_array, num_steps, time_step,
        finite_step_method, sim_fpath, t_stop, iteration_fn
):
    params_dict = make_params_dict(
        params_arr=params_arr, const_params_dict=const_params_dict,
        variable_params_keys=variable_params_keys
    )
    du_src = params_dict['du_src']
    t_0 = params_dict['u_0']
    print('Iteration {:3}'.format(next(iteration_fn)))
    print('params_arr =\n{}'.format(params_arr))
    for k, v in sorted(params_dict.items()):
        print('  {:24}:  {}'.format(k, v))
    del params_dict['du_src']
    del params_dict['u_0']
    dx = (x_array[-1] - x_array[0]) / (len(x_array) - 1)
    bc = BoundaryConditions(
        x0_func=x0_d1_discontinuous(g_t=lambda t: du_src, dx=dx, t_stop=t_stop),
        x1_func=None,
        name='Heating stopped at {}'.format(t_stop)
    )
    sim = Simulation(
        time_step=time_step, x_array=x_array, t_0=t_0,
        finite_step_method=finite_step_method, params_dict=params_dict,
        boundary_conditions=bc
    )
    sim_temp_array = run_simulation_opt(
        simulation=sim, num_steps=num_steps, fpath=sim_fpath,
        exp_time_array=exp_time_array, exp_x_array=exp_x_array,
    )
    sq_sum = 0.0
    for st, et in zip(sim_temp_array.flatten(), exp_temp_array.flatten()):
        sq_sum += (st - et) ** 2
    print('  Sum of squares = {}'.format(sq_sum))
    return sim_temp_array.flatten() - exp_temp_array.flatten()


def optimize_diffusion_parameters_with_bounds(
        params_guess_dict, params_bounds_dict, const_params_dict,
        exp_time_array, exp_x_array, exp_temp_array,
        x_array, num_steps, time_step, finite_step_method, sim_fpath, t_stop,
        lsq_fn=_lsq_func,
):
    """Attempts to optimize the full convection-diffusion equation
    physical parameters based on a given initial guess, parameter boundaries,
    and other simulation parameters.
    :param params_guess_dict: dictionary of parameter guess values
    :param params_bounds_dict: dictionary of (low, high) limit tuples for
    each parameter in params_guess_dict
    :param const_params_dict: dictionary of constant parameters to be
    passed to finite_step_method
    :param exp_time_array: array of times for which data was recorded
    :param exp_x_array: array of x_values
    :param exp_temp_array: N x M array of temperatures for each time
    and x value, where the rows correspond to steps in time and the columns
    correspond to steps in x
    :param x_array: array of x values for the simulation
    :param num_steps: number of steps for simulation
    :param time_step: simulation time step
    :param finite_step_method: simulation step method
    :param sim_fpath: filepath to which to print final simulation results
    :param t_stop: time at which heating ceased
    :return: results of scipy.optimize.least_squares
    """
    # TODO: docstring
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
            finite_step_method, sim_fpath, t_stop, iter_fn
        ),
    )


def _get_experimental_arrays(dat_fpaths_list):
    """Given a list of data files (e.g. one for each thermocouple),
    constructs an ordered list of 2-tuples (time_list, temp_list), where
    each 2-tuple holds the time and temperature data for the corresponding
    file
    :param dat_fpaths_list: list of data file paths
    :return: list of the same length of dat_fpaths_list, containing 2-tuples
    (time_list, temp_list) for each file
    """
    time_temp_lists = list()
    for fpath in dat_fpaths_list:
        time_list = list()
        temp_list = list()
        with open(fpath, 'r') as f:
            for line in f:
                ldat = line.split()
                if len(ldat) > 0:
                    time, temp = [float(x) for x in ldat]
                    time_list.append(time)
                    temp_list.append(temp + 273.15)
            time_temp_lists.append((time_list, temp_list))
    return time_temp_lists


def _get_exp_time_temp_arrays(time_temp_lists):
    """Gets the time array and temp array from the given list of
    (time_list, temp_list) tuples associated with each data file.
    For the time array:
      Uses the first (time_list, temp_list) tuple in time_temp_list to
      define the time array, thus the dimensions of the time array will be
      N x 1, where N is the number of data points in each file
    For the temp array:
      Each row represents the state of all thermocouples at the associated
      time step. Thus the dimensions of the temp array will be
      N x M, where N is the number of data points in each file and M is the
      number of files.
    :param time_temp_lists: list of (time_list, temp_list) for each
    data file
    :return: array representation of the time_list associated with the first
    file
    """
    exp_temp_list = list()
    for temps in zip(*[x[1] for x in time_temp_lists]):
        exp_temp_list.append(np.array(temps))
    return np.array(time_temp_lists[0][0]), np.array(exp_temp_list)


def get_exp_time_temp_arrays(dat_fpaths_list):
    """Gets the time array and temp array from the given list of
    (time_list, temp_list) tuples associated with each data file.
    For the time array:
      Uses the first (time_list, temp_list) tuple in time_temp_list to
      define the time array, thus the dimensions of the time array will be
      N x 1, where N is the number of data points in each file
    For the temp array:
      Each row represents the state of all thermocouples at the associated
      time step. Thus the dimensions of the temp array will be
      N x M, where N is the number of data points in each file and M is the
      number of files.
    :param dat_fpaths_list: list of data filepaths (i.e. for each
    thermocouple associated with a particular data acquisition)
    :return: array representation of the time_list associated with the first
    file
    """
    return _get_exp_time_temp_arrays(_get_experimental_arrays(dat_fpaths_list))

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
        lambda k: k not in params_guess_dict0, PARAMS_DICT.keys())
    const_params_dict0 = {k: PARAMS_DICT[k] for k in const_keys}
    # get experimental arrays
    exp_time_array0, exp_temp_array0 = get_exp_time_temp_arrays(
        dat_fpaths_list=DATA_FPATHS
    )
    # get num_steps
    num_steps0 = ceil(exp_time_array0[-1] / TIME_STEP)
    # run optimization
    result = optimize_diffusion_parameters_with_bounds(
        params_guess_dict=params_guess_dict0,
        params_bounds_dict=params_bounds_dict0,
        const_params_dict=const_params_dict0,
        exp_time_array=exp_time_array0,
        exp_x_array=EXP_X_ARRAY,
        exp_temp_array=exp_temp_array0,
        x_array=np.linspace(MIN_X, MAX_X, DIM_X),
        num_steps=num_steps0, time_step=TIME_STEP,
        finite_step_method=implicit_mod_diffusion,
        sim_fpath=SIM_FPATH, t_stop=STOP_TIME,
    )
    # write results file
    params_array = result.x
    with open(OPT_FPATH, 'w') as fw:
        fw.write(str(datetime.now()) + '\n')
        fw.write('\n')
        fw.write('Optimization of 1-dimensional parameters\n')
        fw.write('\n')
        fw.write('Optimized parameters:\n')
        for param, val in zip(sorted(params_guess_dict0.keys()), params_array):
            fw.write('  {:24}= {}\n'.format(param, val))
        fw.write('\n')
        for name, item in result.items():
            if name == 'x':
                continue
            fw.write('{}:\n'.format(name))
            fw.write('{}\n'.format(item))
            fw.write('\n')
