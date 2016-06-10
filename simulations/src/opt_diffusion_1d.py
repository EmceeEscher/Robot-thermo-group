from datetime import datetime
import numpy as np
from scipy.optimize import leastsq
from scipy.optimize import least_squares
from simu1d import run_simulation_opt
from diffusion_1d import PARAMS_DICT
from diffusion_1d import DIM_X, MIN_X, MAX_X, T_0, T_SRC, T_AMB
from diffusion_1d import THERMAL_CONDUCTIVITY
from diffusion_1d import SPECIFIC_HEAT
from diffusion_1d import MASS_DENSITY
from diffusion_1d import POROSITY_AIR
from diffusion_1d import VELOCITY_AIR
from diffusion_1d import EMISSIVITY
from diffusion_1d import STOP_TIME
from diffusion_1d import NUM_STEPS, TIME_STEP
from diffusion_1d import implicit_mod_diffusion
from bc_1d import x0_d0_discontinuous
from bc_1d import BoundaryConditions


OPT_FPATH = '../results/opt_test-params.dat'
SIM_FPATH = '../results/opt_test-sim.dat'
EXP_X_ARRAY = np.array(sorted([.33 - .01555 - .0725*n for n in range(4)]))


def _iteration():
    i = 0
    while True:
        yield i
        i += 1


def _make_params_dict(params_arr, variable_params_keys, const_params_dict):
    d = dict(const_params_dict)
    d.update({k: v for k, v in zip(sorted(variable_params_keys), params_arr)})
    return d


def _lsq_func(
        params_arr, const_params_dict, variable_params_keys,
        exp_time_array, exp_x_array, exp_temp_array,
        dim_x, min_x, max_x, num_steps, time_step,
        finite_step_method, sim_fpath, t_stop, iteration_fn
):
    params_dict = _make_params_dict(
        params_arr=params_arr, const_params_dict=const_params_dict,
        variable_params_keys=variable_params_keys
    )
    u_src = params_dict['u_src']
    t_0 = params_dict['u_0']
    print('Iteration {:3}'.format(next(iteration_fn)))
    print('params_arr =\n{}'.format(params_arr))
    for k, v in sorted(params_dict.items()):
        print('  {:24}:  {}'.format(k, v))
    del params_dict['u_src']
    del params_dict['u_0']
    bc = BoundaryConditions(
        x0_func=x0_d0_discontinuous(f_t=lambda t: u_src, t_stop=t_stop),
        x1_func=None,
        name='Heating stopped at {}'.format(t_stop)
    )
    sim_temp_array = run_simulation_opt(
        exp_time_array=exp_time_array, exp_x_array=exp_x_array,
        dim_x=dim_x, min_x=min_x, max_x=max_x, t_0=t_0,
        num_steps=num_steps, time_step=time_step,
        finite_step_method=finite_step_method,
        boundary_conditions=bc,
        params_dict=params_dict,
        fpath=sim_fpath,
    )
    sq_sum = 0.0
    for st, et in zip(sim_temp_array.flatten(), exp_temp_array.flatten()):
        sq_sum += (st - et) ** 2
    print('  Sum of squares = {}'.format(sq_sum))
    return sim_temp_array.flatten() - exp_temp_array.flatten()


def optimize_diffusion_parameters(
        params_guess_dict, const_params_dict,
        exp_time_array, exp_x_array, exp_temp_array,
        dim_x, min_x, max_x, num_steps, time_step,
        finite_step_method, sim_fpath, t_stop,
):
    # TODO: docstring
    params_guess = np.array([v for k, v in sorted(params_guess_dict.items())])
    iter_fn = _iteration()
    return leastsq(
        func=_lsq_func, x0=params_guess, full_output=True,
        args=(
            const_params_dict, params_guess_dict.keys(),
            exp_time_array, exp_x_array, exp_temp_array,
            dim_x, min_x, max_x, num_steps, time_step,
            finite_step_method, sim_fpath, t_stop, iter_fn
        ),
    )


def optimize_diffusion_parameters_with_bounds(
        params_guess_dict, params_bounds_dict, const_params_dict,
        exp_time_array, exp_x_array, exp_temp_array,
        dim_x, min_x, max_x, num_steps, time_step,
        finite_step_method, sim_fpath, t_stop,
):
    # TODO: docstring
    params_guess = np.array([v for k, v in sorted(params_guess_dict.items())])
    if params_bounds_dict is None:
        bounds = None
    else:
        pgi = sorted(params_bounds_dict.items())
        lower_bounds = np.array([v[0] for k, v in pgi])
        upper_bounds = np.array([v[1] for k, v in pgi])
        bounds = (lower_bounds, upper_bounds)
    iter_fn = _iteration()
    return least_squares(
        fun=_lsq_func, x0=params_guess, bounds=bounds, verbose=2,
        args=(
            const_params_dict, params_guess_dict.keys(),
            exp_time_array, exp_x_array, exp_temp_array,
            dim_x, min_x, max_x, num_steps, time_step,
            finite_step_method, sim_fpath, t_stop, iter_fn
        ),
    )


def get_experimental_arrays(dat_fpaths_list):
    # TODO: docstring
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


# script
if __name__ == '__main__':
    params_guess_dict0 = {
        'thermal_conductivity': THERMAL_CONDUCTIVITY,
        'specific_heat': SPECIFIC_HEAT,
        'mass_density': MASS_DENSITY,
        'porosity_air': POROSITY_AIR,
        'velocity_air': VELOCITY_AIR,
        'emissivity': EMISSIVITY,
        'u_0': T_0,
        'u_amb': T_AMB,
        'u_src': T_SRC,
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
        'u_src':
            (.5*T_SRC, 1.5*T_SRC),
    }
    const_keys = filter(
        lambda k: k not in params_guess_dict0, PARAMS_DICT.keys())
    const_params_dict0 = {k: PARAMS_DICT[k] for k in const_keys}
    time_temp_arrays = get_experimental_arrays(
        dat_fpaths_list=[
            '../../data/temperature data/June 6/Run1_tc{}.dat'.format(i)
            for i in range(1, 5)
        ]
    )
    exp_time_array0 = np.array(time_temp_arrays[0][0])
    exp_temp_list = list()
    for temps in zip(*[x[1] for x in time_temp_arrays]):
        exp_temp_list.append(np.array(temps))
    exp_temp_array0 = np.array(exp_temp_list)
    result = optimize_diffusion_parameters_with_bounds(
        params_guess_dict=params_guess_dict0,
        params_bounds_dict=params_bounds_dict0,
        const_params_dict=const_params_dict0,
        exp_time_array=exp_time_array0,
        exp_x_array=EXP_X_ARRAY,
        exp_temp_array=exp_temp_array0,
        dim_x=DIM_X, min_x=MIN_X, max_x=MAX_X,
        num_steps=NUM_STEPS, time_step=TIME_STEP,
        finite_step_method=implicit_mod_diffusion,
        sim_fpath=SIM_FPATH, t_stop=STOP_TIME,
    )
    x = result.x
    with open(OPT_FPATH, 'w') as fw:
        fw.write(str(datetime.now()) + '\n')
        fw.write('\n')
        fw.write('Optimization of 1-dimensional parameters\n')
        fw.write('\n')
        fw.write('Optimized parameters:\n')
        for param, val in zip(sorted(params_guess_dict0.keys()), x):
            fw.write('  {:24}= {}\n'.format(param, val))
        fw.write('\n')
        for name, item in result.items():
            if name == 'x':
                continue
            fw.write('{}:\n'.format(name))
            fw.write('{}\n'.format(item))
            fw.write('\n')
