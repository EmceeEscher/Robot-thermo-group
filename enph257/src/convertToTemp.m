function [t,T_dat] = convertToTemp(dataFile, calFile,outFile)
[t,V_dat] = get_calibration(dataFile);
[T_cal,V_cal] = get_calibration(calFile);
p = polyfit(T_cal,V_cal,1);
T_dat = (V_dat - p(2))/p(1);

f = fopen(outFile,'w');
for i = 1:size(T_dat')
   fprintf(f, '%16.8f %16.8f\n\r', t(i),T_dat(i)); 
end
fclose(f);

end