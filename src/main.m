%% Arduino measurements for ENPH 257
%% Dilyn Fullerton, 2016-05-18

%% Constants
%% PATH_ARDUINO = '/dev/tty.usbmodem1421';  % mac/unix
PATH_ARDUINO = 'COM6';  % windows
TIME = 60 * 60;
PERIOD = 0.5;
NUM_ITER = TIME / PERIOD;
MAX_TIME = TIME;
PINS = ['A0'; 'A1'; 'A2'; 'A3';];
WRITE_FILES = [
	       '../data/voltage data/June 13/Run2_tc1_June13.dat';
	       '../data/voltage data/June 13/Run2_tc2_June13.dat';
	       '../data/voltage data/June 13/Run2_tc3_June13.dat';
	       '../data/voltage data/June 13/Run2_tc4_June13.dat';
	      ];

%% Script
clearvars a;
a = arduino(PATH_ARDUINO, 'Uno');
[t_arr, v_arr] = record_data(a, PINS, WRITE_FILES, NUM_ITER,...
	     MAX_TIME, PERIOD);
%% manual_calibrate(a, PINS, WRITE_FILES);
%% [x, y] = get_calibration('../data/calibrate.dat');
