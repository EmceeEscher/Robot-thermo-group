function getTempGraphsNew(f1,f2,f3,f4)

%enter all files starting in the temperature data folder

[o,p] = get_calibration(['../data/temperature data/' f1]);
[q,r] = get_calibration(['../data/temperature data/' f2]);
[s,t] = get_calibration(['../data/temperature data/' f3]);
[u,v] = get_calibration(['../data/temperature data/' f4]);

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