
// Copyright 2020, Diogo Costa
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


#include "utility.h"


// Initialize memory of major variables: arma::field
void initmemory(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar)
{
        // domain/water (number of compartments and grid dimensions)
        int numcmp = JSONfiles.H2O["compartments"].size();
        std::unique_ptr<unsigned int[]> n_xyz(new unsigned int[3]); // pointer to nx, ny, nx information
        n_xyz[0] = 0;
        n_xyz[1] = 0;
        n_xyz[2] = 0;

        // BGC
        int numlistspec = JSONfiles.BGC["list_chemical_species"].size(); // size of list_chemical_speciess
        int numspec_j;

        // Assign and  allocate memory: wflux,wmassk,chemass
               for (int i=0;i<numcmp;i++){
                       
                       // dimensions
                        n_xyz[0] = JSONfiles.H2O[std::to_string(i+1)]["nx"];
                        n_xyz[1] = JSONfiles.H2O[std::to_string(i+1)]["ny"];
                        n_xyz[2] = JSONfiles.H2O[std::to_string(i+1)]["nz"];
                        
                        // fwater fluxes arma::cube
                        arma::Cube<double> domain_xyz(n_xyz[0],n_xyz[1],n_xyz[2]);
                        (*Prj_StateVar.wflux)(i) = domain_xyz; // fwater fluxes arma::cube
                        (*Prj_StateVar.wmass)(i) = domain_xyz; // fwater fluxes arma::cube

                        // water quality
                        numspec_j = 0;
                        for (int j=0;j<numlistspec;j++){ 

                            // number of species in compartment j
                            numspec_j = JSONfiles.BGC["compartments"][std::to_string(i+1)]["chem_species"].size(); 

                            // creating arma::field of arma:c:cube of size numspec_j-> copies of domain_xyz for each species
                            arma::field<arma::Cube<double>> domain_spec(numspec_j);
                            for (int s=0;s<numspec_j;s++){
                                domain_spec(s) = domain_xyz;
                            }
                            (*Prj_StateVar.chemass)(i) = domain_spec;
                        }
        }
}


// Read JSON file to class
void read_JSON_2class(json & jsondata,const std::string& jsonfile)
{
        try{
                std::ifstream i(jsonfile);
                i >> (jsondata);

        }catch (json::type_error){
                std::cout << "An exception occurred parsing" << jsonfile << std::endl;
                abort();
        }
}


// Initial conditions (water and chemical mass)
void IC_calc(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar){

    int numcmp = JSONfiles.H2O["compartments"].size();
    std::string filepath;
    
    for (int i=0;i<numcmp;i++){

            // wmass
            read_file_3Dcoldata(JSONfiles.H2O[std::to_string(i+1)]["IC_water_massbalance_file"],
                (*Prj_StateVar.wmass)(i));

    }

}

// read 3D col data from file at assign to variable
void read_file_3Dcoldata(json & filejson,arma::Cube<double> & to_cubedata){

    // get necessary inf o from JSON file
    std::string filename = filejson["file_path"];
    int skiprows_num = filejson["skip_header_rows"];
    std::string deliminter = filejson["deliminter"];
    std::vector<int> grid_col = filejson["grid_col"];
    int var_col = filejson["var_col"];

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
    int colIdx = 0,line_i = 0;

    // Read the column names
    if(thisFile.good())
    {
        // Extract the first line in the file
        std::getline(thisFile, line);

        // Create a stringstream from line
        std::stringstream ss(line);

        // Extract each column name
        while(std::getline(ss, fieldi, *cdeliminter)){
            
            it = std::find(allcols_2search.begin(), allcols_2search.end(), colIdx + 1); // check if column of interest

            if (it != allcols_2search.end()){  // skip header    
                // Initialize and add <fieldi, int vector> pairs to FileData_extract
                FileData_extract.push_back({fieldi, std::vector<double> {}});
            } 
            colIdx++;
        }
       
    }

    // Read data, line by line AND save to FileData_extrac (for proper debug) and to final to_cubedata
    int colIdx_res;
    std::array<int,4> linedata; linedata.fill(0.0f);

    while(std::getline(thisFile, line))
    {
        // Create a stringstream of the current line
        std::stringstream ss(line);
       
        // Keep track of the current column index
        colIdx = 0; 
        colIdx_res = 0;

        if (line_i>=skiprows_num-1){ // skip header
        
            // Extract each integer
            while(std::getline(ss, fieldi, *cdeliminter)){
                
                it = std::find(allcols_2search.begin(), allcols_2search.end(), colIdx); // check if column of interest

                // Add the current integer to the 'colIdx' column's values vector
                if (it != allcols_2search.end()){  // skip header
                    FileData_extract.at(colIdx_res).second.push_back(std::stod(fieldi)); // save in vector (for proper debugging)
                    linedata[colIdx_res] = std::stod(fieldi);
                    colIdx_res++;
                }                             
                
                colIdx++; // Increment the column index
            }
            (to_cubedata)(linedata[0],linedata[1],linedata[2]) = linedata[3]; // save to to_cubedata
        }
        line_i++;
    }

    // Close file
    thisFile.close();

}