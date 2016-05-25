%% Arduino measurements for ENPH 257
%% Dilyn Fullerton, 2016-05-18

%% Constants
PATH_ARDUINO = '/dev/tty.usbmodem1421';  % mac/unix
%% PATH_ARDUINO = 'COM3';  % windows
TIME = 60 * 20;
PERIOD = 0.5;
NUM_ITER = TIME / PERIOD;
MAX_TIME = TIME;
PINS = ['A0'; 'A1'; 'A2'; 'A3';];
WRITE_FILES = [
	       '../data/calibrate_tc1.dat';
	       '../data/calibrate_tc2.dat';
	       '../data/calibrate_tc3.dat';
	       '../data/calibrate_tc4.dat';
	      ];

%% Script
clearvars a;
a = arduino(PATH_ARDUINO, 'Uno');
%% record_data(a, PINS, WRITE_FILES, NUM_ITER, MAX_TIME, PERIOD);
%% manual_calibrate(a, PINS, WRITE_FILES);
%% [x, y] = get_calibration('../data/calibrate.dat');
