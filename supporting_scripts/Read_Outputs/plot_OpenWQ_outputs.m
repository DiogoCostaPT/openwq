
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
    plot_OpenWQ_outputs_all,...
    openwq_noWaterConcFlag)

    
    % Number of variables to plot (only valid entries)
    % Get this from any file
    plot_OpenWQ_outputs_ref = plot_OpenWQ_outputs_all{1,2};
    num_var = numel(plot_OpenWQ_outputs_ref(:,1));
    
    % Number of results to plot for each variabls
    % Includes derivatives if debug mode is on
    num_res2plot = numel(plot_OpenWQ_outputs_all(:,1));

    % Loop over the number of variables
    for i = 1:num_var

        % Get plot infor for each element selected
        tsnames_i = plot_OpenWQ_outputs_ref{i,1};
        
        % Remove info about type of output
        tsnames_i = extractBefore(tsnames_i,'-');

        % Initiate figure with respective name
        figure('Name',tsnames_i);% Num of results to plot (if debug mode on, then derivatives will be printed
        
        
        % Determine number of subplots (5 in debug mode)
        xnum_subplots = ceil(num_res2plot^0.5);
        ynum_subplots = ceil(num_res2plot/xnum_subplots);
        
        % Loop over outputs (if debug mode is on, derivatives will be
        % printed)
        for m = 1:num_res2plot
            
            subplot(xnum_subplots,ynum_subplots,m)

            plot_OpenWQ_outputs_name = plot_OpenWQ_outputs_all{m,1};
            plot_OpenWQ_outputs = plot_OpenWQ_outputs_all{m,2};
    
            % Get timeseries for compartment tsnames{loc_var}
            ts = plot_OpenWQ_outputs{i,2};
            
            % Get min and max time
            % Just need to get from -main
            if m == 1
                minTime = min(ts.Time);
                maxTime = max(ts.Time);
            end
            
            % Appending string with type of output (main or derivatives)
            output_mode = ['(',plot_OpenWQ_outputs_name,')'];
            
       
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

            % Replace openwq_noWaterConcFlag values by NaN for ommission
            ts.data_save_final(ts.data_save_final == openwq_noWaterConcFlag) = NaN;

            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % Print results
            hold on
            plot(ts.Time,ts.data_save_final,'-o')
                     

            % Grind and labels
            grid on
            ylabel(tsnames_i,...
                'Interpreter', 'none')

            try
                xlim([minTime, maxTime])
            catch
                
            end
            
            datetick('x','keeplimits','keepticks')

            % Prepare legend
            legend(legend_labels,...
                'Location','eastoutside',...
                'Interpreter', 'none')

            % Title
            title(output_mode)
        
        end

    end

   
end