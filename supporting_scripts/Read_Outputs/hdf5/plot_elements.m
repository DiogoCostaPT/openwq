
% Copyright 2020, Diogo Costa, diogo.pinhodacosta@canada.ca
% This file is part of OpenWQ model.

% This program, openWQ, is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.

% You should have received a copy of the GNU General Public License
% along with this program.  If not, see <http://www.gnu.org/licenses/>.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Pot data elements
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function plot_elements(...
    tsnames,...
    output_tscollect,...
    extractElm_info)

    % Number of variables to plot (only valid entries)
    num_var = numel(tsnames(:,1));

    for i = 1:num_var
        
        % Get plot infor for each element selected
        tsnames_i = tsnames{i,:};      
                
        % Initiate figure with respective name
        figure('Name',tsnames_i);

        % Get timeseries for compartment tsnames{loc_var}
        ts = output_tscollect{i};

        % Get instruction for printing (first locate in file in
        % extractElm_info)
        loc_var = find(contains(upper(extractElm_info(:,1)),tsnames_i) == 1);
        plot_elem_select = extractElm_info{loc_var,2};

        % if selected print "all"
        if strcmp(plot_elem_select,'all')

            % selected ts is the entire ts
            ts_select = ts;

            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % Create legend
            elem_size = size(ts_select.Data(1,:,:,:)); % time + up to 3 dimensions
            num_elem2print = numel(ts_select.Data(1,:,:,:));
            legend_labels = cell(num_elem2print,1);

            % Get number of elements in x, y and z directions
            size_x = elem_size(2);                              % x-dir
            try size_y = elem_size(3); catch size_y = 1; end    % y-dir
            try size_z = elem_size(4); catch size_z = 1; end    % z-dir

            % Construct
            iloc = 1;
            for xi = 1:size_x
                for yi = 1:size_y
                    for zi = 1:size_z

                      legend_labels{iloc,1} = ['(',...
                          num2str(xi),',',...
                          num2str(zi),',',...
                          num2str(zi),')'];

                      iloc = iloc + 1;

                    end
                end
            end


        % otherwise
        else
            % num of elements to print
            num_elem2print = numel(plot_elem_select(:,1));

            % initiate ts_select data as a 2D matrix
            ts_select_data = zeros(...
                numel(ts.Time),...
                num_elem2print);

            % initiate legend_info
            legend_labels = cell(num_elem2print,1);

            % Get elements ts_select_data            
            for l=1:num_elem2print
                ts_select_data(:,l) = ts.data_all(:,... % time
                    plot_elem_select(l,1),...           % x elements
                    plot_elem_select(l,2),...           % y elements
                    plot_elem_select(l,3));             % z elements

                % Create legend
                legend_labels{l,1} = ['(',...
                          num2str(plot_elem_select(l,1)),',',...
                          num2str(plot_elem_select(l,2)),',',...
                          num2str(plot_elem_select(l,3)),')'];
            end

            % Build new timetable with selected elements only
             ts_select = timetable(...
                ts.Time,...
                ts_select_data);           

        end

        % Print
        plot(ts_select.Time,ts_select.Var1,'-o')

        % Grind and labels
        grid on
        ylabel(tsnames{i})
        datetick('x','keepticks','keeplimits')

        % Prepare legend
        legend(legend_labels,'Location','eastoutside')
        
    end
   
end