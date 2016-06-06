from __future__ import print_function, division
from matplotlib import pyplot as plt
from scipy.optimize import leastsq
import numpy as np

# CONSTANTS
FPATH_CALIBRATE = '../data/calibrate_tc1.dat'


# FIT FUNCTIONS
def linear(x, params):
    a, b = params[:2]
    return a * x + b


# FUNCTIONS
def get_data(fpath):
    points = list()
    with open(fpath) as fr:
        for line in fr:
            point = tuple([float(x) for x in line.split()])
            points.append(point)
    # Put points into arrays
    points = sorted(points)
    x_pts = [p[0] for p in points]
    y_pts = [p[1] for p in points]
    return np.array(x_pts), np.array(y_pts)


def _lsq(params, fitfn, x_arr, y_arr):
    y_fit = np.empty_like(y_arr)
    for i in range(len(y_fit)):
        y_fit[i] = fitfn(x_arr[i], params)
    return y_fit - y_arr


def get_optimized_parameters(fit_function, x_array, y_array):
    result = leastsq(
        func=_lsq, x0=[1.0, 1.0], args=(fit_function, x_array, y_array))
    params = result[0]
    yfit_array = np.empty_like(y_array)
    for i in range(len(yfit_array)):
        yfit_array[i] = fit_function(x_array[i], params)
    return params, yfit_array


# SCRIPT
if __name__ == '__main__':
    # get data
    x_array, y_array = get_data(fpath=FPATH_CALIBRATE)

    # get fit parameters
    params, yfit_array = get_optimized_parameters(
        fit_function=linear, x_array=x_array, y_array=y_array)
    print('Fit params = {}'.format(params))

    # plot points
    plt.plot(x_array, y_array, '-')
    plt.plot(x_array, yfit_array, '--')
    plt.xlabel('Temperature (C)')
    plt.ylabel('Potential (V)')
    plt.title('Calibration curve')

    # show plot
    plt.show()
