
// Copyright 2020, Diogo Costa, diogo.pinhodacosta@canada.ca
// This file is part of OpenWQ model.

// This program, openWQ, is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) aNCOLS later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#include "OpenWQ_output.hpp"


int OpenWQ_output::writeResults(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_solver& OpenWQ_solver,
    time_t simtime){ // needs to be in seconds since 00:00 hours, Jan 1, 1900 UTC
    

    // Local Variables
    std::vector<int>::iterator it;          // Iterator used to store the position of searched element
    std::string output_file_label;          // label do add to file (to flag debug mode)
    std::string msg_string;                 // interactive message to print
    std::string outputfile_type;            // interactive name for output file

    // Create time string to print
    struct tm *tm_simtime = localtime(&simtime);

    // Converting to string
    char timechar [30];
    strftime(timechar,30,"%Y%b%d-%H:%M:%S",tm_simtime);
    std::string timestr = std::string(timechar);

    // Convert timestr to upper case to avoid issues
    try{
        std::transform(
        timestr.begin(),
        timestr.end(),
        timestr.begin(), // Convert to lower case to avoid issues
        [](unsigned char c) { return std::toupper(c); });
        
    }catch (...){}


    /* ########################################
    // Loop over comparments
    ######################################## */
    
    //Begin OpenMP parallel region

    //#pragma omp parallel for private(it) num_threads(OpenWQ_wqconfig.num_threads_requested)
    for (unsigned int icmp=0;icmp<OpenWQ_hostModelconfig.get_num_HydroComp();icmp++){

        // See if icmp is in compt2print 
        // (which means user wants to print variables for this compartment)
        it = std::find(
            OpenWQ_wqconfig.compt2print.begin(), 
            OpenWQ_wqconfig.compt2print.end(), 
            icmp);

        // If cannot find icmp in compt2print - do not print
        if (it == OpenWQ_wqconfig.compt2print.end())
            continue;

        // ################################################
        // CSV
        if (OpenWQ_wqconfig.is_output_type_csv()){

            outputfile_type = "CSV";

            // ###########
            // MAIN: label file and call main function
            // print: OpenWQ_vars.chemas
            output_file_label.assign("main");
            OpenWQ_output::writeCSV(
                OpenWQ_json,
                OpenWQ_hostModelconfig,
                OpenWQ_wqconfig,
                OpenWQ_vars.chemass,
                output_file_label,
                timestr,
                icmp);

            // ###########
            // If DEBUG model has been selected
            // Print the derivatives
            if (OpenWQ_wqconfig.debug_mode == true){

                // ########### DERIVATIVES (d_dt) (#############
                // chem and transport

                // Var: d_chemass_dt_chem
                output_file_label.assign("d_output_dt_chemisry");
                OpenWQ_output::writeCSV(
                    OpenWQ_json,
                    OpenWQ_hostModelconfig,
                    OpenWQ_wqconfig,
                    OpenWQ_vars.d_chemass_dt_chem_out,
                    output_file_label,
                    timestr,
                    icmp);

                // Var: d_chemass_dt_transp
                output_file_label.assign("d_output_dt_transport");
                OpenWQ_output::writeCSV(
                    OpenWQ_json,
                    OpenWQ_hostModelconfig,
                    OpenWQ_wqconfig,
                    OpenWQ_vars.d_chemass_dt_transp_out,
                    output_file_label,
                    timestr,
                    icmp);

                // ########### SS (sink and source loads) #############   

                // Var: d_chemass_ss
                output_file_label.assign("d_output_ss");
                OpenWQ_output::writeCSV(
                    OpenWQ_json,
                    OpenWQ_hostModelconfig,
                    OpenWQ_wqconfig,
                    OpenWQ_vars.d_chemass_ss_out,
                    output_file_label,
                    timestr,
                    icmp);

                // ########### EWF (external water fluxes) #############   

                // Var: d_chemass_ewf
                output_file_label.assign("d_output_ewf");
                OpenWQ_output::writeCSV(
                    OpenWQ_json,
                    OpenWQ_hostModelconfig,
                    OpenWQ_wqconfig,
                    OpenWQ_vars.d_chemass_ewf_out,
                    output_file_label,
                    timestr,
                    icmp);

                // ########### IC (just print once) #############   

                // Var: d_chemass_ic
                if (OpenWQ_wqconfig.print_oneStep == true){
                    output_file_label.assign("d_output_ic");
                    OpenWQ_output::writeCSV(
                        OpenWQ_json,
                        OpenWQ_hostModelconfig,
                        OpenWQ_wqconfig,
                        OpenWQ_vars.d_chemass_ic,
                        output_file_label,
                        timestr,
                        icmp);
                };

            }

        }
        // ################################################
        // HDF5
        else if (OpenWQ_wqconfig.is_output_type_hdf5()){

            outputfile_type = "HDF5";

            // ###########
            // MAIN: label file and call main function
            // print: OpenWQ_vars.chemas
            output_file_label.assign("main");
            
            OpenWQ_output::writeHDF5(
                OpenWQ_json,
                OpenWQ_hostModelconfig,
                OpenWQ_wqconfig,
                OpenWQ_vars.chemass,
                output_file_label,
                timestr,
                icmp);

            // ###########
            // If DEBUG model has been selected
            // Print the derivatives
            if (OpenWQ_wqconfig.debug_mode == true){

                // ########### DERIVATIVES (d_dt) (#############
                // chem and transport

                // Var: d_chemass_dt_chem
                output_file_label.assign("d_output_dt_chemistry");
                OpenWQ_output::writeHDF5(
                    OpenWQ_json,
                    OpenWQ_hostModelconfig,
                    OpenWQ_wqconfig,
                    OpenWQ_vars.d_chemass_dt_chem_out,
                    output_file_label,
                    timestr,
                    icmp);

                // Var: d_chemass_dt_transp
                output_file_label.assign("d_output_dt_transport");
                OpenWQ_output::writeHDF5(
                    OpenWQ_json,
                    OpenWQ_hostModelconfig,
                    OpenWQ_wqconfig,
                    OpenWQ_vars.d_chemass_dt_transp_out,
                    output_file_label,
                    timestr,
                    icmp);

                // ########### SS (isolated inputs) #############   

                // Var: d_chemass_ss
                output_file_label.assign("d_output_ss");
                OpenWQ_output::writeHDF5(
                    OpenWQ_json,
                    OpenWQ_hostModelconfig,
                    OpenWQ_wqconfig,
                    OpenWQ_vars.d_chemass_ss_out,
                    output_file_label,
                    timestr,
                    icmp);

                // ########### EWF (isolated inputs) #############   

                // Var: d_chemass_ewf
                output_file_label.assign("d_output_ewf");
                OpenWQ_output::writeHDF5(
                    OpenWQ_json,
                    OpenWQ_hostModelconfig,
                    OpenWQ_wqconfig,
                    OpenWQ_vars.d_chemass_ewf_out,
                    output_file_label,
                    timestr,
                    icmp);

                // ########### IC (just print once) #############   

                // Var: d_chemass_ic
                if (OpenWQ_wqconfig.print_oneStep == true){
                    output_file_label.assign("d_output_ic");
                    OpenWQ_output::writeHDF5(
                        OpenWQ_json,
                        OpenWQ_hostModelconfig,
                        OpenWQ_wqconfig,
                        OpenWQ_vars.d_chemass_ic,
                        output_file_label,
                        timestr,
                        icmp);
                };

            }
        
        }
    }

    // turn off (only save once xyz_elements database)
    OpenWQ_wqconfig.print_oneStep = false; 

    // update new OpenWQ_wqconfig.nexttime_out
    OpenWQ_wqconfig.update_nexttime_out();

    // Message in console reporting output printing Sucessful
    // Create Message
    msg_string = "<OpenWQ> Output export successful (" 
        + outputfile_type + "): " 
        + timestr;

    // Reset Derivatives 
    // Needed before start of new time iteraction
    OpenWQ_solver.Reset_Deriv(
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_vars,
        false,       // reset inst derivatives
        true);       // reset cumulative derivatives

    // Print it (Console and/or Log file)
    ConsoleLog(
        OpenWQ_wqconfig,    // for Log file name
        msg_string,         // message
        true,               // print in console
        true);              // print in log file

    // Completed succesfully 
    return EXIT_SUCCESS;

}



 /* ########################################
// CSV output
######################################## */
int OpenWQ_output::writeCSV(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    std::unique_ptr<arma::field<arma::field<arma::cube>>>& OpenWQ_var2print,
    std::string& output_file_label,
    std::string timestr,            // time step (in seconds)
    int icmp){                      // compartment index

    // Local Variables
    unsigned int ix, iy, iz;                // iteractive indexes for each domain elements
    unsigned int num_cells2print;           // iteractive number of cells to print for each compartment
    std::string CompName_icmp;              // compartment name (iteractive)
    std::string chem_name;                  // chemical name (iteractive)
    std::string filename;                   // iteractive output file name 
    std::vector<double> unit_multiplers;    // multipliers to convert units  
    double water_vol_i;                     // interactive water volume for calc of concentrations   
    bool printflag;                         // flag for printing
    
    // Get number of cells to print for compartment icmp
    num_cells2print = OpenWQ_wqconfig.cells2print_vec.at(icmp).n_rows;
    printflag = OpenWQ_wqconfig.cells2print_bool.at(icmp);

    // If nothing to prin, then exit
    if (printflag == false || num_cells2print == 0){
        return EXIT_SUCCESS;
    }
    
    // Compartment info
    CompName_icmp = OpenWQ_hostModelconfig.get_HydroComp_name_at(icmp);

    // Get unit multipliers (numerator and denominator
    unit_multiplers.push_back(OpenWQ_wqconfig.get_output_units_numerator()); // numerator
    unit_multiplers.push_back(OpenWQ_wqconfig.get_output_units_denominator()); // denominator
    
    // Reset file name for each compartment
    filename = OpenWQ_wqconfig.get_output_dir();

    filename.append("/");
    filename.append(CompName_icmp);
    filename.append("@");
    filename.append(timestr); // time stamp
    filename.append("-");
    filename.append(output_file_label); // 
    filename.append(".txt");

    // Initiate arma::mat to print data (double vals)
    arma::dmat filedata(num_cells2print,OpenWQ_wqconfig.chem2print.size() + 3); // starts with ix, iy and iz (so + 3)
    
    // Define header of file
    arma::field<std::string> header(OpenWQ_wqconfig.chem2print.size() + 3); // starts with ix, iy and iz (so + 3)
    header(0) = "ix [-]";
    header(1) = "iy [-]";
    header(2) = "iz [-]";

    // Parallel I/O section (writting to files)
    #pragma omp parallel for private(chem_name,ix,iy,iz,water_vol_i) collapse(2) num_threads(OpenWQ_wqconfig.get_num_threads_requested())
    for (unsigned int ixyz=0;ixyz<num_cells2print;ixyz++){   
        
        for (unsigned int ichem=0;ichem<OpenWQ_wqconfig.chem2print.size();ichem++){

            // Get ix, iy and iz from cells2print_vec 
            ix = OpenWQ_wqconfig.cells2print_vec[icmp](ixyz,0);
            iy = OpenWQ_wqconfig.cells2print_vec[icmp](ixyz,1);
            iz = OpenWQ_wqconfig.cells2print_vec[icmp](ixyz,2);
            // x,y,z for printing, so add 1 to initiate in 1 (and not zeros as in c++)
            filedata(ixyz,0) = ix + 1;
            filedata(ixyz,1) = iy + 1;
            filedata(ixyz,2) = iz + 1;

            // Get cell volume
            // only if concentration is asked as output
            // otherwise set it to 1 for no effect
            if (OpenWQ_wqconfig.is_conentration_requested()){
                water_vol_i = OpenWQ_hostModelconfig.get_waterVol_hydromodel_at(icmp,ix,iy,iz);
            }else{
                water_vol_i = 1.0f;
            }

            // Get chemical name
            chem_name = OpenWQ_wqconfig.BGC_general_chem_species_list[
                OpenWQ_wqconfig.chem2print[ichem]];           // index of chemical to print
                
            // Add chemical name to file header (add column)
            // Include the output units
            header(ichem + 3) = chem_name // chemical name
                                .append("#")
                                .append(OpenWQ_wqconfig.get_output_units()); // output units

            // Set to zero if volume of water is smaller 
            // than OpenWQ_hostModelconfig.watervol_minlim
            if (water_vol_i > OpenWQ_hostModelconfig.get_watervol_minlim()){
                
                filedata(ixyz,ichem + 3) = 
                    (*OpenWQ_var2print)
                        (icmp)
                        (OpenWQ_wqconfig.chem2print[ichem])         // index of chemical to print
                        (ix,iy,iz) 
                    * unit_multiplers[0]                            // unit conversion (numerator)
                    / ( water_vol_i // volume (if conc requested, set to 1 otherwise)
                        * unit_multiplers [1] );  // unit conversion (denominator) and 

            }else{

                // Set as NaN because the concentration is not really zero,
                // it simply does not exist when there is no water
                filedata(ixyz,ichem + 3) = OpenWQ_wqconfig.noWaterConc;

            }
        }
    }
    
    // Print to CSV file
    filedata.save(arma::csv_name(filename,header));

    return EXIT_SUCCESS;

}


