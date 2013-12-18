t = linspace(0,2*pi,100)(1:end-1);
xt = cos(t);
yt = sin(t);
t = t./(2*pi);
xt = (xt+1.0)/2.0;
yt = (yt+1.0)/2.0;

% scatter(xt,yt);

out = fopen('output.txt','w');
fprintf(out,'%f\n%f %f\n',[t;xt;yt])