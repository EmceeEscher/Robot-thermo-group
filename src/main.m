%% Arduino measurements for ENPH 257
%% Dilyn Fullerton, 2016-05-18

%% Constants
PATH_ARDUINO = '/dev/tty.usbmodem1421';  % mac/unix
%% PATH_ARDUINO = 'COM3';  % windows
TIME = 60 * 20;
PERIOD = 0.5;
NUM_ITER = TIME / PERIOD;
PINS = ['A0';];
WRITE_FILES = ['../data/calibrate_tc1.dat';
	      ];

%% Script
clearvars a;
a = arduino(PATH_ARDUINO, 'Uno');
%% record_data(a, PINS, WRITE_FILES, NUM_ITER, PERIOD);
manual_calibrate(a, PINS, WRITE_FILES);
