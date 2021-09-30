
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
% Plot selected OpenWQ outputs
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function plot_OpenWQ_outputs(...
    plot_OpenWQ_outputs)

    % Number of variables to plot (only valid entries)
    num_var = numel(plot_OpenWQ_outputs(:,1));

    for i = 1:num_var
        
        % Get plot infor for each element selected
        tsnames_i = plot_OpenWQ_outputs{i,1};      
                
        % Initiate figure with respective name
        figure('Name',tsnames_i);

        % Get timeseries for compartment tsnames{loc_var}
        ts = plot_OpenWQ_outputs{i,2};

        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % Create legend      
        
        % Get info for legend
        cells2print = plot_OpenWQ_outputs{i,3};
        num_elem2print = numel(ts.data_save_final(1,:));
        legend_labels = cell(num_elem2print,1);

        % Construct legend string
        for l = 1:num_elem2print

                  legend_labels{l,1} = ['(',...
                      num2str(cells2print(l,1)),',',...
                      num2str(cells2print(l,2)),',',...
                      num2str(cells2print(l,3)),')'];

        end

        
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % Print results
        plot(ts.Time,ts.data_save_final,'-o')

        % Grind and labels
        grid on
        ylabel(tsnames_i,...
            'Interpreter', 'none')
        
        datetick('x','keepticks','keeplimits')

        % Prepare legend
        legend(legend_labels,...
            'Location','eastoutside',...
            'Interpreter', 'none')
        
    end
   
end