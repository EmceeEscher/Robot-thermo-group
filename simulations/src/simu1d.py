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


def simulation_fn(
        time_step, x_array, t_0, finite_step_method, params_dict,
        boundary_conditions,
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


class Simulation:
    def __init__(
            self, time_step, x_array, t_0, finite_step_method, params_dict,
            boundary_conditions
    ):
        self.dt = time_step,
        self.x_array = x_array
        self.u_0 = t_0
        self.step_method = finite_step_method
        self.params_dict = params_dict
        self.bc = boundary_conditions
        self._sim = simulation_fn(
            time_step=self.dt, x_array=self.x_array, t_0=self.u_0,
            finite_step_method=self.step_method, params_dict=self.params_dict,
            boundary_conditions=self.bc,
        )

    def __iter__(self):
        return self

    def __next__(self):
        return next(self._sim)

    def description_lines(self):
        params_lines = [
            '   {} = {}{}\n'.format(k.replace('_', ' '), ' '*(25 - len(k)), v)
            for k, v in self.params_dict.items()]
        return [
            'Simulation of heat flow through 1-dimensional rod\n',
            '\n',
            'Dimensions\n',
            '  dim x =                  {}\n'.format(len(self.x_array)),
            '  min x =                  {}\n'.format(self.x_array[0]),
            '  max x =                  {}\n'.format(self.x_array[-1]),
            '\n',
            'Thermodynamical parameters\n',
            '  initial temp =           {}\n'.format(self.u_0),
        ] + params_lines + [
            '\n',
            ' Boundary conditions:\n',
            '   method =                    {}\n'
            ''.format(self.bc.name),
            '   x0 order =                  {}\n'
            ''.format(self.bc.x0_order),
            '   x1 order =                  {}\n'
            ''.format(self.bc.x1_order),
            '\n',
            ' Finite differencing\n',
            '   time step =                 {}\n'.format(self.dt),
            '   method =                    {}\n'.format(self.step_method.name),
            '\n',
        ]


def run_simulation_f(
        simulation, num_steps, fpath, verbose, write_period,
):
    """Runs the simulation based on the given Simulation object with the
    given number of steps and prints the output to fpath.
    :param simulation: Simulation object to iterate over
    :param num_steps: number of steps for simulation
    :param fpath: file in which to print results
    :param verbose: if true, print progress to stdout
    :param write_period: number of steps for each write. For example, if 10
    were given, only every tenth step would be written to fpath
    """
    lines = simulation.description_lines()
    if verbose:
        print(''.join(lines), end='\n')
    with open(fpath, 'w') as f:
        f.write('# ' + str(datetime.now()) + '\n')
        f.write('\n')
        for line in lines:
            f.write('# ' + line)
        for point_to_temp_map, step in zip(simulation, range(num_steps+1)):
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


def get_closest_time_and_x_indices(
        x_array, time_step, exp_x_array, exp_time_array):
    """From the given simulation x_array and time_step and the experimental
    x_array (actual positions of holes) and time_array (times for which
    data was gathered), determine the index arrays of the closest
    simulation x and time values to the experimental
    :param x_array: simulation array of x points
    :param time_step: simulation time step
    :param exp_x_array: positions for which data was collected
    :param exp_time_array: times for which data was collected
    :return: (t_idx_list, x_idx_list), the indices of the x values and time
    values in the simulation that are closest to the experimental ones.
    These lists will be the same size as the given exp_time_array and
    exp_x_array.
    """
    imp_x_idx_list = [mindiff(x_array, exp_x) for exp_x in exp_x_array]
    imp_t_idx_list = list()
    for exp_t in exp_time_array:
        t_idx_0 = floor(exp_t/time_step)
        md = mindiff([t_idx_0, t_idx_0 + time_step], exp_t)
        imp_t_idx_list.append(t_idx_0 + md)
    return imp_t_idx_list, imp_x_idx_list


def run_simulation_opt(
        simulation, num_steps, exp_time_array, exp_x_array, fpath,
):
    """Given a simulation, a number of steps, and the time and x arrays for
    which experimental data was gathered, returns an array of simulation
    temperatures where each row in the array corresponds to a distinct time
    step for which there is data and each column of the array corresponds
    to a distinct x position for which there is data
    :param simulation: Simulation object
    :param num_steps: number of steps to run simulation
    :param exp_time_array: ordered array of times for which there is data
    :param exp_x_array: ordered array of x values for which there is data
    :param fpath: path to write result of simulation
    :return: N x M array of temperatures where N corresponds to the number
    of times in exp_time_array and M corresponds to the number of x values
    in exp_x_array
    """
    # get indices of positions and times closest to the experimental
    imp_t_idx_list, imp_x_idx_list = get_closest_time_and_x_indices(
        x_array=simulation.x_array, time_step=simulation.dt,
        exp_time_array=exp_time_array, exp_x_array=exp_x_array,
    )
    # get points
    step_to_temp_array = list()
    lines = simulation.description_lines()
    with open(fpath, 'w') as fw:
        fw.write('# ' + str(datetime.now()) + '\n')
        fw.write('\n')
        for line in lines:
            fw.write('# ' + line)
        for point_to_temp_map, step in zip(simulation, range(num_steps+1)):
            if step in imp_t_idx_list:
                fw.write('Step= {}\n'.format(step))
                temp_list = list()
                for x_idx, temp in sorted(point_to_temp_map.items()):
                    if x_idx in imp_x_idx_list:
                        fw.write('{:8}  {:16.8f}\n'.format(x_idx, temp))
                        temp_list.extend([temp] * imp_x_idx_list.count(x_idx))
                step_to_temp_array.extend(
                    [np.array(temp_list)] * imp_t_idx_list.count(step))
    return np.array(step_to_temp_array)
