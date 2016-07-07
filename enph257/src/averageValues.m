function averageValues(x,y,file)

y_smooth = smooth(smooth(smooth(smooth(y,60,'sgolay',3),40,'sgolay',3),40,'sgolay',3),40,'sgolay',3);
f = fopen(file,'w');
for i = 1:size(y_smooth)
   fprintf(f, '%16.8f %16.8f\n\r', x(i),y_smooth(i)); 
end
fclose(f);
figure
plot(x,y_smooth);
end
