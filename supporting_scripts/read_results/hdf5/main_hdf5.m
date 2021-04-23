
% Path to HDF5 folder
folderpath = '/media/dcosta/data/megasync/ec_main/models/crhm/code_crhm/build/Output_OpenWQ/HDF5/';

plot_elemt_flag = true;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Read h5 data and save to timeseries collection
output_tscollect = Read_h5_save_tscollection(folderpath);
   
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Get names of timeseries in tscollection
tsnames = Get_output_tsnames(output_tscollect);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Plot elements
if plot_elemt_flag
    
    % get element info
    plotElm_info = {...
        'SOIL_RECHR_NO3','all';...
        'SOIL_RECHR_NH4',[1,1,1;...
                          6,1,1]...
    };
    
    % plot requested data defined in plotElm_info
    plot_elements(...
        tsnames,...
        output_tscollect,...
        plotElm_info)
    
end



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Read timeseries names
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function stsnames = Get_output_tsnames(output_tscollect)
    
    % get number of timeseries
    num_ts = numel(output_tscollect);

    % initiate stsnames cell
    stsnames = cell(num_ts,1);

    for i = 1:num_ts
        
        % Get ts i
        ts = output_tscollect{i};
        
        % Save name of ts i
        stsnames{i} = ts.Name;
        
    end

end




