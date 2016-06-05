function [t,T_dat] = convertToTemp(dataFile, calFile)
[t,V_dat] = get_calibration(dataFile);
[T_cal,V_cal] = get_calibration(calFile);
p = polyfit(T_cal,V_cal,1);
T_dat = (V_dat - p(2))/p(1);