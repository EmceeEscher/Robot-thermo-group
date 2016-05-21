from __future__ import print_function, division
from matplotlib import pyplot as plt
from scipy.optimize import leastsq
import numpy as np

# Constants
FPATH_CALIBRATE = '../data/calibrate_tc1.dat'

# Parse file
points = list()
with open(FPATH_CALIBRATE) as fr:
    for line in fr:
        point = tuple([float(x) for x in line.split()])
        points.append(point)

# Put points into arrays
points = sorted(points)
x_pts = [p[0] for p in points]
y_pts = [p[1] for p in points]
x_array = np.array(x_pts)
y_array = np.array(y_pts)


# Fit to points
def linear(x, params):
    a, b = params[:2]
    return a * x + b

fit_function = linear


def lsq(params, fitfn, x_arr, y_arr):
    y_fit = np.empty_like(y_arr)
    for i in range(len(y_fit)):
        y_fit[i] = fitfn(x_arr[i], params)
    return y_fit - y_arr

result = leastsq(
    func=lsq, x0=[1.0, 1.0], args=(fit_function, x_array, y_array))
params = result[0]
print('Fit params = {}'.format(params))

# Make fit array
yfit_array = np.empty_like(y_array)
for i in range(len(yfit_array)):
    yfit_array[i] = fit_function(x_array[i], params)

# Plot points
plt.plot(x_array, y_array, '-')
plt.plot(x_array, yfit_array, '--')
plt.xlabel('Temperature (C)')
plt.ylabel('Potential (V)')
plt.title('Calibration curve')

# Show plot
plt.show()
