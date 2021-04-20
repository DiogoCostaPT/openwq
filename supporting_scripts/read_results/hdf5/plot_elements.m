
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Pot data elements
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function plot_elements(...
    tsnames,...
    output_tscollect,...
    plotElm_info)

    num_tsnames = numel(tsnames);   % number of timeseries stored
    
    % Number of variables to plot
    num_var = numel(plotElm_info(:,1));

    for i = 1:num_var
        
        % Get plot infor for each element selected
        plotElm_info_i = plotElm_info(i,:);

        % Find location of compartment/chemical in tsnames
        loc_var = find(contains(tsnames,plotElm_info_i{1}) == 1);
        
        % Initiate figure with respective name
        figure('Name',tsnames{loc_var})
        
        % Get timeseries for compartment tsnames{loc_var}
        ts = output_tscollect.(genvarname(tsnames{loc_var}));
        
        % Get instruction for printing
        plot_elem_select = plotElm_info_i{2};
       % if selected print "all"
        if strcmp(plot_elem_select,'all')
            ts_select = ts;
        % otherwise
        else
            ts_select = ts;
        end
        
        % Print
        plot(ts_select,'-o')
        
        % Grind and labels
        grid on
        ylabel(ts.Name)
        
        % Prepare legend
        legend('Location','eastoutside')

    end
    
end