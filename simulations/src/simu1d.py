from __future__ import division, print_function
from datetime import datetime
from math import floor
import numpy as np


def d1_matrix(n, diff=1):
    # TODO: docstring
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


def d2_matrix(n, diff=1):
    # TODO: docstring
    return np.dot(d1_matrix(n=n - diff, diff=diff), d1_matrix(n=n, diff=diff))


class FiniteStepMethod:
    def __init__(self, f, name):
        self._f = f
        self.name = name

    def __call__(self, *args, **kwargs):
        return self._f(*args, **kwargs)


def simulation(
        time_step, x_array, t_0, finite_step_method,
        boundary_conditions, params_dict
):
    # TODO: docstring
    # initialize points
    point_to_temp_map = {idx_x: t_0 for idx_x in range(len(x_array))}
    time = 0
    # begin finite differencing
    yield point_to_temp_map
    while True:
        point_to_temp_map = finite_step_method(
            point_to_temp_map=point_to_temp_map,
            time=time, x_array=x_array, dt=time_step,
            boundary_conditions=boundary_conditions, **params_dict
        )
        time += time_step
        yield point_to_temp_map


def run_simulation_f(
        fpath, verbose, write_period, dim_x, min_x, max_x, t_0,
        num_steps, time_step, finite_step_method, boundary_conditions,
        params_dict,
):
    # TODO: docstring
    x_array = np.linspace(min_x, max_x, dim_x)
    s = simulation(
        time_step=time_step, x_array=x_array,
        t_0=t_0, finite_step_method=finite_step_method,
        boundary_conditions=boundary_conditions, params_dict=params_dict
    )
    params_lines = [
        '   {} = {}{}\n'.format(k.replace('_', ' '), ' '*(25 - len(k)), v)
        for k, v in params_dict.items()]
    lines = [
        ' {}\n'.format(str(datetime.now())),
        '\n',
        ' Simulation of heat flow through 1-dimensional rod\n',
        '\n',
        ' Dimensions\n',
        '   dim x =                     {}\n'.format(dim_x),
        '   min x =                     {}\n'.format(min_x),
        '   max x =                     {}\n'.format(max_x),
        '\n',
        ' Thermodynamical parameters\n',
        '   initial temp =              {}\n'.format(t_0),
    ] + params_lines + [
        '\n',
        ' Boundary conditions:\n',
        '   method =                    {}\n'
        ''.format(boundary_conditions.name),
        '   x0 order =                  {}\n'
        ''.format(boundary_conditions.x0_order),
        '   x1 order =                  {}\n'
        ''.format(boundary_conditions.x1_order),
        '\n',
        ' Finite differencing\n',
        '   num steps =                 {}\n'.format(num_steps),
        '   time step =                 {}\n'.format(time_step),
        '   method =                    {}\n'.format(finite_step_method.name),
        '   write period =              {}\n'.format(write_period),
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


def mindiff(array, value):
    # TODO: docstring
    diff = abs(array[0] - value)
    idx = 0
    for x, i in zip(array, range(len(array))):
        if abs(x - value) < diff:
            diff = abs(x - value)
            idx = i
    return idx


def run_simulation_opt(
        exp_time_array, exp_x_array,
        dim_x, min_x, max_x, t_0, num_steps, time_step, finite_step_method,
        boundary_conditions, params_dict,
):
    # TODO: docstring
    x_array = np.linspace(min_x, max_x, dim_x)
    s = simulation(
        time_step=time_step, x_array=x_array,
        t_0=t_0, finite_step_method=finite_step_method,
        boundary_conditions=boundary_conditions, params_dict=params_dict
    )
    # get indices of positions and times closest to the experimental
    imp_x_idx_list = [mindiff(x_array, exp_x) for exp_x in exp_x_array]
    imp_t_idx_list = list()
    for exp_t in exp_time_array:
        t_idx_0 = floor(exp_t/time_step)
        md = mindiff([t_idx_0, t_idx_0 + time_step], exp_t)
        imp_t_idx_list.append(t_idx_0 + md)
    # get points
    step_to_temp_array = list()
    for point_to_temp_map, step in zip(s, range(num_steps+1)):
        if step in imp_t_idx_list:
            temp_list = list()
            for x_idx, temp in sorted(point_to_temp_map.items()):
                if x_idx in imp_x_idx_list:
                    temp_list.extend([temp] * imp_x_idx_list.count(x_idx))
            step_to_temp_array.extend(
                [np.array(temp_list)] * imp_t_idx_list.count(step))
    return np.array(step_to_temp_array)
