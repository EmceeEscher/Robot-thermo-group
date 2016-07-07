%% Arduino measurements for ENPH 257
%% Dilyn Fullerton, 2016-05-18

%% Constants
%% PATH_ARDUINO = '/dev/tty.usbmodem1421';  % mac/unix
PATH_ARDUINO = 'COM6';  % windows
TIME = 60 * 30;
PERIOD = 0.5;
NUM_ITER = TIME / PERIOD;
MAX_TIME = TIME;
PINS = ['A0'; 'A1'; 'A2'; 'A3';];
WRITE_FILES = [
	       '../data/calibration stuff/tc1_r1_June17.dat';
	       '../data/calibration stuff/tc2_r1_June17.dat';
	       '../data/calibration stuff/tc3_r1_June17.dat';
	       '../data/calibration stuff/tc4_r1_June17.dat';
	      ];

%% Script
clearvars a;
a = arduino(PATH_ARDUINO, 'Uno');
%% [t_arr, v_arr] = record_data(a, PINS, WRITE_FILES, NUM_ITER,...
%%	     MAX_TIME, PERIOD);
manual_calibrate(a, PINS, WRITE_FILES);
%% [x, y] = get_calibration('../data/calibrate.dat');
