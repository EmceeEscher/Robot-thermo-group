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
