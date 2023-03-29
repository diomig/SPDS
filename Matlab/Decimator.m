r = 13;
n = 1:1000;
lx = length(n);
fs = 1/lx;
x = sin(2*pi*n * fs);

plot(0:lx-1,x,'o')
hold on
y = decimate(x,r,82,'fir');
stem(0:r:lx-1,y,'ro','filled','markersize',4)

legend('Original','Decimated','Location','south')
xlabel('Sample number')
ylabel('Signal')