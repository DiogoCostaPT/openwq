
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


%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% EXAMPLES OF INPUT ARGUMENTS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%{ 
 
    folderpath = '/media/dcosta/data/megasync/ec_main/models/crhm/code_crhm/build/Case_Studies/stc_openwq/Output_OpenWQ/HDF5/';

    plot_elemt_flag = true;

    plotElm_info = {...
        'SOIL_RECHR_LABILEP','all';...
        'SOIL_RECHR_REFRACTORYP',[1,1,1;...
                          6,1,1]...
        };

%}

%% FUNCTION
% Read and plot OpenWQ output data (HDF5 and CSV)

function output_openwq_tscollect_all = read_OpenWQ_outputs(...
    openwq_readfuncs_dir,...    % Fullpath for needed functions
    folderpath,...              % Provide fullpath to directory where the HDF5 files are located
    plot_elemt_flag,...         % Flag to specify if to plot variables
    extractElm_info,...         % If the flag above is 1, then provide details about the variables to plot
    OpenWQ_output_format,...    % Output format
    OpenWQ_output_debugmode)    % Debug mode

    % Disable Latex interpreter
    set(0, 'DefaultTextInterpreter', 'none')
    
    % Get fullpath to outputs
    fullpath_outputs = [folderpath,OpenWQ_output_format,'/'];
    
    file_extensions = {'main',...
                       'd_output_dt_chemisry',...   % only in debug mode
                       'd_output_dt_transport',...  % only in debug mode
                       'd_output_ss',...            % only in debug mode
                       'd_output_ic'};              % only in debug mode
    
   % Get number of files to load
   % IF OpenWQ_output_debugmode==true: load all file_extensions files
   % IF OpenWQ_output_debugmode==false: load only the first file (main)
   if (OpenWQ_output_debugmode == true)
       load_files_num = numel(file_extensions);
   else
       load_files_num = 1;
   end
   
   % Add path to HDF5 and CSV directories
    addpath(strcat(openwq_readfuncs_dir,'hdf5/'));
    addpath(strcat(openwq_readfuncs_dir,'csv/'));
   
    % Initiate the cell to save all results
    output_openwq_tscollect_all = cell(load_files_num,2);

    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Read selected h5 data and save to timeseries collection
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    % Set output names (including debug variables if requested)
    output_openwq_tscollect_all(:,1) = file_extensions(1:1:load_files_num);
    

   for f=1:load_files_num
        
        % Get file extension
        file_extensions_i = file_extensions{f};
    
        % if HDF5
        if strcmp(OpenWQ_output_format,'HDF5')

            % Read and sava requested data in extractElm_info
            output_openwq_tscollect = Read_h5_save_tscollection(...
                fullpath_outputs,...
                file_extensions_i,...
                extractElm_info);

        % if CSV
        elseif strcmp(OpenWQ_output_format,'CSV')

            % Read and sava requested data in extractElm_info
            output_openwq_tscollect = Read_csv_save_tscollection(...
                fullpath_outputs,...
                file_extensions_i,...
                extractElm_info);

        end
        
        % Save results
        %output_openwq_tscollect_all{f,1} = file_extensions_i;
        output_openwq_tscollect_all{f,2} = output_openwq_tscollect;
        
        
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Plot elements
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    if plot_elemt_flag

        % plot requested data defined in plotElm_info
        plot_OpenWQ_outputs(...
            output_openwq_tscollect_all);

    end

end



