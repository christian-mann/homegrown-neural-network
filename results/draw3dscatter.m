colors = ['y','b','c','r','g','m','w','k'];
nColors = 8;

files = input('List of files: ');

hold on;

for i = [1:length(files)]
	f = char(files(i));
	% disp(x);
	in = fopen(f,'r');
	xyz=[0,0,0];
	while ~feof(in)
		xyz=[xyz;fscanf(in,'%f',3)'];
	end
	xyz=xyz(2:end,:);
	x = xyz(:,1);
	y = xyz(:,2);
	z = xyz(:,3);

	scatter3(x,y,z,colors(mod(i,nColors)+1));
	fclose(in);
end