
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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Read HDF5 data and save to timeseries collection
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function output_tscollect = Read_h5_save_tscollection(folderpath)

    
    % Get hdf5 files (one per compartment)
    dinfo = dir(folderpath);
    filenames = {dinfo.name};
    filenames(strcmp(filenames,'.')) = [];
    filenames(strcmp(filenames,'..')) = [];
    
    % Create timeseries collection
    output_tscollect = cell(numel(filenames),1);
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % loop over files/compartments
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    hbar = parfor_progressbar(numel(filenames),'Extracting OpenWQ *.h5 outputs...');  %create the progress bar
    % loop
    parfor i = 1:numel(filenames)

        filename_i = filenames{i};
        filepath_i = [folderpath,filename_i];
       
        % update waitbar
        hbar.iterate(1);
        
        % Check if proper h5 file
        if ~strcmp(filepath_i(end-2:end),'.h5')
           continue; 
        end
        
        % Another check if h5 file
        try
           datasets = {h5info(filepath_i).Datasets.Name};
        catch
           disp(["ERR: not an HDF5 file:", filepath_i]) 
           continue
        end
        
        % Get time samples and model elements
        timestamps = datasets(1:end-3);         % time steps
        x_elements_name = datasets{end-2};      % x elements
        y_elements_name = datasets{end-1};      % y_elements
        z_elements_name = datasets{end};        % z elements

        % x, y, z elements
        x_elements = h5read(filepath_i,['/',x_elements_name]);
        y_elements = h5read(filepath_i,['/',y_elements_name]);
        z_elements = h5read(filepath_i,['/',z_elements_name]);

        % number of elements x, y and z
        num_x_elements = numel(x_elements);
        num_y_elements = numel(y_elements);
        num_z_elements = numel(z_elements);

        % Re-organize all the data (loop over time)
        num_timestaps = numel(timestamps);
        time_all = cell(num_timestaps,1); % as data-time string
        data_all = zeros(...
            num_timestaps,...       % time
            num_x_elements,...      % x-dir
            num_y_elements,...      % y-dir
            num_z_elements) * NaN;  % z-dir
        

        % loop over datasets 
        for tstep = 1:numel(timestamps)

            % load hdf5 data
            data_i = h5read(filepath_i,['/',timestamps{tstep}])';

            % Time
            time_i = timestamps(tstep);
            time_all(tstep) = time_i;  

            % Data (up to 3D)
            data_all(tstep,:,:,:) = data_i;

        end
        
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % Order timeseries
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        
        % first convert time string to time num
        time_all_num = datenum(time_all,'yyyymmmdd-HH:MM:SS');
        
        % then sort results and get indexes for new order
        [time_all_num,reorderedIndex] = sort(time_all_num);
        
        % re-order time and data 
        %time_all = time_all(reorderedIndex);
        data_all = data_all(reorderedIndex,:);
        
        % Timeseries name 
        ts_name = filename_i(1:end-3);
        
        % Create timeseries
        ts = timeseries(data_all,datestr(time_all_num),'Name',ts_name);

        % Add timeseries for timeseries collection    
        output_tscollect{i} = ts;
        
    end
    
    % close waitbar
    close(hbar);
    
    % remove empty rows in output_tscollect
    output_tscollect = output_tscollect(~cellfun(@isempty, output_tscollect(:,1)), :);


end
