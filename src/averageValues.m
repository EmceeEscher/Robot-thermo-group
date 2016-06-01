function averageValues(x,y)
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
plot(x,smooth(smooth(smooth(smooth(y,60,'sgolay',3),40,'sgolay',3),40,'sgolay',3),40,'sgolay',3));
end
