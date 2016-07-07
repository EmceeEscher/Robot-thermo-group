import re
from os import path
from matplotlib import pyplot as plt, colors, cm
from mpl_toolkits.mplot3d import Axes3D
import numpy as np

FPATH_FIGURE = '../figures/june8_run1-fig.pdf'

FPATHS_LIST = [
    '../../data/temperature data/June 8/Run1_tc1.dat',
    '../../data/temperature data/June 8/Run1_tc2.dat',
    '../../data/temperature data/June 8/Run1_tc3.dat',
    '../../data/temperature data/June 8/Run1_tc4.dat',
]

# SIM_FPATH = '../results/June 8 - Run 1/june8_run1-tc2,3-sim.dat'
SIM_FPATH = '../results/june8_run1-sim.dat'


def _get_exp_datum(fpath):
    """For a given data file, returns (time_arr, temp_arr), where
    time_arr is the one-dimensional ordered array of times and temp_arr
    is the one-dimensional ordered array of temperatures
    """
    time_list = list()
    temp_list = list()
    with open(fpath, 'r') as fr:
        for line in fr:
            ldat = line.split()
            if len(ldat) == 0:
                continue
            else:
                time, temp_c = [float(x) for x in ldat]
                time_list.append(time)
                temp_list.append(temp_c + 273.15)
    return np.array(time_list), np.array(temp_list)


def get_exp_data(fpaths_list):
    """Makes an ordered list of items (time_arr, temp_arr) for each file
    in fpaths_list
    """
    return [_get_exp_datum(fpath) for fpath in fpaths_list]


def get_exp_data_matrix(exp_data):
    """Given a list of items (time_arr, temp_arr) such as that from
    _get_exp_data(), returns the first time array and the 2-dimensional
    array formed by combining the temp_arrays
    :param exp_data: list of items (time_arr, temp_arr)
    """
    time_arr = exp_data[0][0]
    temp_arrays = [dat[1] for dat in exp_data]
    temp_arr = np.vstack(temp_arrays).T
    return time_arr, temp_arr


def _get_sim_params_dict(fpath):
    params = dict()
    with open(fpath, 'r') as fr:
        for line in fr:
            if re.match('#.*=\s+[0-9]+\.', line):
                name, param = [s.strip('#').strip() for s in line.split('=')]
                params[name] = float(param)
            elif re.match('#.*=\s+[0-9]', line):
                name, param = line.split('=')
                params[name] = int(param)
            elif re.match('#.*=\s+', line):
                name, param = line.split('=')
                params[name] = param
            elif re.match('Step', line):
                break
    return params


def get_sim_data(fpath):
    """Parses the given *-sim.dat to retrieve (time_arr, temp_arr), where
    time_arr is a 1d array of times and temp_arr is a 2d array
    of temperatures, where each row corresponds to a time and each column
    corresponds to a time position
    """
    steps = [0]
    temp_list = list()
    temps_list = list()
    with open(fpath, 'r') as fr:
        while True:
            line = fr.readline()
            if re.match('\s*[0-9]+\s+[0-9]+\.', line):
                temp_list.append(float(line.split()[1]))
            elif len(temp_list) > 0:
                temps_list.append(np.array(temp_list))
                temp_list = list()
                if re.match('Step', line):
                    steps.append(int(line.split()[1]))
                elif not line:
                    break
    time_step = _get_sim_params_dict(fpath)['time step']
    return time_step * np.array(steps), np.array(temps_list)


def plot_experimental_data(fpaths_list):
    time_temp_arrays = get_exp_data(fpaths_list)
    plots = list()
    for time_temp_arrs, fpath in zip(time_temp_arrays, fpaths_list):
        time_arr, temp_arr = time_temp_arrs
        plots.append((time_arr, temp_arr, path.split(fpath)[1]))
    # make figure
    fig = plt.figure()
    ax = fig.add_subplot(111)
    lines = list()
    for x, y, label in plots:
        line = ax.plot(x, y, label=label)
        lines.append(line)
    plt.xlabel('Time (s)')
    plt.ylabel('Temperature (K)')
    plt.title('Temperature vs Time data')
    return lines


