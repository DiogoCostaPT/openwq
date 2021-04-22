
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Pot data elements
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function plot_elements(...
    tsnames,...
    output_tscollect,...
    plotElm_info)

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
        ts = output_tscollect{loc_var};
        
        % Get instruction for printing
        plot_elem_select = plotElm_info_i{2};
        
        % num of elements to print
        num_elem2print = numel(plot_elem_select(:,1));
        
        % if selected print "all"
        if strcmp(plot_elem_select,'all')
            ts_select = ts;
        % otherwise
        else
            % initiate ts_select data as a 2D matrix
            ts_select_data = zeros(...
                numel(ts.Time),...
                num_elem2print);
            
            % Get elements ts_select_data            
            for l=1:num_elem2print
                ts_select_data(:,l) = ts.Data(:,...                   % time
                    plot_elem_select(l,1),...      % x elements
                    plot_elem_select(l,2),...      % y elements
                    plot_elem_select(l,3));        % z elements
            end
                        
            % Build new timeseries with selected elements only
             ts_select = timeseries(...
                ts_select_data,...   
                ts.Time,...
                'Name',ts.Name);
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