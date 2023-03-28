function iir_filter(f0, color)
B = 400;
fs = 16000;
KB = 1 / (1 + cot(pi * B/fs));

%f0 = 2000;
Kf = cos(2*pi * f0/fs);
a1 = 2*Kf*(1-KB);

b = [KB 0 -KB];
a = [1 -a1 1-2*KB];


[h, w] = freqz(b, a, 100);

w = w*fs/2;

subplot(2, 1, 1);
plot(w/pi,(abs(h)), color)
xlabel('Frequency [Hz]')
ylabel('Magnitude')

hold on

subplot(2, 1, 2);
plot(w/pi,angle(h), color)
ax = gca;
xlabel('Frequency [Hz]')
ylabel('Phase [rad]')


hold on


end