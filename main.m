%% Arduino measurements for ENPH 257
%% Dilyn Fullerton, 2016-05-18

%% Constants
PATH_ARDUINO = '/dev/tty.usbmodem1421';  % mac/unix
%% PATH_ARDUINO = 'COM3';  % pc

%% Script
arduino = initialize_arduino(PATH_ARDUINO);
if (arduino ~= 0)
  record_data(arduino, './test.dat', 1000, 0.5);
else
  fprintf('Arduino not found at %s\n', PATH_ARDUINO);
end
