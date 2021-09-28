

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

function [output_tscollect] = Read_h5_save_tscollection(...
    folderpath,...
    extractElm_info)


    % Get csv files in directory folderpath
    dinfo = dir(folderpath);
    filenames = {dinfo.name}';
    filenames(strcmp(filenames,'.')) = [];
    filenames(strcmp(filenames,'..')) = [];
    
    % Determine number of data requests to examine
    num_info2print = numel(extractElm_info(:,1));
    
     % Loop over the info2print requested to get compartment names
    cmpot_name_allraw = {};
    for i = 1:num_info2print
        
        % Get plot infor for each element selected
        extractElm_info_i = extractElm_info(i,:);

        % Find location of compartment/chemical in tsnames
        cmpot_name = extractBefore(upper(extractElm_info_i{1}),'_');
        
        % If cannot identify compartment name, through a warning message   
        if ~isempty(cmpot_name)
            
            % Add new entry to cmpot_name_all
            cmpot_name_allraw = [cmpot_name_allraw; cmpot_name];
            
        else
            
            % Display warning messagea; requestd file could not be found
            disp(['<main_csv> Warning: could not find compartment identifier "',...
                cmpot_name,'" in output file names. Request skipped.'])

        end  
    end
    
    % Remove repeated compartment names
    cmpot_name_allraw = unique(cmpot_name_allraw);
    cmpot_name_all = {};
    
     % Identify the respective output files for each compartment requested
     % Remove compartment requests for which no output files have been
     % found
    loc_valid_info2print = [];
    for i = 1:numel(cmpot_name_allraw)
        
        % Find location of compartments
        cmpot_name_i = cmpot_name_allraw{i};
        loc_file_i = find(contains(filenames,cmpot_name_i) == 1);
        
        % If cannot find a extractElm_info_i{1}, then through a warning message   
        if ~isempty(loc_file_i)
            
            loc_valid_info2print = [loc_valid_info2print, loc_file_i];
            
            % Add valid entry to final cmpot_name_all cell
            cmpot_name_all  = [cmpot_name_all;cmpot_name_i];
            
        else
            
            % Display warning messagea; requestd file could not be found
            disp(['<main_csv> Warning: could not find output data files for compartment "',...
                cmpot_name_i,'". Request skipped.'])

        end  
    end
    
    
    % Number of valid csv files found for requested fields
    num_valid_info2print = numel(loc_valid_info2print);
    
    % Create timeseries collection (with only valid info2print entries
    output_tscollect = cell(num_valid_info2print,1);
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % loop over files/compartments selected
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    

end