%Making jpegs of voltages
%f1 to f4 are the filenames for thermocouple data from TCs 1-4
function getVoltageGraphs(f1,f2,f3,f4)

[o,p] = get_calibration(f1);
[q,r] = get_calibration(f2);
[s,t] = get_calibration(f3);
[u,v] = get_calibration(f4);

figure

subplot(2,2,1)
plot(o,p)
title('Thermocouple 1')
xlabel('Time (seconds)')
ylabel('Voltage (volts)')

subplot(2,2,2)
plot(q,r)
title('Thermocouple 2')
xlabel('Time (seconds)')
ylabel('Voltage (volts)')

subplot(2,2,3)
plot(s,t)
title('Thermocouple 3')
xlabel('Time (seconds)')
ylabel('Voltage (volts)')

subplot(2,2,4)
plot(u,v)
title('Thermocouple 4')
xlabel('Time (seconds)')
ylabel('Voltage (volts)')