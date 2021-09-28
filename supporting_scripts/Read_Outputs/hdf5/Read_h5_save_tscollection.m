
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

function [stsnames,output_tscollect] = Read_h5_save_tscollection(...
    folderpath,...
    extractElm_info)

    
    % Get hdf5 files in directory folderpath
    dinfo = dir(folderpath);
    filenames = {dinfo.name}';
    filenames(strcmp(filenames,'.')) = [];
    filenames(strcmp(filenames,'..')) = [];
    
    % Determine number of hdf5 files to examine
    num_info2print = numel(extractElm_info(:,1));
    
    % Loop over the info2print
    loc_valid_info2print = [];
    for i = 1:num_info2print
        
       
        % Get plot infor for each element selected
        extractElm_info_i = extractElm_info(i,:);

        % Find location of compartment/chemical in tsnames
        loc_file_i = find(contains(filenames,upper(extractElm_info_i{1})) == 1);
        
        % If cannot find a extractElm_info_i{1}, then through a warning message   
        if ~isempty(loc_file_i)
            
            loc_valid_info2print = [loc_valid_info2print, loc_file_i];
            
        else
            
            % Display warning messagea; requestd file could not be found
            disp(['<main_hdf5> Warning: could not find "',extractElm_info_i{1},'.h5 file. Request skipped.'])

        end
        
    end
    
    % Number of valid info2print requests
    num_valid_info2print = numel(loc_valid_info2print);
    
    % Create stsnames cell
    stsnames = cell(num_valid_info2print,1);
        
    % Create timeseries collection (with only valid info2print entries
    output_tscollect = cell(num_valid_info2print,1);
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % loop over files/compartments selected
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    hbar = parfor_progressbar(...
        num_valid_info2print,...
        ['Extracting data from OpenWQ *.h5 files: ',num2str(num_valid_info2print),' file(s)']);  %create the progress bar

    for i = 1:num_valid_info2print

        filename_i = filenames{loc_valid_info2print(i)};
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
        data_all = data_all(reorderedIndex,:,:,:);
        
        % Timeseries name 
        ts_name = filename_i(1:end-3);
        
        % Get names of timeseries in tscollection
        stsnames{i} = ts_name;
        
        % Create TimeTable
        ttdata = timetable(...
            datetime(datevec(time_all_num)),... % Time
            data_all);                          % Data
        
        % Add timeseries for timeseries collection    
        output_tscollect{i} = ttdata;
        
    end
    
    % close waitbar
    close(hbar);
    
    % remove empty rows in output_tscollect
    output_tscollect = output_tscollect(~cellfun(@isempty, output_tscollect(:,1)), :);


end
