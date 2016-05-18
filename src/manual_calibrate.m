function manual_calibrate(a, pins, fpaths)
  [n, zzz] = size(fpaths);
  f = zeros(n);
  v = zeros(n);
  for i = 1:n
    f(i) = fopen(fpaths(i, :), 'w');
  end
  fprintf('Beginning calibration curve...\n');
  while (1)
    temp = input('\n\nEnter temperature (C) or -999 to stop:\n  ');
    for i = 1:n
      v(i) = readVoltage(a, pins(i, :));
    end
    if (temp == -999)
      break
    else
      fprintf('  Temp = %16.8f\n', temp);
      for i = 1:n
	fprintf('  V_%s = %16.8f\n', pins(i, :), v(i));
        fprintf(f(i), '%16.8f  %16.8f\n', temp, v(i));
      end
    end
  end
  for i = 1:n
    fclose(f(i));
    fprintf('\nOutput for pin %s written to %s\n\n', pins(i, :), f(i));
  end
end
