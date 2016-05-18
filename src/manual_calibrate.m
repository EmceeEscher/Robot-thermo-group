function manual_calibrate(a, fpath)
  fw = fopen(fpath, 'w');
  fprintf('Beginning calibration curve...\n');
  while (1)
    temp = input('\nEnter temperature (C) or -999 to stop:\n  ');
    v = readVoltage(a, 'A0');
    if (temp == -999)
      break
    else
      fprintf('  Temp = %16.8f  V = %16.8f\n', temp, v);
      fprintf(fw, '%16.8f  %16.8f\n', temp, v);
    end
  end
  fclose(fw);
  fprintf('\nOutput written to %s\n\n', fpath);
end
