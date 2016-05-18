%% Arduino measurements for ENPH 257
%% Dilyn Fullerton, 2016-05-18

%% Constants
PATH_ARDUINO = '/dev/tty.usbmodem1421';

%% Functions
function a = initialize_arduino(path)
  %% Check that path to Arduino exists
  clearvars a;
  if !(exist(path, 'file'))
    fprintf('Arduino not found at %s', path);
    a = 0;
  else
    a = arduino(path, 'Uno');
  end
end

function record_data(arduino, fpath, numiter, wait)
  file = fopen(fpath);
  i = 0;
  tic;
  while (i < numiter)
    t_now = toc;
    v = readVoltage(a, 'A0');
    fprintf('%16.8f  %16.8f\n', t_now, v);
    fprintf(file, '%16.8f  %16.8f\n', t_now, v);
    i = i + 1;
    pause(wait);
  end
  fclose(file);
end

%% Script
arduino = initialize_arduino(PATH_ARDUINO);
record_data(arduino, './test.dat', 1000, 0.5);

