from __future__ import division, print_function
from datetime import datetime
import numpy as np
from scipy.optimize import leastsq
from simu1d import run_simulation_opt
from diffusion_1d import PARAMS_DICT
from diffusion_1d import DIM_X, MIN_X, MAX_X, T_0, T_SRC
from diffusion_1d import NUM_STEPS, TIME_STEP
from diffusion_1d import implicit_mod_diffusion
from bc_1d import get_bc_dirichlet


FPATH_SAVE = '../results/opt_test.txt'
EXP_X_ARRAY = np.array(sorted([.33 - .01555 - .0725*n for n in range(4)]))


def _make_params_dict(params_arr, variable_params_keys, const_params_dict):
    d = dict(const_params_dict)
    d.update({k: v for k, v in zip(sorted(variable_params_keys), params_arr)})
    return d


def _lsq_func(
        params_arr, const_params_dict, variable_params_keys,
        exp_time_array, exp_x_array, exp_temp_array,
        dim_x, min_x, max_x, t_0, num_steps, time_step,
        finite_step_method, boundary_conditions,
):
    params_dict = _make_params_dict(
        params_arr=params_arr, const_params_dict=const_params_dict,
        variable_params_keys=variable_params_keys
    )
    sim_temp_array = run_simulation_opt(
        exp_time_array=exp_time_array, exp_x_array=exp_x_array,
        dim_x=dim_x, min_x=min_x, max_x=max_x, t_0=t_0,
        num_steps=num_steps, time_step=time_step,
        finite_step_method=finite_step_method,
        boundary_conditions=boundary_conditions,
        params_dict=params_dict,
    )
    return sim_temp_array.flatten() - exp_temp_array.flatten()


def optimize_diffusion_parameters(
        params_guess_dict, const_params_dict,
        exp_time_array, exp_x_array, exp_temp_array,
        dim_x, min_x, max_x, t_0, num_steps, time_step,
        finite_step_method, boundary_conditions,
):
    params_guess = np.array([v for k, v in sorted(params_guess_dict.items())])
    return leastsq(
        func=_lsq_func, x0=params_guess, full_output=True,
        args=(
            const_params_dict, params_guess_dict.keys(),
            exp_time_array, exp_x_array, exp_temp_array,
            dim_x, min_x, max_x, t_0, num_steps, time_step,
            finite_step_method, boundary_conditions,
        ),
    )


def get_experimental_arrays(dat_fpaths_list):
    time_temp_lists = list()
    for fpath in dat_fpaths_list:
        time_list = list()
        temp_list = list()
        with open(fpath, 'r') as f:
            for line in f:
                time, temp = [float(x) for x in line.split()]
                time_list.append(time)
                temp_list.append(temp)
            time_temp_lists.append((time_list, temp_list))
    return time_temp_lists


# script
if __name__ == '__main__':
    params_guess_dict0 = {
        'thermal_conductivity': 1.0,
        'specific_heat': 1.0,
        'mass_density': 1.0,
        'porosity_air': 1.0,
        'velocity_air': 1.0,
        'emissivity': 1.0,
        'u_amb': 300.0,
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

    print(exp_time_array0)
    # result = optimize_diffusion_parameters(
    #     params_guess_dict=params_guess_dict0,
    #     const_params_dict=const_params_dict0,
    #     exp_time_array=exp_time_array0,
    #     exp_x_array=EXP_X_ARRAY,
    #     exp_temp_array=exp_temp_array0,
    #     dim_x=DIM_X, min_x=MIN_X, max_x=MAX_X,
    #     t_0=T_0, num_steps=NUM_STEPS, time_step=TIME_STEP,
    #     finite_step_method=implicit_mod_diffusion,
    #     boundary_conditions=get_bc_dirichlet(x0=T_SRC, x1=None)
    # )
    # x, cov, info, msg, ier = result
    # with open(FPATH_SAVE, 'w') as fw:
    #     fw.write(str(datetime.now()) + '\n')
    #     fw.write('\n')
    #     fw.write('Optimization of 1-dimensional parameters\n')
    #     fw.write('\n')
    #     fw.write('Optimized parameters:\n')
    #     for pk, v in zip(sorted(params_guess_dict0.keys()), x):
    #         fw.write('  {:16} = {}\n'.format(pk, v))
    #     fw.write('\n')
    #     fw.write('Covariance matrix:\n')
    #     for row in cov:
    #         fw.write('  ')
    #         for item in row:
    #             fw.write('{:16.8f}  '.format(item))
    #         fw.write('\n')
    #     fw.write('\n')
    #     fw.write('Info:\n')
    #     for k, v in info.items():
    #         fw.write('{}:\n  {}\n'.format(k, v))
    #     fw.write('\n')
    #     fw.write('Message:\n')
    #     fw.write('  {}\n'.format(msg))
    #     fw.write('\n')
    #     fw.write('Flag:\n')
    #     fw.write('  {}\n'.format(ier))