def plot_experimental_data_surface(fpaths_list):
    time_arr, temp_arr = get_exp_data_matrix(get_exp_data(fpaths_list))
    x_arr = np.arange(1, temp_arr.shape[1]+1)
    # make figure
    fig = plt.figure()
    ax = fig.gca(projection='3d')
    x_arr, time_arr = np.meshgrid(x_arr, time_arr)
    surf = ax.plot_surface(
        x_arr, time_arr, temp_arr, cmap='coolwarm',
        rstride=1, cstride=1,
        linewidth=0
    )
    ax.set_xlabel('Thermocouple')
    ax.set_ylabel('Time (s)')
    ax.set_zlabel('Temperature (K)')
    plt.title('Temperature vs. Time Surface Plot')
    return surf


def plot_simulation_data(fpath):
    # get data
    time_array, temp_array = get_sim_data(fpath)
    plots = list()
    for tc_dat, i in zip(temp_array.T, range(len(temp_array.T))):
        plots.append((time_array, tc_dat, 'Thermocouple {}'.format(i)))
    # make figure
    fig = plt.figure()
    ax = fig.add_subplot(111)
    lines = list()
    for x, y, lab in plots:
        lines.append(ax.plot(x, y, label=lab))
    plt.xlabel('Time (s)')
    plt.ylabel('Temperature (K)')
    plt.title('Temperature vs Time Data for Simulation')
    return lines


def plot_simulation_data_surface(fpath):
    # get data
    time_arr, temp_arr = get_sim_data(fpath)
    x_arr = np.arange(1, 1+temp_arr.shape[1])
    # make figure
    fig = plt.figure()
    ax = fig.gca(projection='3d')
    x_arr, time_arr = np.meshgrid(x_arr, time_arr)
    # reduce sizes
    surf = ax.plot_surface(
        x_arr, time_arr, temp_arr, cmap='coolwarm',
        rstride=1, cstride=1,
        linewidth=0
    )
    ax.set_xlabel('Thermocouple')
    ax.set_ylabel('Time (s)')
    ax.set_zlabel('Temperature (K)')
    plt.title('Temperature vs. Time Surface Plot for Simulation')
    return surf


def plot_experimental_and_simulation_data(sim_fpath, exp_fpaths):
    # get sim data
    time_array, temp_array = get_sim_data(sim_fpath)
    plots = list()
    for tc_dat, i in zip(temp_array.T, range(len(temp_array.T))):
        plots.append((time_array, tc_dat, 'Thermocouple {} fit'.format(i+1)))
    # get colors
    c_norm = colors.Normalize(vmin=0, vmax=len(plots)-1)
    scalar_map = cm.ScalarMappable(norm=c_norm, cmap=plt.get_cmap('jet'))
    # make figure
    fig = plt.figure()
    ax = fig.add_subplot(111)
    lines = list()
    for p, i in zip(plots, range(len(plots))):
        x, y, lab = p
        lines.append(
            ax.plot(x, y, '--', label=lab, color=scalar_map.to_rgba(i)))
    plt.xlabel('Time (s)')
    plt.ylabel('Temperature (K)')
    plt.title('Temperature vs Time Data for Simulation')
    # get exp data
    time_temp_arrays = get_exp_data(exp_fpaths)
    plots = list()
    for time_temp_arrs, fpath in zip(time_temp_arrays, exp_fpaths):
        time_arr, temp_arr = time_temp_arrs
        plots.append((time_arr, temp_arr, path.split(fpath)[1]))
    # make figure
    lines = list()
    for p, i in zip(plots, range(len(plots))):
        x, y, label = p
        line = ax.plot(x, y, label=label, color=scalar_map.to_rgba(i))
        lines.append(line)


# script
if __name__ == '__main__':
    # plot_experimental_data(fpaths_list=FPATHS_LIST)
    # plt.legend()
    # plt.show()

    # plot_experimental_data_surface(fpaths_list=FPATHS_LIST)
    # plt.show()

    # plot_simulation_data(fpath=SIM_FPATH)
    # plt.legend()
    # plt.show()

    # plot_simulation_data_surface(fpath=SIM_FPATH)
    # plt.show()
    plot_experimental_and_simulation_data(
        sim_fpath=SIM_FPATH, exp_fpaths=FPATHS_LIST)
    # plt.legend()
    plt.savefig(FPATH_FIGURE)
    plt.show()
