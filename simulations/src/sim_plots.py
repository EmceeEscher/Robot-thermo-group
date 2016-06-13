from os import path
from matplotlib import pyplot as plt
from matplotlib import cm
from mpl_toolkits.mplot3d import Axes3D
import numpy as np


FPATHS_LIST = [
    '../../data/temperature data/June 8/Run1_tc{}.dat'.format(i)
    for i in range(1, 5)
]


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
    x_arr = np.arange(1, len(fpaths_list)+1)
    time_arr = np.array(time_list)[::2]
    temp_arr = np.array(temp_arr_list)[::2]
    # make figure
    fig = plt.figure()
    ax = fig.gca(projection='3d')
    time_arr, x_arr = np.meshgrid(x_arr, time_arr)
    surf = ax.plot_surface(x_arr, time_arr, temp_arr, cmap='coolwarm')
    ax.set_ylabel('Thermocouple')
    ax.set_xlabel('Time (s)')
    ax.set_zlabel('Temperature (K)')
    plt.title('Temperature vs. Time Surface Plot')
    return surf



# script
if __name__ == '__main__':
    plot_experimental_data(fpaths_list=FPATHS_LIST)
    plt.legend()
    plt.show()

    plot_experimental_data_surface(fpaths_list=FPATHS_LIST)
    plt.show()
