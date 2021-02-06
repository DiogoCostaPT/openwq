
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

/* #################################################
// Initialize memory of major variables: arma::field
################################################# */
void OpenWQ_initiate::initmemory(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig){

    // Pointer to nx, ny, nx information (grids or HRU)
    std::unique_ptr<unsigned int[]> n_xyz(new unsigned int[3]); // pointer to nx, ny, nx information
    n_xyz[0] = 0;
    n_xyz[1] = 0;
    n_xyz[2] = 0;

    // Local variables
    std::string HydroCmpName;   // Hydrological compartment names as specified in main.cpp and 
                                // OpenWQ_json.openWQ_config.json (they need to match)
    int num_HydroComp = OpenWQ_hostModelconfig.HydroComp.size(); // number of hydrological compartments in host model
    
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

/* #################################################
// Initial conditions (water and chemical mass)
################################################# */
void OpenWQ_initiate::readSetIC(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    const int icmp,
    const int ix,
    const int iy,
    const int iz,
    const double igridcell_volume,  // all calculations assume unit = m3
    const double iwater_volume){    // all calculations assume unit = m3
    
    // Local variables
    int num_chem = OpenWQ_json.BGCcycling["CHEMICAL_SPECIES"]["list"].size(); // number of chemical species
    std::string chemname; // chemical name
    std::tuple<int,std::string,std::string> ic_info_i; // IC information in config file
    double ic_value; // IC value of chemical i
    std::string ic_type; // IC value type of chemical (mass or concentration)
    std::string ic_units; // // IC value units of chemical (e.g, kg/m3, mg/l))

     // Get chemical species list from BGC_json
    num_chem = OpenWQ_json.BGCcycling["CHEMICAL_SPECIES"]["list"].size();
    std::vector<std::string> chem_species_list;
    for (int chemi=0;chemi<num_chem;chemi++){
        chem_species_list.push_back(OpenWQ_json.BGCcycling["CHEMICAL_SPECIES"]
            ["list"][std::to_string(chemi+1)]);
    }

    // Find compartment icmp name from code (host hydrological model)
    std::string CompName_icmp = std::get<1>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp)); // Compartment icomp name

    /* ########################################
    // Loop over chemical species
    ######################################## */

    for (int chemi=0;chemi<num_chem;chemi++){

        chemname = chem_species_list[chemi]; // chemical name in BGC-json list

        // Get tuple with IC information for compartment CompName_icmp and chemical chemname
        // If not found in compartment icmp, it's because IC were not defined - set to zero.
        try{ // IC conditions provided
            ic_info_i = 
                OpenWQ_json.Config["BIOGEOCHEMISTRY_CONFIGURATION"][CompName_icmp]
                ["initial_conditions"][chemname];

            ic_value = std::get<0>(ic_info_i);      // get IC value
            ic_type = std::get<1>(ic_info_i);       // get IC type
            ic_units = std::get<2>(ic_info_i);      // get IC units 

            // Transform units based on info provided
            OpenWQ_initiate::Transform_Units(
                ic_value, // ic_value passed by reference so that it can be changed
                ic_type,
                ic_units);

            // Apply IC conditons (no need to handle error
            // Already done in OpenWQ_initiate::Transform_Units)
            // if CONCENTRATION
            if(ic_type.compare("concentration") == 0){
                (*OpenWQ_vars.chemass)(icmp)(chemi)(ix,iy,iz) =// units: g (basic units of MASS in openWQ)
                    ic_value // converted to mg/l (or g/m3) in OpenWQ_initiate::Transform_Units
                    * iwater_volume; // passed in m3
            }
            // if MASS
            else if(ic_type.compare("mass") == 0){
                (*OpenWQ_vars.chemass)(icmp)(chemi)(ix,iy,iz) =// units: g (basic units of MASS in openWQ)
                    ic_value // converted to mg/l (or g/m3) in OpenWQ_initiate::Transform_Units
                    * igridcell_volume; // passed in m3
            }

        }catch(json::type_error& e){ // If IC conditions are not provided
            std::cout << "IC conditions: not defined " 
                    << "(compartment: " << CompName_icmp << ", " 
                    << "chemical: " << chemname << ")"
                    << std::endl;
        }  
    }
}

/* #################################################
// Transform units of IC
################################################# */
void OpenWQ_initiate::Transform_Units(
    double &ic_value, // IC value of chemical (passed by reference)
    std::string ic_type, // IC value type of chemical (mass or concentration)
    std::string ic_unit){ // // IC value units of chemical (e.g, kg/m3, mg/l))

    // Local Variales
    double unit_convert_k = 1.0f;
    bool type_unkown_flag = false;
    bool unit_unkown_flag = false;


    // Convert ic_type and ic_unit to lower case to guarantee match
    std::transform(ic_type.begin(), ic_type.end(), ic_type.begin(),
        [](unsigned char c){ return std::tolower(c); });
    std::transform(ic_unit.begin(), ic_unit.end(), ic_unit.begin(),
        [](unsigned char c){ return std::tolower(c); });

    /* ########################################
    // Check type of IC (concentration or mass)
    ########################################## */

    // CONCENTRATION (Goal: convert to mg/l or g/m3 => openWQ internal units)
    if(ic_type.compare("concentration") == 0){
        
        // Default concentration units = mg/l or g/m3 (openWQ internal units)
        if (ic_unit.compare("mg/l") == 0 || ic_unit.compare("g/m3") == 0){
            unit_convert_k = 1.0f;
        }else if (ic_unit.compare("ug/l") == 0){
            unit_convert_k = 0.001;
        }else{ // ERROR: ic_unit unkown
            unit_unkown_flag = true;
        }
    }
    // MASS (Goal: convert to g => openWQ internal units)
    else if(ic_type.compare("mass") == 0){
        
        // Default mass units = g (openWQ internal units)
        if (ic_unit.compare("g/m3") == 0){ 
            unit_convert_k = 1.0f;
        }else if (ic_unit.compare("kg/m3") == 0){
            unit_convert_k = 1000;
        }else{ // ERROR: ic_tunit unkown
            unit_unkown_flag = true;
        }
    } 
    // ERROR: ic_type nkown
    else{
        type_unkown_flag = true;
    }

    /* ########################################
    // Error Handling
    ########################################## */
    if (type_unkown_flag || unit_unkown_flag){
        
        if (type_unkown_flag) 
            std::cout << "ERROR: Initial conditions type: unkown (" << ic_type << ")" << std::endl;
        if (unit_unkown_flag) 
            std::cout << "ERROR: Initial conditions unit: unkown (" << ic_unit << ")" << std::endl;
        exit (EXIT_FAILURE);
    }   

    /* ########################################
    // Convert units using unit_convert_k (ic_value passed by reference)
     ########################################## */
    ic_value *= unit_convert_k;

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