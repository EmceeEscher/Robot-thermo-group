function quickwrite(vFile,calFile,smthVFile,tFile)

%Enter all files as the path STARTING from in the data folder

[x,y] = get_calibration(['../data/' vFile]);

averageValues(x,y,['../data/' smthVFile]);

[time,temp] = convertToTemp( ...
    ['../data/' smthVFile], ['../data/' calFile], ['../data/' tFile])

end