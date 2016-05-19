%% Arduino measurements for ENPH 257
%% Dilyn Fullerton, 2016-05-18

%% Constants
PATH_ARDUINO = '/dev/tty.usbmodem1421';  % mac/unix
%% PATH_ARDUINO = 'COM3';  % pc
TIME = 60 * 20;
PERIOD = 0.5;
NUM_ITER = TIME / PERIOD;
PINS = ['A0'; 'A1'; 'A2'; 'A3';];
WRITE_FILES = ['../data/testA0.dat';
	       '../data/testA1.dat';
	       '../data/testA2.dat';
	       '../data/testA3.dat';
	      ];

%% Script
clearvars a;
a = arduino(PATH_ARDUINO, 'Uno');
%% record_data(a, PINS, WRITE_FILES, NUM_ITER, PERIOD);
manual_calibrate(a, PINS, WRITE_FILES);
