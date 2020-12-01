
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


#include "DEMOS_OpenWQ_start.h"


// Read JSON file to class
void DEMOS_OpenWQ_start::read_JSON_2class(json & jsondata,const std::string& jsonfile)
{
        try{
                std::ifstream i(jsonfile);
                i >> (jsondata);

        }catch (json::type_error){
                std::cout << "An exception occurred parsing" << jsonfile << std::endl;
                abort();
        }
}


// Initialize memory of major variables: arma::field
void DEMOS_OpenWQ_start::initmemory(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json,
        DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars)
{
        // domain/water (number of compartments and grid dimensions)
        int numcmp = DEMOS_OpenWQ_json.H2O["compartments"].size();
        std::unique_ptr<unsigned int[]> n_xyz(new unsigned int[3]); // pointer to nx, ny, nx information
        n_xyz[0] = 0;
        n_xyz[1] = 0;
        n_xyz[2] = 0;
        int numInter = DEMOS_OpenWQ_json.CMPI["interactions"].size();

        // BGC
        int numlistspec = DEMOS_OpenWQ_json.BGC["list_chemical_species"].size(); // size of list_chemical_speciess
        int numspec_j;

        // Assign and  allocate memory: wflux,wmassk,chemass
        for (int i=0;i<numcmp;i++){
                
            // dimensions
            n_xyz[0] = DEMOS_OpenWQ_json.H2O[std::to_string(i+1)]["nx"];
            n_xyz[1] = DEMOS_OpenWQ_json.H2O[std::to_string(i+1)]["ny"];
            n_xyz[2] = DEMOS_OpenWQ_json.H2O[std::to_string(i+1)]["nz"];
            
            // wmass fluxes arma::cube
            arma::Cube<double> domain_xyz(n_xyz[0],n_xyz[1],n_xyz[2]);
            (*DEMOS_OpenWQ_vars.wmass)(i) = domain_xyz; // fwater fluxes arma::cube

            // wflux
            arma::field<arma::Cube<double>> domain_field(3); // for x, y, z
            for (int d=0;d<3;d++){ 
                domain_field(d) = domain_xyz;
            }
                (*DEMOS_OpenWQ_vars.wflux)(i) = domain_field;

            // chemass
            numspec_j = DEMOS_OpenWQ_json.WQ["compartments"][std::to_string(i+1)]["chem_species"].size(); 

            // creating arma::field of arma:c:cube of size numspec_j-> copies of domain_xyz for each species
            arma::field<arma::Cube<double>> domain_field_2(numspec_j);
            for (int s=0;s<numspec_j;s++){
                domain_field_2(s) = domain_xyz;
            }
            (*DEMOS_OpenWQ_vars.chemass)(i) = domain_field_2;
        }

}

// read 3D col data from file at assign to variable
void DEMOS_OpenWQ_start::read_file_3Dcoldata(json & filejson,arma::Cube<double> & to_cubedata, int var_col, 
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

// Initial conditions (water and chemical mass)
void DEMOS_OpenWQ_start::readSetIC(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars){

    int numcmp = DEMOS_OpenWQ_json.H2O["compartments"].size();
    int numspec;
    std::string filepath;
    
    for (int i=0;i<numcmp;i++){

        // wmass
        read_file_3Dcoldata(DEMOS_OpenWQ_json.H2O[std::to_string(i+1)]["IC_file"],
            (*DEMOS_OpenWQ_vars.wmass)(i),DEMOS_OpenWQ_json.H2O[std::to_string(i+1)]["IC_file"]["var_col"],
            DEMOS_OpenWQ_json.H2O[std::to_string(i+1)]["IC_file"]["file_path"]);
        
        // chemass
        numspec = DEMOS_OpenWQ_json.WQ["compartments"][std::to_string(i+1)]["chem_species"].size();
        std::vector<std::string> chemnames = 
            DEMOS_OpenWQ_json.WQ["compartments"][std::to_string(i+1)]["chem_species"]; //chem species # within compartment icmp (from JSON.WQ)
        
        for (int chem_i=0;chem_i<numspec;chem_i++){
            read_file_3Dcoldata(DEMOS_OpenWQ_json.WQ["compartments"][std::to_string(i+1)][std::to_string(chem_i+1)]["IC_file"],(*DEMOS_OpenWQ_vars.chemass)(i)(chem_i),
                DEMOS_OpenWQ_json.WQ["compartments"][std::to_string(i+1)][std::to_string(chem_i+1)]["IC_file"]["var_col"],
                DEMOS_OpenWQ_json.WQ["compartments"][std::to_string(i+1)][std::to_string(chem_i+1)]["IC_file"]["file_path"]);
        }
    }
}

// Call function to (1) read JSONs
void DEMOS_OpenWQ_start::initiate(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json,const std::string configjson_file){

        // Real all configuration files
        read_JSON_2class(DEMOS_OpenWQ_json.Master,configjson_file); // master file
        read_JSON_2class(DEMOS_OpenWQ_json.H2O,DEMOS_OpenWQ_json.Master["water_balance_setup_file"]); // H2O file
        read_JSON_2class(DEMOS_OpenWQ_json.CMPI,DEMOS_OpenWQ_json.Master["cmp_interaction_file"]); // CMP file
        read_JSON_2class(DEMOS_OpenWQ_json.WQ,DEMOS_OpenWQ_json.Master["wq_balance_setup_file"]); // WQ file
        read_JSON_2class(DEMOS_OpenWQ_json.BGC,DEMOS_OpenWQ_json.Master["BGC_cycling_setup_file"]); // BGC file

        // Assign the main arma::field variables
        int numcmp = DEMOS_OpenWQ_json.H2O["compartments"].size(); // number of compartments
        int numinteract = DEMOS_OpenWQ_json.CMPI["interactions"].size(); // number of interactions between compartments
        DEMOS_OpenWQ_vars DEMOS_OpenWQ_vars(numcmp,numinteract);

        // Initialize memmory for major arma::field variables
        initmemory(DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars);

        // IC (water and chemical mass)
        readSetIC(DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars);

}

