B = 400;
fs = 16000;
KB = 1 / (1 + cot(pi * B/fs));
hold off

%% function
iir_filter(2000, 'blue')
iir_filter(4000, 'gree')
iir_filter(6000, 'red')

