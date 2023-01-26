
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

function output_tscollect = Read_h5_save_tscollection(...
    folderpath,...
    file_extensions_i,...
    extractElm_info)

    
    % Get hdf5 files in HDF5 directory
    dinfo = dir(folderpath);
    filenames = {dinfo.name}';
    filenames(strcmp(filenames,'.')) = [];
    filenames(strcmp(filenames,'..')) = [];
    
    % Determine number of hdf5 files available
    num_info2print = numel(extractElm_info(:,1));
    
    
    %% %%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Loop over the requested data provided in info2print
    % Check if all entries are valid and eliminate those that aren't
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    loc_valid_info2print = [];
    for i = 1:num_info2print
        
       
        % Get plot info for each data requested
        extractElm_info_i = extractElm_info(i,:);

        % Check if extractElm_info_i requested exist in existing HDF5 files
        temp = extractElm_info_i{1};
        temp(isspace(temp)) = [];       % remove all spaces
        temp = upper(temp);             % upper case
        temp = strrep(temp,'/','|');    % *.h5 cannot have "/", so replaced by "|" 
        temp = [temp,'-',file_extensions_i];
        extractElm_info_i{1} = temp;
        extractElm_info{i,1} = temp;
        loc_file_i = find(contains(filenames,extractElm_info_i{1}) == 1);
        
        
        % If cannot find cannot find apropriate HDF5 file,
        % then through a warning message and skip the requested data
        if ~isempty(loc_file_i)
            
            loc_valid_info2print = [loc_valid_info2print; loc_file_i];
            
        else
            
            % Add in error_INVALID marker
            loc_valid_info2print = [loc_valid_info2print; -99999];
             
            % Display warning messagea; requestd file could not be found
            disp(['<Read_h5_save_tscollection> Warning: could not find "',...
                extractElm_info_i{1},...
                '.h5 file. Request skipped.'])

        end
        
    end
    
    % Get valid and requested data locations in iloc_valid
    iloc_valid = find(loc_valid_info2print ~= -99999);
    
    % Get filenames needed for valid entries
    extractElm_info_valid = extractElm_info(iloc_valid,:);
    
    % Number of valid info2print requests
    num_valid_info2print = numel(iloc_valid);
    
        
    % Create timeseries collection (with only valid info2print entries
    output_tscollect = cell(num_valid_info2print,3);
    
    
    %% %%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % loop over files/compartments selected
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
     %create the progress bar
    hbar = parfor_progressbar(...
        num_valid_info2print,...
        ['Extracting requested data from OpenWQ *.h5 files for (-',...
        file_extensions_i,'): ',num2str(num_valid_info2print),' file(s)']);

    parfor i = 1:num_valid_info2print

        filename_i = extractElm_info_valid{i,1};
        filepath_i = [folderpath,filename_i,'.h5'];
       
        % update waitbar
        hbar.iterate(1);
        
        % Another check if h5 file
        try
           datasets = {h5info(filepath_i).Datasets.Name}';
        catch
           disp(['<main_hdf5> Warning: Could not find file ', filename_i,'.h5']) 
           continue
        end
        
        % Get time samples and model elements
        timestamps = datasets(1:end-2);         % time steps
        xyz_elements_name = datasets{end};        % z elements

        % x, y, z elements
        xyz_elements_source = h5read(filepath_i,['/',xyz_elements_name]);

        % Skip if source does not have data
        if isempty(xyz_elements_source)
           continue; 
        end
        
        % number of cell elements available in file
        num_xyx_elements = numel(xyz_elements_source(:,1));

        % cells requested
        xyz_elements_requested = extractElm_info_valid{i,2};
        
        % if "all", then need to change the request to actual elem,ents
        if strcmp(xyz_elements_requested,"all") 
            xyz_elements_requested = xyz_elements_source;
        end
        
        % Look for cells to print
        icell_request_all = [];
        xyz_elements_requested_validated = [];
        
        for l = 1:numel(xyz_elements_requested(:,1))
            
            icell_request_i = find(sum([...
                                xyz_elements_source(:,1) == xyz_elements_requested(l,1),...
                                xyz_elements_source(:,2) == xyz_elements_requested(l,2),...
                                xyz_elements_source(:,3) == xyz_elements_requested(l,3)],...
                                2) == 3); 
                                
            % add location if found
            icell_request_all = [icell_request_all,icell_request_i];
            
            % Get validated xyz_elements_requested
            xyz_elements_requested_validated = [xyz_elements_requested_validated;...
                                                xyz_elements_requested(l,:)];
            
        end
        
        % Validate the number of cells to print
        num_xyx_elements_validated = numel(xyz_elements_requested_validated(:,1));
        
        % Re-organize all the data (loop over time)
        num_timestaps = numel(timestamps);
        time_all = cell(num_timestaps,1); % as data-time string
        data_all = zeros(...
            num_timestaps,...           % time
            num_xyx_elements_validated) * NaN;    % number of cell elements available
        

        % loop over datasets 
        for tstep = 1:numel(timestamps)

            % load hdf5 data
            data_i = h5read(filepath_i,['/',timestamps{tstep}])';

            % Time
            time_i = timestamps(tstep);
            time_all(tstep) = time_i;  

            % Data (up to 3D)
            data_all(tstep,:,:) =...
                data_i(:,icell_request_all);

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
        data_save_final = data_all(reorderedIndex,:,:,:);
        
        
        % Create TimeTable
        ttdata = timetable(...
            datetime(datevec(time_all_num)),... % Time
            data_save_final...                        % Data
            );
        
        % Add timeseries for timeseries collection  
        output_tscollect(i,:) = {filename_i,...
                                    ttdata,...
                                    xyz_elements_requested_validated};
        
    end
    
    % close waitbar
    close(hbar);
    
    % remove empty rows in output_tscollect
    output_tscollect = output_tscollect(~cellfun(@isempty, output_tscollect(:,1)), :);


end
