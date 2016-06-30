from math import ceil
import numpy as np
from simu1d import Simulation, run_simulation_opt
from opt_diffusion_1d import get_exp_time_temp_arrays
from diffusion_1d import PERIMETER, AREA
from diffusion_1d_v3 import explicit_diffusion_simple


DATA_FPATHS = [
    '../../data/temperature data/June 3/Run2_tc2_June3.dat',
    '../../data/temperature data/June 3/Run2_tc3_June3.dat',
]

AVG_SIM_FPATH = '../results/June 3 - Run 2/june3_run2-tc2,3-AVG-sim.dat'

TIME_STEP = .25

EXP_X_ARRAY = np.array([
    # .09695,
    .16945,
    .24195,
    # .31445,
])

EMISSIVITY = .9091
MASS_DENSITY = 8520.07
SPECIFIC_HEAT = 370.5488
THERMAL_CONDUCTIVITY = 100.3

# update these
CONVECTION_COEFF = 6.6296

STOP_TIME = 900.
POWER_HEAT = 15.2304
POWER_COOL = -0.1991
U_0 = 299.6723
U_AMB = 299.9362

PARAMS_DICT = dict(
    u_amb=U_AMB,
    k=THERMAL_CONDUCTIVITY,
    c=SPECIFIC_HEAT,
    rho=MASS_DENSITY,
    k_c=CONVECTION_COEFF,
    emiss=EMISSIVITY,
    power_heat=POWER_HEAT,
    power_cool=POWER_COOL,
    stop_time=STOP_TIME,
    perim=PERIMETER,
    area=AREA,
)

# script
if __name__ == '__main__':
    # get experimental arrays
    exp_time_array0, exp_temp_array0 = get_exp_time_temp_arrays(
        dat_fpaths_list=DATA_FPATHS)

    # get num steps
    num_steps0 = ceil(exp_time_array0[-1] / TIME_STEP)

    # run simulation
    sim = Simulation(
        time_step=TIME_STEP, x_array=EXP_X_ARRAY, t_0=U_0,
        finite_step_method=explicit_diffusion_simple, params_dict=PARAMS_DICT,
        boundary_conditions=None,
    )
    run_simulation_opt(
        simulation=sim, num_steps=num_steps0,
        exp_time_array=exp_time_array0, exp_x_array=EXP_X_ARRAY,
        fpath=AVG_SIM_FPATH,
    )
