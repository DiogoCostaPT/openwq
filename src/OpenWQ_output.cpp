
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
    unsigned int ts){
    
    
    /* ########################################
    // Return if not time to print yet
    ######################################## */
    if (ts < OpenWQ_wqconfig.nexttime_out)
        return EXIT_SUCCESS;
    else
        OpenWQ_wqconfig.nexttime_out += OpenWQ_wqconfig.timetep_out;


    // CSV
    if (OpenWQ_wqconfig.output_type == 0){
            OpenWQ_output::writeCSV(
                OpenWQ_json,
                OpenWQ_vars,
                OpenWQ_hostModelconfig,
                OpenWQ_wqconfig,
                ts);

    // VTU
    }else if (OpenWQ_wqconfig.output_type == 1){
        OpenWQ_output::writeVTU(
            OpenWQ_json,
            OpenWQ_vars,
            OpenWQ_hostModelconfig,
            OpenWQ_wqconfig,
            ts); 
    }
}



// CSV output
int OpenWQ_output::writeCSV(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    unsigned int ts){ // time step (in seconds)

    // Local Variables
    unsigned int nx, ny, nz;                // compartment dimensions
    unsigned int num_chem2print;            // number of chemical species to print
    std::string CompName_icmp;              // compartment name (iteractive)
    std::string chem_name;                  // chemical name (iteractive)
    std::string filename;                   // iteractive output file name 
    std::vector<int>::iterator it;          // Iterator used to store the position of searched element
    int a;                                  // iteractive dummy variable for printing

    /* ########################################
    // Get necessary info
    ######################################## */

    // Number of chemical species to print (needed for output header)
    num_chem2print = OpenWQ_wqconfig.chem2print.size();

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
        filename.append(std::to_string(ts)); // time stamp
        filename.append("_");
        filename.append("sec");
        filename.append(".txt");

        // Initiate arma::mat to print data
        arma::mat filedata(nx*ny*nz,num_chem2print + 3); // starts with ix, iy and iz (so + 3)
        
        // Define header of file
        arma::field<std::string> header(num_chem2print + 3); // starts with ix, iy and iz (so + 3)
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
  
                    for (unsigned int ichem=0;ichem<num_chem2print;ichem++){

                        // Get chemical name
                        chem_name = OpenWQ_wqconfig.chem_species_list[
                            OpenWQ_wqconfig.chem2print[ichem]           // index of chemical to print
                            ];
                           
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
    }
    
}


int OpenWQ_output::writeVTU(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    unsigned int ts){ // time step (in seconds)

    // Local Variables
    unsigned int nx, ny, nz;                // compartment dimensions
    unsigned int index_i;                   // iteractive index
    unsigned int numvert, nnumel;           // iteractive variables for VTK
    std::string CompName_icmp;              // compartment name (iteractive)
    std::string filename;                   // iteractive output file name
    

    /* ########################################
    // Loop over comparments
    ######################################## */
    for (unsigned int icmp=0;icmp<OpenWQ_hostModelconfig.num_HydroComp;icmp++){

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
        filename.append(std::to_string(ts)); // time stamp
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
        for (unsigned int ichem=0;ichem<(OpenWQ_wqconfig.num_chem);ichem++){ // all chemical species

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
                                varexpot->SetValue(index_i, (*OpenWQ_vars.chemass)(icmp)(ichem)(ix,iy,iz));
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

    }

    return EXIT_SUCCESS;

}