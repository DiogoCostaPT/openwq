
% Path to HDF5 folder
folderpath = '/media/dcosta/data/megasync/ec_main/models/crhm/code_crhm/build/Output_OpenWQ/HDF5/';

plot_elemt_flag = true;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Read h5 data and save to timeseries collection
output_tscollect = Read_h5_save_tscollection(folderpath);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Get names of timeseries in tscollection
tsnames = gettimeseriesnames(output_tscollect);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Plot elements
if plot_elemt_flag
    
    % element info
    plotElm_info = {...
        'SOIL_RECHR_NO3','all';...
        'SOIL_RECHR_NO3',[3,6]};
    
    % plot elements
    plot_elements(...
        tsnames,...
        output_tscollect,...
        plotElm_info)
    
end







