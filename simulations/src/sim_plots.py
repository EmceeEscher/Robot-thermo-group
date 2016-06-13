import re
from os import path
from matplotlib import pyplot as plt
from matplotlib import cm
from mpl_toolkits.mplot3d import Axes3D
import numpy as np


FPATHS_LIST = [
    '../../data/temperature data/June 8/Run1_tc{}.dat'.format(i)
    for i in range(1, 5)
]

SIM_FPATH = '../results/June 6 - Run 2/opt_test-sim.dat'


def plot_experimental_data(fpaths_list):
    # read data
    plots = list()
    for fpath in fpaths_list:
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
        plots.append(
            (np.array(time_list), np.array(temp_list), path.split(fpath)[1]))
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
    # read data
    files = [open(fpath, 'r') for fpath in fpaths_list]
    time_list = list()
    temp_arr_list = list()
    for lines in zip(*files):
        ldats = list()
        for line in lines:
            ldats.append([float(x) for x in line.split()])
        for ldat in ldats:
            if len(ldat) == 0:
                break
        else:
            time_list.append(ldats[0][0])
            temp_arr_list.append(np.array([ldat[1]+273.15 for ldat in ldats]))
    for f in files:
        f.close()
    x_arr = np.arange(1, len(fpaths_list)+1)
    time_arr = np.array(time_list)
    temp_arr = np.array(temp_arr_list)
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


def _get_simulation_data(fpath):
    steps = list()
    temp_list = list()
    with open(fpath, 'r') as fr:
        while True:
            line = fr.readline()
            if re.match('\s*[0-9]+\s+[0-9]+\.', line):
                temp_list.append(float(line.split()[1]))
            elif len(temp_list) > 0:
                steps.append(np.array(temp_list))
                temp_list = list()
                if not line:
                    break
    return np.array(steps)


def _get_simulation_params_dict(fpath):
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


def plot_simulation_data(fpath):
    # get data
    temp_array = _get_simulation_data(fpath)
    time_step = _get_simulation_params_dict(fpath)['time step']
    time_array = time_step * np.arange(len(temp_array))
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
    temp_arr = _get_simulation_data(fpath)
    time_step = _get_simulation_params_dict(fpath)['time step']
    time_arr = time_step * np.arange(len(temp_arr))
    x_arr = np.arange(1, 1+temp_arr.shape[1])
    # make figure
    fig = plt.figure()
    ax = fig.gca(projection='3d')
    x_arr, time_arr = np.meshgrid(x_arr, time_arr)
    # reduce sizes
    x_arr = x_arr
    time_arr = time_arr
    temp_arr = temp_arr
    try:
        surf = ax.plot_surface(
            x_arr, time_arr, temp_arr, cmap='coolwarm',
            rstride=1, cstride=1,
            linewidth=0
        )
    except ValueError:
        print('x_arr shape= {}'.format(x_arr.shape))
        print('time_arr shape= {}'.format(time_arr.shape))
        print('temp_arr shape= {}'.format(temp_arr.shape))
        raise
    ax.set_xlabel('Thermocouple')
    ax.set_ylabel('Time (s)')
    ax.set_zlabel('Temperature (K)')
    plt.title('Temperature vs. Time Surface Plot for Simulation')
    return surf


# script
if __name__ == '__main__':
    # plot_experimental_data(fpaths_list=FPATHS_LIST)
    # plt.legend()
    # plt.show()
    #
    plot_experimental_data_surface(fpaths_list=FPATHS_LIST)
    plt.show()
    #
    # plot_simulation_data(fpath=SIM_FPATH)
    # plt.legend()
    # plt.show()

    plot_simulation_data_surface(fpath=SIM_FPATH)
    plt.show()