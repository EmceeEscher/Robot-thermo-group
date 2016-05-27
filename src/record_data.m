function [t_arr, v_arr] = record_data(a, pins, fpaths, numiter,...
				      max_time, wait)
  [n, zzz] = size(fpaths);
  %% open files
  f = zeros(n);
  for (i = 1:n)
    f(i) = fopen(fpaths(i, :), 'w');
  end
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
    for (j = 1:n)
      t = toc;
      v = readVoltage(a, pins(j, :));
      fprintf('t = %16.8f  V_%s = %16.8f\n', t, pins(j, :), v);
      fprintf(f(j), '%16.8f  %16.8f\n', t, v);
      t_arr0 = [t_arr0; t];
      v_arr0 = [v_arr0; v];
    end
    pause(wait);
  end
  %% close files
  for (i = 1:n)
    fclose(f(i));
    fprintf('\nOutput for pin %s written to %s\n\n', pins(i, :), f(i));
  end
  %% return vectors
  t_arr = t_arr0;
  v_arr = v_arr0;
end
