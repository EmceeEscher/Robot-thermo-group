function [t_arr, v_arr] = record_data(a, pins, fpaths, numiter,...
				      max_time, wait)
  [n, zzz] = size(fpaths);
  %% open files
  f = zeros(n);
  for (j = 1:n)
    f(j) = fopen(fpaths(j, :), 'w');
  end
  %% do an initial read for all pins
  for (j = 1:n)
    readVoltage(a, pins(j, :));
  end
  %% begin recording
  fprintf('Beginning data recording...\n');
  tic;
  t = 0
  i = 0
  t_arr0 = [];
  v_arr0 = [];
  while (i < numiter & t < max_time)
    i = i + 1;
    %% get data for each pin
    fprintf('Iteration # %d\n', i);
    t_row = zeros(1, n);
    v_row = zeros(1, n);
    for (j = 1:n)
      t = toc;
      v = readVoltage(a, pins(j, :));
      fprintf('t = %16.8f  V_%s = %16.8f\n', t, pins(j, :), v);
      fprintf(f(j), '%16.8f  %16.8f\n', t, v);
      t_row(1, j) = t;
      v_row(1, j) = v;
    end
    t_arr0 = [t_arr0; t_row];
    v_arr0 = [v_arr0; v_row];
    pause(wait);
  end
  %% close files
  for (j = 1:n)
    fclose(f(j));
    fprintf('\nOutput for pin %s written to %s\n\n', pins(j, :), f(j));
  end
  %% return vectors
  t_arr = t_arr0;
  v_arr = v_arr0;
end
