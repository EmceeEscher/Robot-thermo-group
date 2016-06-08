function removeNoise(dataFile,outFile)

[x,y] = get_calibration(dataFile);

n = size(x);

for i = 2:n(2)
   if(abs(y(i)-y(i-1)) > 0.15)
       y(i) = y(i-1);
   end
end

f = fopen(outFile,'w');
for i = 1:n(2)
   fprintf(f, '%16.8f %16.8f\n\r', x(i),y(i)); 
end
fclose(f);

end