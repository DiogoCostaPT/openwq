

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

function output_tscollect = Read_csv_save_tscollection(...
    folderpath,...
    file_extensions_i,...
    extractElm_info)

    
    % Get csv files in directory folderpath
    dinfo = dir(folderpath);
    filenames = {dinfo.name}';
    filenames(strcmp(filenames,'.')) = [];
    filenames(strcmp(filenames,'..')) = [];
    
    % Determine number of data requests to examine
    num_info2print = numel(extractElm_info(:,1));
    
    %% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % DATA REQUESTED
    % Identify valid data requestes in extractElm_info
    %
    % Loop over the extractElm_info to get compartment names for which
    % informaton was requested, show "error_INVALID"
    % and warn of any unvalid entry
    
    % initiate variable with compartment-chem data requests
    cmpot_chem_request_allraw = {};
    
    for i = 1:num_info2print
        
        % Get plot info for each element selected
        extractElm_info_i = extractElm_info(i,:);

        % Find location of compartment/chemical in tsnames
        cmpot_name = strtrim(...
            extractBefore(upper(extractElm_info_i{1}),'@'));
        
        % Find chem information
        chem_name = strtrim(...
            extractAfter(upper(extractElm_info_i{1}),'@'));
        
        
        % If cannot identify compartment name, through a warning message   
        if ~isempty(cmpot_name) && ~isempty(chem_name)
            
            % Add new entry to cmpot_name_all
            cmpot_chem_request_allraw = [cmpot_chem_request_allraw;...
                {cmpot_name,...     % compartment name
                chem_name...        % chem name
                },extractElm_info_i(2)];
            
        else
            
            % Add "error_INVALID" entry to cmpot_name_all
            cmpot_chem_request_allraw = [cmpot_chem_request_allraw;...
                [{'error_INVALID'},{'error_INVALID'},{'error_INVALID'}]];
            
            % Display warning messagea; requestd file could not be found
            disp(['<main_csv> Warning: could not combination of compartment and chemical',... 
                'requested: ',...
                cmpot_name,' and ', chem_name,' in output file names. Request skipped.'])

        end  
    end
    
    % Identify requested compartment names
    cmpot_name_list = unique(cmpot_chem_request_allraw(:,1));
    
    
    %% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % OUTPUT DATA AVAILABLE IN FILES
    % Identify the respective output files for each compartment requested
    %
    % Remove compartment requests for which no output files have been
    % found
    
    % Initiate variables to storge info about cvs files for each compartment
    cmpot_name_fileList_index = {};             % indexes of allfiles in filenames for each comparment
    cmpot_name_list_fileexist = {};             % valid entris of cmpot_name_list 
    cmpot_chem_request_allraw_valid = {};       % valid cmpot_chem_request_allraw 
    
    filelist_i = {}; % iteractive variable for file i
     
    % Loop over compartment request list
    for i = 1:numel(cmpot_name_list)
        
        % Find location of compartments
        cmpot_name_i = cmpot_name_list{i};
        
        % Ignore if cmpot_name_i = 'error_INVALID'
        if contains(cmpot_name_i,'error_INVALID')
            continue
        end
        
        % Identify the files for each compartment
        loc_file_i = find(contains(filenames,cmpot_name_i) == 1);
        
        % If cannot find a extractElm_info_i{1}, then through a warning message   
        if ~isempty(loc_file_i)
            
            cmpot_name_fileList_index = [cmpot_name_fileList_index, loc_file_i];
            
            % Add valid entry to final cmpot_name_all cell
            cmpot_name_list_fileexist  = [cmpot_name_list_fileexist;cmpot_name_i];
            
            % Update cmpot_chem_request_allraw_valid 
            cmpt_loc = find(strcmp(cmpot_chem_request_allraw(:,1),cmpot_name_i)==1);
            cmpot_chem_request_allraw_valid = [cmpot_chem_request_allraw_valid;...
                cmpot_chem_request_allraw(cmpt_loc,:)];

        else
            
            % Display warning messagea; requestd file could not be found
            disp(['<main_csv> Warning: could not find CSV output data files for compartment "',...
                cmpot_name_i,'". Request skipped.'])

        end  
    end
    
    
    % Number of valid csv files found for requested fields
    num_valid_cmpt = numel(cmpot_name_list_fileexist);
  
    % Create timetable collection (with only valid info2print entries
    output_tscollect = {};
    
    % Interactor for printing data into 
    itrct = 0;
    
    
    %% %%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % BUILD THE OUTPUT ARGUMENT: output_tscollect
    %
    % loop over files/compartments selected
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    % Create variable to store all times stamps (as datenum)
    % Only need to create this timestamp_all when examining the first
    % compartment because it will be the same for all 
    timestamp_all = zeros(numel(cmpot_name_fileList_index{1}),1);
    
    
     %create the progress bar
     hbar = parfor_progressbar(...
        num_valid_cmpt,...
        ['Extracting requested data from OpenWQ *.csv files: ',...
        num2str(num_valid_cmpt),...
        ' compartment(s)']); 

    for i = 1:num_valid_cmpt
        
        % Updating waitbar
        hbar.iterate(1);
        
        % Compartment name i
        cmpt_name_i = cmpot_name_list_fileexist{i};
        
        % Get valid output files for compartment cmpt_name_i
        valid_files4compt_index_i = cmpot_name_fileList_index{i};
        
        % Get chem and cells requests for cmpt_name_i
        cmpt_loc = find(strcmp(cmpot_chem_request_allraw_valid(:,1),cmpt_name_i)==1);
        chem_cells_request = cmpot_chem_request_allraw_valid(cmpt_loc,2:3);
                
        
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % Loop over files
        %
        % Create a structure to save all chem/cell data for each
        % compartment for saving in output_tscollect after the loop is
        % finished
        data_compt_2save = struct;
        cells_valid = struct;
        
        % Initiating loop 
        % over the files of interest
        for f = 1:numel(valid_files4compt_index_i)
            
            % Get filename i
            filename_i = filenames{valid_files4compt_index_i(f)};
            
            % Extract timestamp
            timestamp = extractBetween(filename_i,'@','.txt');
            
            % Add timestamp to timestamp_all if first screening
            % only needed during analysis of first compartment
            % it's the same for all compartments
            if i == 1
               timestamp_all(f) = datenum(timestamp{:},'yyyymmmdd-HH:MM:SS');
            end
           
            % Open each output file in valid_files4compt_index_i
            datacsv_i = readtable([folderpath,...
                filename_i]);
            
            % Get cells (x,y,z) available
            xyz_elements_source = [datacsv_i.ix___,... % get x info
                                    datacsv_i.iy___,... % get y info
                                    datacsv_i.iz___];    % get z info
                            
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % Loop over chem requested
            %
            for c = 1:numel(chem_cells_request(:,1))
                
                % Get chem name i
                chem_name_i = chem_cells_request{c,1};
                
                % Remove spaces
                chem_name_i(isspace(chem_name_i)) = [];
                
                % Replace # and / by "_" because matlab 
                % does not recognize these symbols
                chem_name_i = strrep(chem_name_i,'#','_');
                chem_name_i = strrep(chem_name_i,'/','_');
                               
                % Get data in file for chem chem_name_i
                chem_data_i = datacsv_i.(genvarname(chem_name_i));
                
                % Get all cells requested
                xyz_elements_requested = chem_cells_request{c,2};
                
                % Get number of cells requested
                num_xyz_elements_requested = numel(xyz_elements_requested(:,1));
                
                % Create variable to store data for each xyz element
                % requested, as well as the valid cells
                chem_data_i_cell_k = [];
                cells_valid_k = [];
                
               
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                % Loop over all cells (x,y,z) requested
                %                
                parfor k = 1:num_xyz_elements_requested

                    % Identify relevant row in file
                    icell_request_i = find(sum([...
                        xyz_elements_source(:,1) == xyz_elements_requested(k,1),...
                        xyz_elements_source(:,2) == xyz_elements_requested(k,2),...
                        xyz_elements_source(:,3) == xyz_elements_requested(k,3)],...
                        2) == 3); 
                    
                    % If icell_request_i is not empty, then save data in output_tscollect
                    if ~isempty(icell_request_i)
                        
                        % Get specific data for cell icell_request_i
                        chem_data_i_cell_k = [chem_data_i_cell_k;...
                            chem_data_i(icell_request_i)];
                        
                        % Save valid cell
                        cells_valid_k = [cells_valid_k,...
                            xyz_elements_requested(k,:)];
    
                    else
                        
                        % Warning that cell x,y,z data could not be found
                        disp(['<Reac_csv_save_tscollection> Warning: Cell data ', ...
                            num2str(xyz_elements_requested(k,:)),...
                            'could not be found for compartment ',...
                            cmpt_name_i,...
                            ' and chemical ',...
                            chem_name_i]);
                        
                        continue;
                        
                    end 
                    
                end
                
                % Add data to field in data_compt_2save structure
                data_compt_2save(f).(genvarname(chem_name_i)) = chem_data_i_cell_k;
                
                % Add valid cells to cells_valid
                cells_valid(f).(genvarname(chem_name_i)) = cells_valid_k;
                
            end
            
        end
        
        
        % Save all compartment information to output_tscollect
        % lopp over all chemicals
        for c = 1:numel(chem_cells_request(:,1))
        
            % Gen data name
            chem_name_i = chem_cells_request{c,1};
            data_name = [cmpt_name_i,' @ ',chem_name_i];
            
            % Order chronologically
            [timestamp_all_sort, isort] = sort(timestamp_all,'ascend');
            data_save_now = [data_compt_2save.(genvarname(chem_name_i))]';
            data_save_final = data_save_now(isort,:);
            
            % Create TimeTable
            ttdata = timetable(...
                datetime(datevec(timestamp_all_sort)),...    % Time
                data_save_final...                        % Data
                );
            
            itrct = itrct + 1;
            output_tscollect(itrct,:) = {...
                data_name,...
                ttdata,...
                chem_cells_request{c,2}...
                };
            
        end
        
        
    end
    
    % close waitbar
      close(hbar);
    

end