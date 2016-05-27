function manual_calibrate(a, pins, fpaths)
  [n, zzz] = size(fpaths);
  f = zeros(n, 1);
  v = zeros(n, 1);
  %% open files
  for (i = 1:n)
    f(i, 1) = fopen(fpaths(i, :), 'w');
  end
  %% do an initial read for all pins
  for (i = 1:n)
    readVoltage(a, pins(i, :));
  end
  %% begin calibration
  fprintf('Beginning calibration curve...\n');
  while (1)
    %% user inputs temperatire
    temp = input('\n\nEnter temperature (C) or -999 to stop:\n  ');
    %% get potential for each pin
    for (i = 1:n)
      v(i, 1) = readVoltage(a, pins(i, :));
    end
    if (temp == -999)
      break
    else
      fprintf('  Temp = %16.8f\n', temp);  % stdout
      for (i = 1:n)
	fprintf('  V_%s = %16.8f\n', pins(i, :), v(i, 1));  % stdout
        fprintf(f(i, 1), '%16.8f  %16.8f\n', temp, v(i, 1));  % file
      end
    end
  end
  %% close files
  for (i = 1:n)
    fclose(f(i));
    fprintf('Output for pin %s written to %s\n', pins(i, :), fpaths(i, :));
  end
end
