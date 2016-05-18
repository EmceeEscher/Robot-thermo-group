%% Arduino measurements for ENPH 257
%% Dilyn Fullerton, 2016-05-18

%% Constants
PATH_ARDUINO = '/dev/tty.usbmodem1421';  % mac/unix
%% PATH_ARDUINO = 'COM3';  % pc
TIME = 60 * 20;
PERIOD = 0.5;
NUM_ITER = TIME / PERIOD;
WRITE_FILE = '../data/calibrate.dat';

%% Script
clearvars a;
a = arduino(PATH_ARDUINO, 'Uno');
%% record_data(a, WRITE_FILE, NUM_ITER, PERIOD);
manual_calibrate(a, WRITE_FILE);
