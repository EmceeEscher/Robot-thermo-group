figure

[T_cal1,V_cal1] = get_calibration('../data/calibration stuff/calibrate_tc1_May_25_Trial_3.dat');
p = polyfit(T_cal1,V_cal1,1);
T_1 = (run1(2,:) - p(2))/p(1);
subplot(2,2,1)
plot(run1(1,:),T_1)
title('Thermocouple 1')
xlabel('Time (seconds)')
ylabel('Temperature (°C)')

[T_cal2,V_cal2] = get_calibration('../data/calibration stuff/calibrate_tc2_May_25_Trial_3.dat');
p = polyfit(T_cal2,V_cal2,1);
T_2 = (run1(4,:) - p(2))/p(1);
subplot(2,2,2)
plot(run1(3,:),T_2)
title('Thermocouple 2')
xlabel('Time (seconds)')
ylabel('Temperature (°C)')

[T_cal3,V_cal3] = get_calibration('../data/calibration stuff/calibrate_tc3_May_25_Trial_3.dat');
p = polyfit(T_cal3,V_cal3,1);
T_3 = (run1(6,:) - p(2))/p(1);
subplot(2,2,3)
plot(run1(5,:),T_3)
title('Thermocouple 3')
xlabel('Time (seconds)')
ylabel('Temperature (°C)')

[T_cal4,V_cal4] = get_calibration('../data/calibration stuff/calibrate_tc4_May_25_Trial_3.dat');
p = polyfit(T_cal4,V_cal4,1);
T_4 = (run1(8,:) - p(2))/p(1);
subplot(2,2,4)
plot(run1(7,:),T_4)
title('Thermocouple 4')
xlabel('Time (seconds)')
ylabel('Temperature (°C)')