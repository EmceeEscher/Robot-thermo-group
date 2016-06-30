from sim_plots import get_exp_data_matrix, get_exp_data, get_sim_data
from scipy.stats import linregress


EXP_FPATHS = [
    '../../data/temperature data/June 3/Run2_tc2_June3.dat',
    '../../data/temperature data/June 3/Run2_tc3_June3.dat',
]
SIM_FPATH = '../results/June 3 - Run 2/june3_run2-tc2,3-AVG-sim.dat'
REG_FPATH = '../results/June 3 - Run 2/june3_run2-tc2,3-AVG-regress.dat'


def compute_regression(exp_fpaths_list, sim_fpath):
    exp_temp_arr_2d = get_exp_data_matrix(get_exp_data(exp_fpaths_list))[1]
    sim_temp_arr_2d = get_sim_data(sim_fpath)[1]
    return linregress(sim_temp_arr_2d.flatten(), exp_temp_arr_2d.flatten())


# script
if __name__ == '__main__':
    reg_result = compute_regression(EXP_FPATHS, SIM_FPATH)
    with open(REG_FPATH, 'w') as fw:
        fw.write('Linear regression results for {}\n'.format(SIM_FPATH))
        m, b, r_value, p_value, stderr = reg_result
        fw.write('Slope:             {:16.8f}\n'.format(m))
        fw.write('Intercept:         {:16.8f}\n'.format(b))
        fw.write('Correlation coeff: {:16.8f}\n'.format(r_value))
        fw.write('R-squared:         {:16.8f}\n'.format(r_value**2))
        fw.write('P-value:           {:16.8f}\n'.format(p_value))
        fw.write('Standard err:      {:16.8f}\n'.format(stderr))
