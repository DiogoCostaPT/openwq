
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

function plot_OpenWQ_outputs_ConcX_ZProfileY_perTime(...
    plot_OpenWQ_outputs_all,...
    requestProfileAPI_raw,...
    openwq_noWaterConcFlag)
    
    % Extracting requestProfileAPI_raw container
    requestProfileAPI_raw_profile = requestProfileAPI_raw('Profile');
    requestProfileAPI_raw_layer_m_interv = requestProfileAPI_raw('layer_m_interval');
    requestProfileAPI_Axis = requestProfileAPI_raw('ReverseAxis_XY');
    requestProfileAPI_XaxisRev = requestProfileAPI_Axis{1};
    requestProfileAPI_YaxisRev = requestProfileAPI_Axis{2};
    requestProfileAPI_timestamps = requestProfileAPI_raw('TimeStamps');
    
    % API request: Convert to char and remove all whitespaces
    requestProfileAPI_raw = erase(char(requestProfileAPI_raw_profile),' ');
    requestProfileAPI = upper(requestProfileAPI_raw);
    
    % Parsing profile request -> requestProfileAPI
    reqAPI_MainDir = requestProfileAPI(1);
    reqAPI_SecDir1_dim = requestProfileAPI(3);
    reqAPI_SecDir1_val = str2double(extractBetween(requestProfileAPI,...
                            [reqAPI_SecDir1_dim,'='],','));
    reqAPI_SecDir2_dim = char(extractBetween(requestProfileAPI,',','='));
    reqAPI_SecDir2_val = str2double(extractBetween(requestProfileAPI,...
                          	[reqAPI_SecDir2_dim,'='],')'));
    
    % Number of variables to plot (only valid entries)
    % Get this from any file
    plot_OpenWQ_outputs_ref = plot_OpenWQ_outputs_all{1,2};
    num_var = numel(plot_OpenWQ_outputs_ref(:,1));
    
    % Number of results to plot for each variabls
    % Includes derivatives if debug mode is on
    num_res2plot = numel(plot_OpenWQ_outputs_all(:,1));

    % Loop over the number of variables
    for i = 1:num_var

        % Get plot info for each element selected
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
            % This contains all data extract, but not processed yet
            % for profiling based on requestProfileAPI
            ts = plot_OpenWQ_outputs{i,2};
            
            % Replace openwq_noWaterConcFlag values by NaN for ommission
            ts.data_save_final(ts.data_save_final == openwq_noWaterConcFlag) = NaN;
            
            % Get min and max time
            % Just need to get from -main
            if m == 1
                minTime = min(ts.Time);
                maxTime = max(ts.Time);
            end
            
            % Appending string with type of output (main or derivatives)
            output_mode = ['(',plot_OpenWQ_outputs_name,')'];
            
            % Get all cells extracted in read_output
            cellsAllExtracted = plot_OpenWQ_outputs{i,3};
            
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % Extract profile data based on requestProfileAPI  
            XexistFlag = 0;
            YexistFlag = 0;
            ZexistFlag = 0;
            switch reqAPI_MainDir
                case 'X'; XexistFlag = XexistFlag + 1;
                case 'Y'; YexistFlag = YexistFlag + 1;
                case 'Z'; ZexistFlag = ZexistFlag + 1;
                otherwise
                    % Display warning messagea; requestd invalid
                    % and skip
                    disp(['<plot_ConcX_ZProfile_perTime> Warning: Invalid requestProfileAPI -> "',...
                        reqAPI_SecDir1_dim, '". Should have the format e.g. "z(x=10,y=14)"'])
                    return; 
            end
            switch reqAPI_SecDir1_dim
                case 'X'; SecDir1_index = 1; XexistFlag = XexistFlag + 1;
                case 'Y'; SecDir1_index = 2; YexistFlag = YexistFlag + 1;
                case 'Z'; SecDir1_index = 3; ZexistFlag = ZexistFlag + 1;
                otherwise
                    % Display warning messagea; requestd invalid
                    % and skip
                    disp(['<plot_ConcX_ZProfile_perTime> Warning: Invalid requestProfileAPI -> "',...
                        reqAPI_SecDir1_dim, '". Should have the format e.g. "z(x=10,y=14)"'])
                    return; 
            end
            switch reqAPI_SecDir2_dim
                case 'X'; SecDir2_index = 1; XexistFlag = XexistFlag + 1;
                case 'Y'; SecDir2_index = 2; YexistFlag = YexistFlag + 1;
                case 'Z'; SecDir2_index = 3; ZexistFlag = ZexistFlag + 1;
                otherwise
                    % Display warning messagea; requestd invalid
                    % and skip
                    disp(['<plot_ConcX_ZProfile_perTime> Warning: Invalid requestProfileAPI -> "',...
                        reqAPI_SecDir2_dim, '". Should have the format e.g. "z(x=10,y=14)"'])
                    return; 
            end
            if (XexistFlag~=1 || YexistFlag~=1 || ZexistFlag~=1)
                disp(['<plot_ConcX_ZProfile_perTime> Warning: Invalid requestProfileAPI -> "',...
                        requestProfileAPI_raw, '". Should have the format e.g. "z(x=10,y=14)", "x(z=1,y=22)"'])
                return; 
            end
            
            % Get indexes of requested profile cells
            nrows_cellsAllExtracted = numel(cellsAllExtracted(:,1));
            ProfileCells_Loc = zeros(nrows_cellsAllExtracted,1);
            rowi = 0;
            for k = 1:nrows_cellsAllExtracted
                if (cellsAllExtracted(k,SecDir1_index) == reqAPI_SecDir1_val...
                    && cellsAllExtracted(k,SecDir2_index) == reqAPI_SecDir2_val)
                    rowi = rowi + 1; 
                    ProfileCells_Loc(rowi) = k;
                end
            end
            ProfileCells_Loc(ProfileCells_Loc(:,1)==0) = []; % remove unused rows
            
            % Get Timestamps in timeseries
            nTimeStamps_ts = numel(ts.Time);
            nTimeStamps_req = numel(requestProfileAPI_timestamps);
            TimeStamps_Loc = zeros(nTimeStamps_req,1);
            rowi = 0;
            for p = 1:nTimeStamps_req
                foundFlag = false;
                for k = 1:nTimeStamps_ts
                    if ts.Time(k) == requestProfileAPI_timestamps(p)
                        rowi = rowi + 1; TimeStamps_Loc(rowi) = k;
                        foundFlag = true;
                    end
                end
                % If requestProfileAPI_timestamps(p) is not found in the timeseries
                % Then through a warning message
                if foundFlag==false
                    % Disp message
                    disp(['<plot_ConcX_ZProfile_perTime> Warning: Timesamp requested not available in simulation outputs -> "',...
                    char(requestProfileAPI_timestamps(p)),...
                    '". Revise request in "requestProfileAPI(TimeStamps)". Request skipped.'])
                end
            end
            TimeStamps_Loc(TimeStamps_Loc(:,1)==0) = []; % remove unused rows
            
            % Check if Timestamps requested exist in the timeseries
            % If not, display warning message and return
            if isempty(TimeStamps_Loc)
                % Disp message
                disp(['<plot_ConcX_ZProfile_perTime> Warning: Any of the timesamps requested match those available in',...
                    ' the model results. Revise request in "requestProfileAPI(TimeStamps)". Request skipped.'])
                return; 
            end
            
           
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % Print results
            
            num_elem2print = numel(TimeStamps_Loc);  % # timestamps to print
            nelemProfile = numel(ProfileCells_Loc);  % # profile elements
            
            % legend
            legend_labels = char(ts.Time(TimeStamps_Loc));
            
            % Construct legend string
            for l = 1:num_elem2print    
                hold on
                plot(ts.data_save_final(TimeStamps_Loc(l),ProfileCells_Loc),...
                     (1:1:nelemProfile)*requestProfileAPI_raw_layer_m_interv,...
                     '-o');
                 
                % Reverse axis if requested
                if(requestProfileAPI_XaxisRev == true)
                    set(gca, 'Xdir', 'reverse'); 
                end
                if(requestProfileAPI_YaxisRev == true)
                    set(gca, 'Ydir', 'reverse'); 
                end
            end
            
            
            
            % Grnd and labels
            grid on
            ylabel([requestProfileAPI_raw_profile(1), ' (m)'])
            xlabel(tsnames_i,...
                'Interpreter', 'none')
            
            % Prepare legend
            legend(legend_labels,...
                'Location','eastoutside',...
                'Interpreter', 'none')

            % Title
            title(output_mode)
        
        end

    end

   
end