/* ########################################
// HDF5 format
######################################## */

int OpenWQ_output::writeHDF5(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    std::unique_ptr<arma::field<arma::field<arma::cube>>>& OpenWQ_var2print,
    std::string& output_file_label,
    std::string timestr,            // time step (in seconds)
    int icmp){                      // compartment index

     // Local Variables
    unsigned ix, iy, iz;                    // iteractive x, y, z values for cell elemebts to print 
    unsigned int num_chem2print;            // number of chemical species to print
    std::string CompName_icmp;              // compartment name (iteractive)
    const char * chem_name;                 // chemical name pointer (iteractive)
    std::string filename;                   // iteractive output file name 
    std::string internal_database_name;     // iteractive database name
    unsigned int num_cells2print;           // iteractive number of cells to print
    arma::mat cells2print_xyzElements;          // matrix with the x,y,z combinations for printing
                                            // it will be equatl to OpenWQ_wqconfig.cells2print_vec[icmp] + 1;
    arma::mat cells2print_xyzElements_total(1,3); // nx, ny and nz (entire domain)
    std::vector<double> unit_multiplers;    // multipliers to convert units
    std::string units_string;               // units of output
    std::size_t it;                         // Iterator used to locate "/" symbol in units
    double water_vol_i;                    // interactive water volume for calc of concentrationsdouble water_vol_i;                 
    bool printflag;                         // flag for printing


    // Get number of cells to print for compartment icmp
    num_cells2print = OpenWQ_wqconfig.cells2print_vec.at(icmp).n_rows;
    printflag = OpenWQ_wqconfig.cells2print_bool.at(icmp);

    // If nothing to prin, then exit
    if (printflag == false || num_cells2print == 0){
        return EXIT_SUCCESS;
    }
    
    // Compartment info
    CompName_icmp = OpenWQ_hostModelconfig.get_HydroComp_name_at(icmp);
    
    // Get compartment nx, ny, nz
    cells2print_xyzElements_total(0,0) = OpenWQ_hostModelconfig.get_HydroComp_num_cells_x_at(icmp);
    cells2print_xyzElements_total(0,1) =  OpenWQ_hostModelconfig.get_HydroComp_num_cells_y_at(icmp);
    cells2print_xyzElements_total(0,2) =  OpenWQ_hostModelconfig.get_HydroComp_num_cells_z_at(icmp);

    // num of chemicals to print
    num_chem2print = OpenWQ_wqconfig.chem2print.size();

    // Get unit multipliers (numerator and denominator
    unit_multiplers.push_back(OpenWQ_wqconfig.get_output_units_numerator()); // numerator
    unit_multiplers.push_back(OpenWQ_wqconfig.get_output_units_denominator()); // denominator

    // Units string
    // replace "/" by "|" is needed because "/" is not compatible with directory full paths
    units_string = OpenWQ_wqconfig.get_output_units();
    it = (int) units_string.find("/");
    if (it <= units_string.size()){
        units_string.replace(it,1, "|");
    }

    // ########################################
    // Save ix, iy and iz in dataset (the first time)
    if (OpenWQ_wqconfig.print_oneStep == true){

        // Get x,y,z to print, which will be +1 to local 
        // indexes of OpenWQ_wqconfig.cells2print_vec[icmp] 
        cells2print_xyzElements = OpenWQ_wqconfig.cells2print_vec[icmp];      
        cells2print_xyzElements.for_each( [](arma::mat::elem_type& val) { val += 1.0; } );


        for (unsigned int ichem=0;ichem<num_chem2print;ichem++){

            // Get chemical name (convert to char for use in .save())
            chem_name = OpenWQ_wqconfig.BGC_general_chem_species_list[
            OpenWQ_wqconfig.chem2print[ichem]].c_str();           // index of chemical to print

            // Reset file name for each compartment
            filename = OpenWQ_wqconfig.get_output_dir();

            filename.append("/");
            filename.append(CompName_icmp); // compartment
            filename.append("@");
            filename.append(chem_name);     // chemical name
            filename.append("#");
            filename.append(units_string); // units
            filename.append("-");
            filename.append(output_file_label); // info about output (debug model) 
            filename.append(".h5");

            // x,y,z elements
            internal_database_name = "xyz_elements";      
            cells2print_xyzElements
                .save(arma::hdf5_name(
                    filename,
                    internal_database_name)); 

            // nx, ny, nz
            // x,y,z elements
            internal_database_name = "xyz_elements_total"; 
            cells2print_xyzElements_total
                .save(arma::hdf5_name(
                    filename,
                    internal_database_name,
                    arma::hdf5_opts::append));                  // no append (to clean old file if existant)

        }
    }

    // ########################################
    // Save datasets for each time step

    // Get number of cell elements to print
    num_cells2print = OpenWQ_wqconfig.cells2print_vec[icmp].n_rows;

    // Parallel I/O section (writting to files)
    #pragma omp parallel for private(chem_name,filename,ix,iy,iz,water_vol_i) collapse(1) num_threads(OpenWQ_wqconfig.get_num_threads_requested())
    for (unsigned int ichem=0;ichem<num_chem2print;ichem++){

        // Initiate arma::mat to print data
        arma::mat data2print(num_cells2print,1);

        // Get chemical name (convert to char for use in .save())
        chem_name = OpenWQ_wqconfig.BGC_general_chem_species_list[
            OpenWQ_wqconfig.chem2print[ichem]].c_str();           // index of chemical to print

        // Reset file name for each compartment
        filename = OpenWQ_wqconfig.get_output_dir();

        filename.append("/");
        filename.append(CompName_icmp); // compartment
        filename.append("@");
        filename.append(chem_name);     // chemical name
        filename.append("#");
        filename.append(units_string); // units
        filename.append("-");
        filename.append(output_file_label); // info about output (debug model) 
        filename.append(".h5");

        // Extract data 2 print
        for (unsigned int celli=0;celli<num_cells2print;celli++){

            // Get ix, iy and iz from cells2print_vec 
            ix = OpenWQ_wqconfig.cells2print_vec[icmp](celli,0);
            iy = OpenWQ_wqconfig.cells2print_vec[icmp](celli,1);
            iz = OpenWQ_wqconfig.cells2print_vec[icmp](celli,2);

            // Get cell volume
            // only if concentration is asked as output
            // otherwise set it to 1 for no effect
            if (OpenWQ_wqconfig.is_conentration_requested()){
                water_vol_i = OpenWQ_hostModelconfig.get_waterVol_hydromodel_at(icmp,ix,iy,iz);
            }else{
                water_vol_i = 1.0f;
            }

            // Save data to data2print datastructure
            // Set to zero if volume of water is smaller 
            // than OpenWQ_hostModelconfig.watervol_minlim (only if requesting conc)
            if (
                (OpenWQ_wqconfig.is_conentration_requested() &&
                water_vol_i > OpenWQ_hostModelconfig.get_watervol_minlim()) 
                || OpenWQ_wqconfig.is_conentration_requested() == false){
             
                data2print(celli,0) = 
                    (*OpenWQ_var2print)
                        (icmp)
                        (OpenWQ_wqconfig.chem2print[ichem])
                        (ix,iy,iz)
                    * unit_multiplers[0]          // numerator unit conversion
                    / (  water_vol_i              // water volume (= 1 if mass requested (and not conc))
                        * unit_multiplers [1] );  // denominator unit conversion 

            }else{

                // Set as NaN because the concentration is not really zero,
                // it simply does not exist when there is no water
                data2print(celli,0) = OpenWQ_wqconfig.noWaterConc;

            }

        }
        
        // Save results
        data2print
            .save(arma::hdf5_name(
                filename,                           // file name
                timestr,                            // database name: chem name + time
                arma::hdf5_opts::append));          // options

    }

    return EXIT_SUCCESS;
}
