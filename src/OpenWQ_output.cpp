
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
    time_t simtime){ // needs to be in seconds since 00:00 hours, Jan 1, 1970 UTC
    
    // Return if not time to print
    if (simtime =< OpenWQ_wqconfig.nexttime_out)
        return EXIT_SUCCESS;

    // Local Variables
    std::vector<int>::iterator it;          // Iterator used to store the position of searched element

    // Create time string to print
    struct tm * timeinfo;
    char timechar [30];
    timeinfo = gmtime (&simtime);
    strftime (timechar,30,"%Y%b%d-%H:%M:%S",timeinfo);
    std::string timestr = std::string(timechar);
    std::cout << timestr << std::endl;

    /* ########################################
    // Loop over comparments
    ######################################## */
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

        // CSV
        if (OpenWQ_wqconfig.output_type == 0){
                OpenWQ_output::writeCSV(
                    OpenWQ_json,
                    OpenWQ_vars,
                    OpenWQ_hostModelconfig,
                    OpenWQ_wqconfig,
                    timestr,
                    icmp);

        // VTU
        }else if (OpenWQ_wqconfig.output_type == 1){
            OpenWQ_output::writeVTU(
                OpenWQ_json,
                OpenWQ_vars,
                OpenWQ_hostModelconfig,
                OpenWQ_wqconfig,
                timestr,
                icmp);

        // HDF5
        }else if (OpenWQ_wqconfig.output_type == 2){
            OpenWQ_output::writeHDF5(
                OpenWQ_json,
                OpenWQ_vars,
                OpenWQ_hostModelconfig,
                OpenWQ_wqconfig,
                timestr,
                icmp);
        
        }
    }

    // turn off (only save once xyz_elements database)
    OpenWQ_wqconfig.print_xyz = false; 

    // update new OpenWQ_wqconfig.nexttime_out
    OpenWQ_wqconfig.nexttime_out += OpenWQ_wqconfig.timetep_out;

    // Completed succesfully 
    return EXIT_SUCCESS;

}



 /* ########################################
// CSV output
######################################## */
int OpenWQ_output::writeCSV(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    std::string timestr,            // time step (in seconds)
    int icmp){                  // compartment index

    // Local Variables
    unsigned int nx, ny, nz;                // compartment dimensions
    unsigned int num_chem2print;            // number of chemical species to print
    std::string CompName_icmp;              // compartment name (iteractive)
    std::string chem_name;                  // chemical name (iteractive)
    std::string filename;                   // iteractive output file name 
    int a;                                  // iteractive dummy variable for printing

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
    filename.append("_");
    filename.append("sec");
    filename.append(".txt");

    // Initiate arma::mat to print data
    arma::mat filedata(nx*ny*nz,OpenWQ_wqconfig.chem2print.size() + 3); // starts with ix, iy and iz (so + 3)
    
    // Define header of file
    arma::field<std::string> header(OpenWQ_wqconfig.chem2print.size() + 3); // starts with ix, iy and iz (so + 3)
    header(0) = "ix [-]";
    header(1) = "iy [-]";
    header(2) = "iz [-]";

    // Prepare data structure to print
    a = 0;
    for (unsigned int iz=0;iz<nz;iz++){   
        for (unsigned int iy=0;iy<ny;iy++){
            for (unsigned int ix=0;ix<nx;ix++){

                // ix, iy and iz
                filedata(a,0) = ix;
                filedata(a,1) = iy;
                filedata(a,2) = ix;

                for (unsigned int ichem=0;ichem<OpenWQ_wqconfig.chem2print.size();ichem++){

                    // Get chemical name
                    chem_name = OpenWQ_wqconfig.chem_species_list[
                        OpenWQ_wqconfig.chem2print[ichem]];           // index of chemical to print
                        
                    // Add chemical name to file header (add column)
                    header(ichem + 3) = chem_name;

                    filedata(a,ichem + 3) = (*OpenWQ_vars.chemass)
                        (icmp)
                        (OpenWQ_wqconfig.chem2print[ichem])         // index of chemical to print
                        (ix,iy,iz);     
                }
                    a += 1; 
            }
        }
    }
    // Print to CSV file
    filedata.save(arma::csv_name(filename,header));

    return EXIT_SUCCESS;

}

 /* ########################################
// VTU output
######################################## */

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
    
    /* ########################################
    // Determine the all the vertices (assuming a spacing of 1 m for now)
    ######################################## */
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

    /* ########################################
    // Determine the faces of each element/hexahedron
    ######################################## */
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

    /* ########################################
    // Loop over chemical species
    ######################################## */
    for (unsigned int ichem=0;ichem<(OpenWQ_wqconfig.chem2print.size());ichem++){ // all chemical species

        vtkSmartPointer<vtkDoubleArray> varexpot = vtkSmartPointer<vtkDoubleArray>::New();
        varexpot->SetNumberOfValues(numvert);

        // Set name of array (chem variable name)
        // Get chemical species name
        chemname = (OpenWQ_wqconfig.chem_species_list)[ichem];

        // Assign chem name to variable exported
        varexpot->SetName(chemname.c_str());

        /* ########################################
        // Loop dimensions 
        ######################################## */
        index_i = 0;
        for (unsigned int iz=0;iz<=nz;iz++){   
                for (unsigned int ix=0;ix<=nx;ix++){
                    for (unsigned int iy=0;iy<=ny;iy++){
                        if(iz!=nz && iy!=ny && ix!=nx){
                            varexpot->SetValue(
                                index_i, 
                                (*OpenWQ_vars.chemass)
                                    (icmp)
                                    (OpenWQ_wqconfig.chem2print[ichem])         // index of chemical to print
                                    (ix,iy,iz));
                        }else{
                            varexpot->SetValue(index_i, 0);
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

/* ########################################
// HDF5 format
######################################## */

int OpenWQ_output::writeHDF5(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    std::string timestr,    // time step (in seconds)
    int icmp){          // compartment index

     // Local Variables
    unsigned int nx, ny, nz;                // compartment dimensions
    unsigned int num_chem2print;            // number of chemical species to print
    std::string CompName_icmp;              // compartment name (iteractive)
    const char * chem_name  ;              // chemical name pointer (iteractive)
    std::string filename;                   // iteractive output file name 
    std::string internal_database_name;     // iteractive database name


    // Compartment info
    CompName_icmp = std::get<1>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp));  // name
    nx = std::get<2>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp));  // get domain dimensions                     
    ny = std::get<3>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp));
    nz = std::get<4>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp));

    // num of chemicals to print
    num_chem2print = OpenWQ_wqconfig.chem2print.size();


    // ########################################
    // Save ix, iy and iz in dataset (the first time)
    if (OpenWQ_wqconfig.print_xyz == true){

        for (unsigned int ichem=0;ichem<num_chem2print;ichem++){

            // Get chemical name (convert to char for use in .save())
            chem_name = OpenWQ_wqconfig.chem_species_list[
            OpenWQ_wqconfig.chem2print[ichem]].c_str();           // index of chemical to print

            // Reset file name for each compartment
            filename = OpenWQ_wqconfig.output_dir;

            filename.append("/");
            filename.append(CompName_icmp);
            filename.append("_");
            filename.append(chem_name);
            filename.append(".h5");

            // x elements
            internal_database_name = "x_elements";             // name for internal HDF5 database name
            arma::linspace(1,nx,nx)
                .save(arma::hdf5_name(
                    filename,
                    internal_database_name));                  // no append (to clean old file if existant)

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

        }
    }

    // ########################################
    // Save datasets for each time step

    // Initiate arma::mat to print data
    arma::mat filedata(nx*ny*nz,1);

    for (unsigned int ichem=0;ichem<num_chem2print;ichem++){

        // Get chemical name (convert to char for use in .save())
        chem_name = OpenWQ_wqconfig.chem_species_list[
        OpenWQ_wqconfig.chem2print[ichem]].c_str();           // index of chemical to print

        // Reset file name for each compartment
        filename = OpenWQ_wqconfig.output_dir;

        filename.append("/");
        filename.append(CompName_icmp);
        filename.append("_");
        filename.append(chem_name);
        filename.append(".h5");

        // Save
        (*OpenWQ_vars.chemass)
            (icmp)
            (OpenWQ_wqconfig.chem2print[ichem])
            .save(arma::hdf5_name(
                filename,                               // file name
                timestr,   // database name: chem name + time
                arma::hdf5_opts::append));              // options
    }

    return EXIT_SUCCESS;
}