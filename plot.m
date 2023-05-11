% open the file for reading
fid = fopen('FinalOutput.txt', 'r');

% read in the height and length of the graph
size_data = fscanf(fid, '%f', [1 2]);
graph_width = size_data(1);
graph_height = size_data(2);

% read in the coordinates
data = textscan(fid, '%s %f %f %f', 'HeaderLines', 0);
coords = [data{2}, data{3}, data{4}];

% close the file
fclose(fid);

% plot the coordinates
loop = size(coords, 1);
hold on
for i = 1:loop
    x = coords(i, 1);
    y = coords(i, 2) * 32;
    len = coords(i, 3) / 32;
    if coords(i, 3) == 0
        x = x + 32;
        scatter(x, y, 'r', 'filled');
    else
        % rectangle('Position',[x, y, len, 32],'FaceColor','None','EdgeColor',[0.5,0.5,0.5]);
    end
end

% set the axis limits
xlim([0 graph_width]);
ylim([0 graph_height]);

% add labels and title
xlabel('X coordinate');
ylabel('Y coordinate');
title('Coordinate plot');
