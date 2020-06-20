
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
                        
                        // wmass fluxes arma::cube
                        arma::Cube<double> domain_xyz(n_xyz[0],n_xyz[1],n_xyz[2]);
                        (*Prj_StateVar.wmass)(i) = domain_xyz; // fwater fluxes arma::cube

                        // wflux
                        arma::field<arma::Cube<double>> domain_field(3); // for x, y, z
                        for (int d=0;d<3;d++){ 
                            domain_field(d) = domain_xyz;
                        }
                         (*Prj_StateVar.wflux)(i) = domain_field;

                        // chemass
                        numspec_j = JSONfiles.BGC["compartments"][std::to_string(i+1)]["chem_species"].size(); 

                        // creating arma::field of arma:c:cube of size numspec_j-> copies of domain_xyz for each species
                        arma::field<arma::Cube<double>> domain_field_2(numspec_j);
                        for (int s=0;s<numspec_j;s++){
                            domain_field_2(s) = domain_xyz;
                        }
                        (*Prj_StateVar.chemass)(i) = domain_field_2;
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


// read 3D col data from file at assign to variable
void read_file_3Dcoldata(json & filejson,arma::Cube<double> & to_cubedata, int var_col, std::string filename){

    // get necessary inf o from JSON file
    int skiprows_num = filejson["skip_header_rows"];
    std::string deliminter = filejson["deliminter"];
    std::vector<int> grid_col = filejson["grid_col"];

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
    int colIdx = 1,line_i = 0;

    // Read the column names
    if(thisFile.good())
    {
        // Extract the first line in the file
        std::getline(thisFile, line);

        // Create a stringstream from line
        std::stringstream ss(line);

        // Extract each column name
        while(std::getline(ss, fieldi, *cdeliminter)){
            
            it = std::find(allcols_2search.begin(), allcols_2search.end(), colIdx); // check if column of interest

            if (it != allcols_2search.end()){  // skip header    
                // Initialize and add <fieldi, int vector> pairs to FileData_extract
                FileData_extract.push_back({fieldi, std::vector<double> {}});
            } 
            colIdx++;
        }
       
    }

    // Read data, line by line AND save to FileData_extrac (for proper debug) and to final to_cubedata
    int colIdx_res;
    std::array<double,4> linedata; linedata.fill(0.0f);

    while(std::getline(thisFile, line))
    {
        // Create a stringstream of the current line
        std::stringstream ss(line);
       
        // Keep track of the current column index
        colIdx = 1; 
        colIdx_res = 0;

        if (line_i>=skiprows_num-1){ // skip header
        
            // Extract each integer
            while(std::getline(ss, fieldi, *cdeliminter)){
                
                it = std::find(allcols_2search.begin(), allcols_2search.end(), colIdx); // check if column of interest

                // Add the current integer to the 'colIdx' column's values vector
                if (it != allcols_2search.end()){  // skip header
                    FileData_extract.at(colIdx_res).second.push_back(std::stod(fieldi)); // save in vector (for proper debugging)
                    
                    linedata[std::distance(allcols_2search.begin(),it)] = std::stod(fieldi);
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

// Read all files inside the JSON:H2O[water_fluxes_files][folder_path]
void GetFluxesFiles(JSONfiles& JSONfiles,std::vector<std::vector<std::string>> &fluxes_filenames){

    std::string folder_path;
    bool mobile;
    
    int numcmp = JSONfiles.H2O["compartments"].size();

    for (int icmp=1;icmp<=numcmp;icmp++){
        
        mobile = JSONfiles.H2O[std::to_string(icmp)]["mobile"];
        std::vector<std::string> filenames_i;

        if (mobile){
            folder_path = JSONfiles.H2O[std::to_string(icmp)]["water_fluxes_files"]["folder_path"];
            GetFilesInFolder(folder_path,filenames_i); // list the results files to get the last time step
            
        }else{
            filenames_i.push_back("NOT_MOBILE");
        }
        fluxes_filenames.push_back(filenames_i);
    }
}


/* *****
* Read file names in Results directory 
***** */
void GetFilesInFolder(std::string &path,
        std::vector<std::string> &filenames_i){

    struct dirent *entry;
    int i, timestart, filenum = 0, simnum;
    std::string filename_i;
 
    DIR *dir = opendir((path).c_str());
    
    if (dir != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            filenames_i.push_back(entry->d_name); // storing the file names
            filenum = filenum + 1;
        }
    }
    closedir(dir);
    free(entry);
}

// Convert the fluxes filenames to double AND sort them
void ConvertSortFluxesFilenames2Double(JSONfiles& JSONfiles,
    std::vector<std::vector<std::string>>& fluxes_filenames,
    std::vector<std::vector<double>>& fluxes_filenames_num){

    int numcmp = JSONfiles.H2O["compartments"].size();
    std::string filename_ii;
    double simnum;

    for (int icmp=0;icmp<numcmp;icmp++){
        
        std::vector<std::string> filenames_i = fluxes_filenames[icmp];
        std::vector<double> fluxes_filenames_num_i;

        for(int i=0;i<filenames_i.size();i++){
            filename_ii = filenames_i[i];
           
            try{
                simnum = std::stod(filename_ii);
                fluxes_filenames_num_i.push_back(simnum);
            } catch(const std::exception& e){
                std::cout << " a standard exception was caught in ConvertSortFluxesFilenames2Double, with message '"
                  << e.what() << "'\n";
            }
           
        }
        std::sort(fluxes_filenames_num_i.begin(),fluxes_filenames_num_i.end());
        fluxes_filenames_num.push_back(fluxes_filenames_num_i);
    }
}