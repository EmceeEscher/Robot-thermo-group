%Making jpegs of temperatures
%f1 to f4 are the filenames for thermocouple data from TCs 1-4
function getTempGraphs(f1,f2,f3,f4)

[o,p] = convertToTemp(f1, '../data/calibration stuff/calibrate_tc1_May_25_Trial_3.dat');
[q,r] = convertToTemp(f2, '../data/calibration stuff/calibrate_tc2_May_25_Trial_3.dat');
[s,t] = convertToTemp(f3, '../data/calibration stuff/calibrate_tc3_May_25_Trial_3.dat');
[u,v] = convertToTemp(f4, '../data/calibration stuff/calibrate_tc4_May_25_Trial_3.dat');


figure

subplot(2,2,1)
plot(o,p)
title('Thermocouple 1')
xlabel('Time (seconds)')
ylabel('Temperature (°C)')

subplot(2,2,2)
plot(q,r)
title('Thermocouple 2')
xlabel('Time (seconds)')
ylabel('Temperature (°C)')

subplot(2,2,3)
plot(s,t)
title('Thermocouple 3')
xlabel('Time (seconds)')
ylabel('Temperature (°C)')

subplot(2,2,4)
plot(u,v)
title('Thermocouple 4')
xlabel('Time (seconds)')
ylabel('Temperature (°C)')