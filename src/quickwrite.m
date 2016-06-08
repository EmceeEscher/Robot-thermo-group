function quickwrite(vFile,calFile,smthVFile,tFile)

%Enter all files as the path STARTING from in the data folder

[xp,yp] = get_calibration(['../data/' vFile]);

averageValues(xp,yp,['../data/' smthVFile]);

% [time,temp] = convertToTemp( ...
%     ['../data/' smthVFile], ['../data/' calFile], ['../data/' tFile]);

[time,temp] = convertToTemp( ...
    ['../data/' smthVFile], ['../data/calibration stuff/calibrate_tc1_May_25_Trial_3.dat'], ...
    ['../data/temperature data/June 1/FUCK']);

end