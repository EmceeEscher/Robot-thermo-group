function record_data(a, pins, fpaths, numiter, max_time, wait)
  [n, zzz] = size(fpaths);
  %% open files
  f = zeros(n);
  for (i = 1:n)
    f(i) = fopen(fpaths(i, :), 'w');
  end
  fprintf('Beginning data recording...\n');
  tic;
  t = 0
  while (i < numiter & t < max_time)
    i = i + 1;
    %% get data for each pin
    for (j = 1:n)
      t = toc;
      v = readVoltage(a, pins(j, :));
      fprintf('t = %16.8f  V_%s = %16.8f\n', t, pins(j, :), v);
      fprintf(f(j), '%16.8f  %16.8f\n', t, v);
    end
    pause(wait);
  end
  %% close files
  for (i = 1:n)
    fclose(f(i));
    fprintf('\nOutput for pin %s written to %s\n\n', pins(i, :), f(i));
  end
end
