function record_data(a, pin, fpath, numiter, wait)
  fpath_write = fopen(fpath, 'w');
  fprintf('Beginning in...\n');
  for (i = 0:2)
    fprintf('%d\n', 3-i);
    pause(1);
  end
  tic;
  while (i < numiter)
    t_now = toc;
    v = readVoltage(a, pin);
    fprintf('%16.8f  %16.8f\n', t_now, v);
    fprintf(fpath_write, '%16.8f  %16.8f\n', t_now, v);
    pause(wait);
  end
  fclose(fpath_write);
end
