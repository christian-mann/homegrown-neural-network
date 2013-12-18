colors = ['y','r','b','m','g','c','w','k'];
nColors = 8;

files = input('List of files: ');

hold on;

for i = [1:length(files)]
	f = char(files(i));
	% disp(x);
	in = fopen(f,'r');
	xy=[0,0];
	while ~feof(in)
		xy=[xy;fscanf(in,'%f',2)'];
	end
	xy=xy(2:end-1,:);
	x = xy(:,1);
	y = xy(:,2);

	if(i==1)
		plot(x,y,colors(mod(i,nColors)+1));
	else
		scatter(x,y,colors(mod(i,nColors)+1));
	end
	fclose(in);
end