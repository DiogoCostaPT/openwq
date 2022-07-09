
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

// For VTK support:
// This was built for hexahedrons (see line 66) for now (cubes, Rectangular cuboid, Trigonal trapezohedron, etc)
// But line 33:33 makes determining cubes of side lenght = 1 (but this can all be changed)
// based on https://lorensen.github.io/VTKExamples/site/Cxx/IO/WriteVTU/


#include "OpenWQ_output.h"


int OpenWQ_output::writeResults(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_solver& OpenWQ_solver,
    time_t simtime){ // needs to be in seconds since 00:00 hours, Jan 1, 1970 UTC
    

    // Local Variables
    std::vector<int>::iterator it;          // Iterator used to store the position of searched element
    std::string output_file_label;          // label do add to file (to flag debug mode)
    std::string msg_string;                  // interactive message to print
    std::string outputfile_type;            // interactive name for output file

    // Create time string to print
    struct tm *tm_simtime = localtime(&simtime);

    // Converting to string
    char timechar [30];
    strftime(timechar,30,"%Y%b%d-%H:%M:%S",tm_simtime);
    std::string timestr = std::string(timechar);


    /* ########################################
    // Loop over comparments
    ######################################## */
    
    //Begin OpenMP parallel region

    //#pragma omp parallel for private(it) num_threads(OpenWQ_wqconfig.num_threads_requested)
    for (unsigned int icmp=0;icmp<OpenWQ_hostModelconfig.num_HydroComp;icmp++){

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
        if (OpenWQ_wqconfig.output_type == 0){

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
                    OpenWQ_vars.d_chemass_dt_chem,
                    output_file_label,
                    timestr,
                    icmp);

                // Var: d_chemass_dt_transp
                output_file_label.assign("d_output_dt_transport");
                OpenWQ_output::writeCSV(
                    OpenWQ_json,
                    OpenWQ_hostModelconfig,
                    OpenWQ_wqconfig,
                    OpenWQ_vars.d_chemass_dt_transp,
                    output_file_label,
                    timestr,
                    icmp);

                // ########### SS (isolated inputs) #############   

                // Var: d_chemass_ss
                output_file_label.assign("d_output_ss");
                OpenWQ_output::writeCSV(
                    OpenWQ_json,
                    OpenWQ_hostModelconfig,
                    OpenWQ_wqconfig,
                    OpenWQ_vars.d_chemass_ss,
                    output_file_label,
                    timestr,
                    icmp);

                // ########### IC (just print once) #############   

                // Var: d_chemass_ss
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
        /*
        // ################################################
        // VTU
        }else if (OpenWQ_wqconfig.output_type == 1){

            outputfile_type = "VTK";
            
            OpenWQ_output::writeVTU(
                OpenWQ_json,
                OpenWQ_vars,
                OpenWQ_hostModelconfig,
                OpenWQ_wqconfig,
                timestr,
                icmp);
        */

        // ################################################
        // HDF5
        else if (OpenWQ_wqconfig.output_type == 2){

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
                output_file_label.assign("d_output_dt_chemisry");
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

                // ########### IC (just print once) #############   

                // Var: d_chemass_ss
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
    OpenWQ_wqconfig.nexttime_out += OpenWQ_wqconfig.timetep_out;

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
    OpenWQ_output::ConsoleLog(
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
    int icmp){                  // compartment index

    // Local Variables
    unsigned int ix, iy, iz;                // iteractive indexes for each domain elements
    unsigned int num_cells2print;           // iteractive number of cells to print for each compartment
    std::string CompName_icmp;              // compartment name (iteractive)
    std::string chem_name;                  // chemical name (iteractive)
    std::string filename;                   // iteractive output file name 
    std::vector<double> unit_multiplers;    // multipliers to convert units  
    double water_vol_i;                     // interactive water volume for calc of concentrations   

    
    // Get number of cells to print for compartment icmp
    num_cells2print = OpenWQ_wqconfig.cells2print_vec.at(icmp).n_rows;

    // If nothing to prin, then exit
    if (num_cells2print == 0){
        return EXIT_SUCCESS;
    }
    
    // Compartment info
    CompName_icmp = std::get<1>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp));  // name

    // Get unit multipliers (numerator and denominator
    unit_multiplers.push_back(std::get<1>(OpenWQ_wqconfig.output_units)); // numerator
    unit_multiplers.push_back(std::get<2>(OpenWQ_wqconfig.output_units)); // denominator
    
    // Reset file name for each compartment
    filename = OpenWQ_wqconfig.output_dir;

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
    #pragma omp parallel for private(chem_name,ix,iy,iz,water_vol_i) collapse(2) num_threads(OpenWQ_wqconfig.num_threads_requested)
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
            if (std::get<3>(OpenWQ_wqconfig.output_units) == true){
                water_vol_i = (*OpenWQ_hostModelconfig.waterVol_hydromodel)[icmp](ix,iy,iz);
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
                                .append(std::get<0>(OpenWQ_wqconfig.output_units)); // output units

            // Set to zero if volume of water is smaller 
            // than OpenWQ_hostModelconfig.watervol_minlim
            if (water_vol_i > OpenWQ_hostModelconfig.watervol_minlim){
                
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
                filedata(ixyz,ichem + 3) = -9999.0f;

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
    const char * chem_name  ;               // chemical name pointer (iteractive)
    std::string filename;                   // iteractive output file name 
    std::string internal_database_name;     // iteractive database name
    unsigned int num_cells2print;           // iteractive number of cells to print
    arma::mat cells2print_oneStep;          // matrix with the x,y,z combinations for printing
                                            // it will be equatl to OpenWQ_wqconfig.cells2print_vec[icmp] + 1;
    std::vector<double> unit_multiplers;    // multipliers to convert units
    std::string units_string;               // units of output
    std::size_t it;                         // Iterator used to locate "/" symbol in units
    double water_vol_i;                    // interactive water volume for calc of concentrationsdouble water_vol_i;                    // interactive water volume for calc of concentrations


    // Compartment info
    CompName_icmp = std::get<1>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp));  // name
    /*
    nx = std::get<2>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp));  // get domain dimensions                     
    ny = std::get<3>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp));
    nz = std::get<4>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp));
    */

    // num of chemicals to print
    num_chem2print = OpenWQ_wqconfig.chem2print.size();

    // Get unit multipliers (numerator and denominator
    unit_multiplers.push_back(std::get<1>(OpenWQ_wqconfig.output_units)); // numerator
    unit_multiplers.push_back(std::get<2>(OpenWQ_wqconfig.output_units)); // denominator

    // Units string
    // replace "/" by "|" is needed because "/" is not compatible with directory full paths
    units_string = std::get<0>(OpenWQ_wqconfig.output_units);
    it = (int) units_string.find("/");
    if (it <= units_string.size()){
        units_string.replace(it,1, "|");
     }

    // ########################################
    // Save ix, iy and iz in dataset (the first time)
    if (OpenWQ_wqconfig.print_oneStep == true){

        // Get x,y,z to print, which will be +1 to local 
        // indexes of OpenWQ_wqconfig.cells2print_vec[icmp] 
        cells2print_oneStep = OpenWQ_wqconfig.cells2print_vec[icmp];      
        cells2print_oneStep.for_each( [](arma::mat::elem_type& val) { val += 1.0; } );


        for (unsigned int ichem=0;ichem<num_chem2print;ichem++){

            // Get chemical name (convert to char for use in .save())
            chem_name = OpenWQ_wqconfig.BGC_general_chem_species_list[
            OpenWQ_wqconfig.chem2print[ichem]].c_str();           // index of chemical to print

            // Reset file name for each compartment
            filename = OpenWQ_wqconfig.output_dir;

            filename.append("/");
            filename.append(CompName_icmp); // compartment
            filename.append("@");
            filename.append(chem_name);     // chemical name
            filename.append("#");
            filename.append(units_string); // units
            filename.append("-");
            filename.append(output_file_label); // info about output (debug model) 
            filename.append(".h5");

            //arma::mat xelements_cube(nx*ny*nz,1);
            //OpenWQ_wqconfig.cells2print_vec
    
            //OpenWQ_wqconfig.cells2print_vec.at(icmp).size();

            // x,y,z elements
            internal_database_name = "xyz_elements";             // name for internal HDF5 database name
            
            cells2print_oneStep
                .save(arma::hdf5_name(
                    filename,
                    internal_database_name));                  // no append (to clean old file if existant)
            
            /*
            // y elements
            internal_database_name = "y_elements";             // name for internal HDF5 database name
            arma::linspace(1,ny,ny)
                .save(arma::hdf5_name(
                    filename,
                    internal_database_name,
                    arma::hdf5_opts::append));

            // z elements
            internal_database_name = "z_elements";             // name for internal HDF5 database name
            arma::linspace(1,nz,nz)
                .save(arma::hdf5_name(
                    filename,
                    internal_database_name,
                    arma::hdf5_opts::append));
            */

        }
    }

    // ########################################
    // Save datasets for each time step

    // Get number of cell elements to print
    num_cells2print = OpenWQ_wqconfig.cells2print_vec[icmp].n_rows;

    // Parallel I/O section (writting to files)
    #pragma omp parallel for private(chem_name,filename,ix,iy,iz,water_vol_i) collapse(1) num_threads(OpenWQ_wqconfig.num_threads_requested)
    for (unsigned int ichem=0;ichem<num_chem2print;ichem++){

        // Initiate arma::mat to print data
        arma::mat data2print(num_cells2print,1);

        // Get chemical name (convert to char for use in .save())
        chem_name = OpenWQ_wqconfig.BGC_general_chem_species_list[
            OpenWQ_wqconfig.chem2print[ichem]].c_str();           // index of chemical to print

        // Reset file name for each compartment
        filename = OpenWQ_wqconfig.output_dir;

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
            if (std::get<3>(OpenWQ_wqconfig.output_units) == true){
                water_vol_i = (*OpenWQ_hostModelconfig.waterVol_hydromodel)[icmp](ix,iy,iz);
            }else{
                water_vol_i = 1.0f;
            }

            // Save data to data2print datastructure
            // Set to zero if volume of water is smaller 
            // than OpenWQ_hostModelconfig.watervol_minlim
            if (water_vol_i > OpenWQ_hostModelconfig.watervol_minlim){
            
                data2print(celli,0) = 
                    (*OpenWQ_var2print)
                        (icmp)
                        (OpenWQ_wqconfig.chem2print[ichem])
                        (ix,iy,iz)
                    * unit_multiplers[0]                            // numerator unit conversion
                    / (  water_vol_i              // water volume (= 1 if mass requested (and not conc))
                        * unit_multiplers [1] );  // denominator unit conversion 

            }else{

                // Set as NaN because the concentration is not really zero,
                // it simply does not exist when there is no water
                data2print(celli,0) = -9999.0f;

            }

        }
        
        // Save
        data2print
            .save(arma::hdf5_name(
                filename,                               // file name
                timestr,   // database name: chem name + time
                arma::hdf5_opts::append));              // options
    }

    return EXIT_SUCCESS;
}

