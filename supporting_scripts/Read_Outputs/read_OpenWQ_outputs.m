
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

function output_openwq_tscollect = read_OpenWQ_outputs(...
    openwq_readfuncs_dir,...    % Fullpath for needed functions
    folderpath,...              % Provide fullpath to directory where the HDF5 files are located
    plot_elemt_flag,...         % Flag to specify if to plot variables
    extractElm_info,...         % If the flag above is 1, then provide details about the variables to plot
    OpenWQ_output_format)       % Output format

    
    % Get fullpath to outputs
    fullpath_outputs = [folderpath,OpenWQ_output_format,'/'];
    

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Read selected h5 data and save to timeseries collection
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    % if HDF5
    if strcmp(OpenWQ_output_format,'HDF5')

        % Add path to HDF5 directory
        addpath([openwq_readfuncs_dir,'hdf5/']);
        
        % Read and sava requested data in extractElm_info
        output_openwq_tscollect = Read_h5_save_tscollection(...
            fullpath_outputs,...
            extractElm_info);
        
    % if CSV
    elseif strcmp(OpenWQ_output_format,'CSV')
        
        % Add path to CSV directory
        addpath([openwq_readfuncs_dir,'csv/']);
        
        % Read and sava requested data in extractElm_info
        output_openwq_tscollect = Read_csv_save_tscollection(...
            fullpath_outputs,...
            extractElm_info);
        
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Plot elements
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    if plot_elemt_flag

        % plot requested data defined in plotElm_info
        plot_OpenWQ_outputs(...
            output_openwq_tscollect);

    end

end



