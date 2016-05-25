function [x, y] = get_calibration(fpath)
  %% open file
  fr = fopen(fpath, 'r');
  format_spec = '%f %f';
  size = [2 Inf];
  A = fscanf(fr, format_spec, size);
  fclose(fr);
  x = A(1, :);
  y = A(2, :);