/* ########################################
// VTU output
########################################

int OpenWQ_output::writeVTU(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    std::string timestr,    // time step (in seconds)
    int icmp){          // compartment index

    // Local Variables
    unsigned int nx, ny, nz;                // compartment dimensions
    unsigned int index_i;                   // iteractive index
    unsigned int numvert, nnumel;           // iteractive variables for VTK
    std::string CompName_icmp;              // compartment name (iteractive)
    std::string filename;                   // iteractive output file name
    std::vector<double> unit_multiplers;    // multipliers to convert units      
    double water_vol_i;                    // interactive water volume for calc of concentrations
    

    // Compartment info
    CompName_icmp = std::get<1>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp));  // name
    nx = std::get<2>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp));  // get domain dimensions                     
    ny = std::get<3>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp));
    nz = std::get<4>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp));

    // Reset file name for each compartment
    filename = OpenWQ_wqconfig.output_dir;

    filename.append("/");
    filename.append(CompName_icmp);
    filename.append("_");
    filename.append(timestr); // time stamp
    filename.append("sec");
    filename.append(".vtu");
    std::string chemname;        

    // total number of vertices and hexahedrons
    numvert = (nx+1)*(ny+1)*(nz+1);
    nnumel = nx * ny * nz;

    
    ########################################
    // Determine the all the vertices (assuming a spacing of 1 m for now)
    ######################################## 
    double x[numvert][3];
    index_i = 0;
    for (unsigned int iz=0;iz<=nz;iz++){   
            for (unsigned int ix=0;ix<=nx;ix++){
                for (unsigned int iy=0;iy<=ny;iy++){
                x[index_i][0] = ix-0.5;
                x[index_i][1] = iy-0.5;
                x[index_i][2] = iz-0.5;
                index_i++;
            }
        }
    }

    ########################################
    // Determine the faces of each element/hexahedron
    ######################################## 
    vtkIdType pts[nnumel][8];
    index_i = 0;
    for (unsigned int iz=0;iz<nz;iz++){   
            for (unsigned int ix=0;ix<nx;ix++){
                for (unsigned int iy=0;iy<ny;iy++){
                pts[index_i][0] = (nx+1)*(ny+1)*(iz) + (ny+1)*(ix) + iy;
                pts[index_i][1] = (nx+1)*(ny+1)*(iz) + (ny+1)*(ix+1) + iy;
                pts[index_i][2] = (nx+1)*(ny+1)*(iz) + (ny+1)*(ix+1) + iy+1;
                pts[index_i][3] = (nx+1)*(ny+1)*(iz) + (ny+1)*(ix) + iy+1;
                pts[index_i][4] = (nx+1)*(ny+1)*(iz+1) + (ny+1)*(ix) + iy;
                pts[index_i][5] = (nx+1)*(ny+1)*(iz+1) + (ny+1)*(ix+1) + iy;
                pts[index_i][6] = (nx+1)*(ny+1)*(iz+1) + (ny+1)*(ix+1) + iy+1;
                pts[index_i][7] = (nx+1)*(ny+1)*(iz+1) + (ny+1)*(ix) + iy+1;
            index_i++;
            }
        }
    }

    // Determine vtkPoints for vtkUnstructuredGrid build
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for (index_i=0; index_i<numvert; index_i++) points->InsertPoint(index_i,x[index_i]);

    // Initiate vtkUnstructuredGrid and add all the hexahedrons
    vtkSmartPointer<vtkUnstructuredGrid> ugrid =
        vtkSmartPointer<vtkUnstructuredGrid>::New();
    ugrid->Allocate(100);
    for (index_i=0; index_i<nnumel; index_i++){
        ugrid->InsertNextCell(VTK_HEXAHEDRON, 8, pts[index_i]);
    }

    ugrid->SetPoints(points);


    ########################################
    // Loop over chemical species
    ########################################
    for (unsigned int ichem=0;ichem<(OpenWQ_wqconfig.chem2print.size());ichem++){ // all chemical species

        vtkSmartPointer<vtkDoubleArray> varexpot = vtkSmartPointer<vtkDoubleArray>::New();
        varexpot->SetNumberOfValues(numvert);

        // Set name of array (chem variable name)
        // Get chemical species name
        chemname = (OpenWQ_wqconfig.BGC_general_chem_species_list)[ichem];
        // Add discription of output units
        chemname.append("#");
        chemname.append(std::get<0>(OpenWQ_wqconfig.output_units));

        // Assign chem name to variable exported
        varexpot->SetName(chemname.c_str());

        ########################################
        // Loop dimensions 
        ########################################
        index_i = 0;
        for (unsigned int iz=0;iz<=nz;iz++){   
                for (unsigned int ix=0;ix<=nx;ix++){
                    for (unsigned int iy=0;iy<=ny;iy++){

                        // Get cell volume
                        water_vol_i = (*OpenWQ_hostModelconfig.waterVol_hydromodel)[icmp](ix,iy,iy);

                        if(iz!=nz && iy!=ny && ix!=nx
                            && water_vol_i > OpenWQ_hostModelconfig.watervol_minlim){
                            varexpot->SetValue(
                                index_i, 
                                (*OpenWQ_vars.chemass)
                                    (icmp)
                                    (OpenWQ_wqconfig.chem2print[ichem])         // index of chemical to print
                                    (ix,iy,iz))
                                        * unit_multiplers[0]                            // numerator unit conversion
                                        / (  water_vol_i              // water volume (= 1 if mass requested (and not conc))
                                            * unit_multiplers [1] );  // denominator unit conversion 
                        }else{

                            // Set as NaN because the concentration is not really zero,
                            // it simply does not exist when there is no water
                            varexpot->SetValue(index_i, -9999.0f);
                        }
                index_i++;
                }
            }
        }
        ugrid->GetPointData()->AddArray(varexpot);
    }

    // Write file
    vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer =
        vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();  
    writer->SetFileName(filename.c_str());
    
    writer->SetInputData(ugrid);
    writer->Write();

    return EXIT_SUCCESS;

}
*/


/* ########################################
// Write to Console and Log file
######################################## */

void OpenWQ_output::ConsoleLog(
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    std::string& msg_string,
    bool print_console,
    bool print_logFile){

    // Print to Console if Requested (if print_console = true)
    if (print_console == true)
        std::cout << msg_string << std::endl;

    // Print to Console if Requested (if print_console = true)
    if (print_logFile == true){

        // Append line with error or warnning message
        std::ofstream log(
            OpenWQ_wqconfig.LogFile_name, 
            std::ios_base::app | std::ios_base::out);

        log << msg_string.append("\n");

    }

}