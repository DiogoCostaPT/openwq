
// Copyright 2020, Diogo Costa (diogo.pinhodacosta@canada.ca)
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


#include "OpenWQ_initiate.h"


// Initialize memory of major variables: arma::field
void OpenWQ_initiate::initmemory(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    int num_HydroComp){

    // Pointer to nx, ny, nx information (grids or HRU)
    std::unique_ptr<unsigned int[]> n_xyz(new unsigned int[3]); // pointer to nx, ny, nx information
    n_xyz[0] = 0;
    n_xyz[1] = 0;
    n_xyz[2] = 0;

    // Local variables
    std::string HydroCmpName;   // Hydrological compartment names as specified in main.cpp and 
                                // OpenWQ_json.openWQ_config.json (they need to match)
    
    // Create arma for chemical species
    int numspec = OpenWQ_json.BGCcycling["CHEMICAL_SPECIES"]["list"].size(); // number of chemical species in BGCcycling
    typedef arma::field<arma::Cube<double>> arma_fieldcube; // typedef data structure: used for domain_field

    // Assign and  allocate memory to openWQ variables: chemass
    for (int icomp=0;icomp<num_HydroComp;icomp++){
            
        // Dimensions for compartment icomp
        n_xyz[0] = std::get<2>(OpenWQ_hostModelconfig.HydroComp.at(icomp)); // num of x elements
        n_xyz[1] = std::get<3>(OpenWQ_hostModelconfig.HydroComp.at(icomp)); // num of y elements
        n_xyz[2] = std::get<4>(OpenWQ_hostModelconfig.HydroComp.at(icomp)); // num of z elements
        
        // Generate arma::cube of compartment icomp size
        arma::Cube<double> domain_xyz(n_xyz[0],n_xyz[1],n_xyz[2]);

        // Get number of species in compartment icomp
        HydroCmpName = std::get<1>(OpenWQ_hostModelconfig.HydroComp.at(icomp));
        
        // Create arma for chemical species
        // Needs to be reset because each compartment can have a different number of compartments
        arma_fieldcube domain_field(numspec); // all species are simulated for all compartments

        // Push 3D arma::cube into the arma::field of each chemical species
        for (int s=0;s<numspec;s++){
            domain_field(s) = domain_xyz;
        }
        (*OpenWQ_vars.chemass)(icomp) = domain_field;
    }

}

/*
// read 3D col data from file at assign to variable
void OpenWQ_initiate::read_file_3Dcoldata(json & filejson,arma::Cube<double> & to_cubedata, int var_col, 
    std::string filename){

    // get necessary inf o from JSON file
    int skiprows_num = filejson["skip_header_rows"];
    std::string deliminter = filejson["deliminter"];
    std::vector<int> grid_col = filejson["grid_col"];
    double unit_convertion_multipler = filejson["unit_convertion_multipler"];
    

    const char * cdeliminter = deliminter.c_str();
    std::vector<int>::iterator it;

    // all relevant columns: grid and var
    std::vector<int> allcols_2search;
    allcols_2search.insert(allcols_2search.begin(),grid_col.begin(),grid_col.end());
    allcols_2search.insert(allcols_2search.end(),var_col); 

    // Create a vector of <string, int vector> pairs to store the FileData_extract
    std::vector<std::pair<std::string, std::vector<double>>> FileData_extract;

    // Create an input filestream
    std::ifstream thisFile(filename);

    // Make sure the file is open
    if(!thisFile.is_open()) throw std::runtime_error("Could not open file:" + filename);

    // Helper vars
    std::string line, fieldi;
    int line_i = 0;

        // Read data, line by line AND save to FileData_extrac (for proper debug) and to final to_cubedata
    int colIdx,colIdx_res;
    std::array<double,4> linedata; linedata.fill(0.0f);

     if(thisFile.good())
    {

        while(std::getline(thisFile, line))
        {
            // Create a stringstream of the current line
            std::stringstream ss(line);
        
            // Keep track of the current column index
            colIdx = 1; 
            colIdx_res = 0;

            if (line_i>=skiprows_num){ // skip header
            
                // Extract each integer
                while(std::getline(ss, fieldi, *cdeliminter)){
                    
                    it = std::find(allcols_2search.begin(), allcols_2search.end(), colIdx); // check if column of interest

                    // Add the current integer to the 'colIdx' column's values vector
                    if (it != allcols_2search.end()){  // skip header
                                               
                        linedata[std::distance(allcols_2search.begin(),it)] = std::stod(fieldi);
                        colIdx_res++;
                    }                             
                    
                    colIdx++; // Increment the column index
                }
                (to_cubedata)(linedata[0],linedata[1],linedata[2]) = linedata[3] * unit_convertion_multipler; // save to to_cubedata
            }
            line_i++;
        }
    }

    // Close file
    thisFile.close();
}
*/

/*
// Initial conditions (water and chemical mass)
void OpenWQ_initiate::readSetIC(OpenWQ_json& OpenWQ_json,OpenWQ_vars& OpenWQ_vars){

    int num_HydroComp = OpenWQ_json.H2O["compartments"].size();
    int numspec;
    std::string filepath;
    
    for (int i=0;i<num_HydroComp;i++){

        // wmass
        //read_file_3Dcoldata(OpenWQ_json.H2O[std::to_string(i+1)]["IC_file"],
        //    (*OpenWQ_vars.wmass)(i),OpenWQ_json.H2O[std::to_string(i+1)]["IC_file"]["var_col"],
        //    OpenWQ_json.H2O[std::to_string(i+1)]["IC_file"]["file_path"]);
        
        // chemass
        numspec = OpenWQ_json.WQ["compartments"][std::to_string(i+1)]["chem_species"].size();
        std::vector<std::string> chemnames = 
            OpenWQ_json.WQ["compartments"][std::to_string(i+1)]["chem_species"]; //chem species # within compartment icmp (from JSON.WQ)
        
        for (int chem_i=0;chem_i<numspec;chem_i++){
            read_file_3Dcoldata(OpenWQ_json.WQ["compartments"][std::to_string(i+1)][std::to_string(chem_i+1)]["IC_file"],(*OpenWQ_vars.chemass)(i)(chem_i),
                OpenWQ_json.WQ["compartments"][std::to_string(i+1)][std::to_string(chem_i+1)]["IC_file"]["var_col"],
                OpenWQ_json.WQ["compartments"][std::to_string(i+1)][std::to_string(chem_i+1)]["IC_file"]["file_path"]);
        }
    }
}
*/

// Reads JSON files and initiated data structures
void OpenWQ_initiate::initiate(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    int num_HydroComp){
  
    // Initialize memmory for major arma::field variables
    OpenWQ_initiate::initmemory(
        OpenWQ_json,
        OpenWQ_vars,
        OpenWQ_hostModelconfig,
        num_HydroComp);

    /*
    // IC (water and chemical mass)
    readSetIC(
        OpenWQ_json,
        OpenWQ_vars);
    */

}