function a = initialize_arduino(path)
  %% Check that path to Arduino exists
  clearvars a;
  if ~(exist(path, 'file'))
    a = 0;
  else
    a = arduino(path, 'Uno');
  end
end
