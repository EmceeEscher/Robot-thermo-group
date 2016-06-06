function averageValues(x,y,file)
% n = size(x)
% y_avg = zeros(1,n(2));
% 
% for i = num+1:n(2)-num-1
%     ys = zeros(1,num*2+1);
%     for j = i-num:i+num
%         ys(j-(i-num)+1) = y(j);
%     end
%     y_avg(i) = (max(ys)+min(ys))/2;
% end
% 
% x_new = x(num+1:n(2)-num-1);
% y_new = y_avg(num+1:n(2)-num-1);
% plot(x_new,y_new);
% end

y_smooth = smooth(smooth(smooth(smooth(y,60,'sgolay',3),40,'sgolay',3),40,'sgolay',3),40,'sgolay',3);
f = fopen(file,'w');
for i = 1:size(y_smooth)
   fprintf(f, '%16.8f %16.8f\n\r', x(i),y_smooth(i)); 
end
fclose(f);
plot(x,y_smooth);
end
