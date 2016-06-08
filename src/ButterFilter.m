[b,a] = butter(1,0.05);
freqz(b,a)
dataIn = y;
dataOut = filter(b,a,dataIn);

figure
% subplot(2,1,1)
% plot(x,dataIn)
% 
% subplot(2,1,2)
plot(x,dataOut)