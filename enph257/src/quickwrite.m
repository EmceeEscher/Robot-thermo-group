function quickwrite(vFile,calFile,smthVFile,tFile)

%Enter all files as the path STARTING from in the data folder

[xp,yp] = get_calibration(['../data/' vFile]);

averageValues(xp,yp,['../data/' smthVFile]);

[time,temp] = convertToTemp( ...
    ['../data/' smthVFile], ['../data/' calFile], ['../data/' tFile]);

plot(time,temp);
end