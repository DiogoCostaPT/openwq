

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Read HDF5 data and save to timeseries collection
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function output_tscollect = Read_h5_save_tscollection(folderpath)

    % Create timeseries collection
    output_tscollect = tscollection();

    % Get hdf5 files (one per compartment)
    dinfo = dir(folderpath);
    filenames = {dinfo.name};
    filenames(strcmp(filenames,'.')) = [];
    filenames(strcmp(filenames,'..')) = [];
   
    % loop over files/compartments
    for i = 1:numel(filenames)

        filename_i = filenames{i};
        filepath_i = [folderpath,filename_i];

        % Data saved
        datasets = {h5info(filepath_i).Datasets.Name};
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
        time_all = zeros(num_timestaps,1) * NaN;
        data_all = zeros(...
            num_timestaps,...       % time
            num_x_elements,...      % x-dir
            num_y_elements,...      % y-dir
            num_z_elements) * NaN;  % z-dir

        for tstep = 1:numel(timestamps)

            % load hdf5 data
            data_i = h5read(filepath_i,['/',timestamps{tstep}])';

            % Time
            time_i = timestamps(tstep);
            time_num = str2double(time_i{:});
            time_all(tstep) = time_num;  

            % Data (up to 3D)
            data_all(tstep,:,:,:) = data_i;

        end  

        % Create timeseries
        ts = timeseries(data_all,time_all);

        % Add timeseries for timeseries collection 
        % Timeseries name 
        ts_name = filename_i(1:end-3);
        output_tscollect = addts(output_tscollect,ts,ts_name);

    end
end
