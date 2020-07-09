
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
        int numInter = JSONfiles.CMPI["interactions"].size();

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
            numspec_j = JSONfiles.WQ["compartments"][std::to_string(i+1)]["chem_species"].size(); 

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
void read_file_3Dcoldata(json & filejson,arma::Cube<double> & to_cubedata, int var_col, 
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

// Read inter fluxes file data
void read_file_CMPIcoldata(JSONfiles & JSONfiles,Prj_StateVar& Prj_StateVar, int iteraction, int source, 
    int recipient, std::string filename, std::string exchange_type){

    // get necessary inf o from JSON file
    int skiprows_num = JSONfiles.CMPI[std::to_string(iteraction+1)]["mapping_file"]["skip_header_rows"];
    std::string deliminter = JSONfiles.CMPI[std::to_string(iteraction+1)]["mapping_file"]["deliminter"];
    std::vector<int> grid_col_send = JSONfiles.CMPI[std::to_string(iteraction+1)]["mapping_file"]["grid_col_send"];
    std::vector<int> grid_col_receive = JSONfiles.CMPI[std::to_string(iteraction+1)]["mapping_file"]["grid_col_receive"];
    int var_col;
    int num_cols = 9;
    double unit_convertion_multipler;
    
    // var_col only exists for exchange_type = water_flux
    if (exchange_type.compare("water_flux")==0){
        var_col = JSONfiles.CMPI[std::to_string(iteraction+1)]["mapping_file"]["var_col"];
        unit_convertion_multipler = JSONfiles.CMPI[std::to_string(iteraction+1)]["mapping_file"]["unit_convertion_multipler"];
    }

    const char * cdeliminter = deliminter.c_str();
    std::vector<int>::iterator it;

    // all relevant columns: grid and var
    std::vector<int> allcols_2search;
    allcols_2search.insert(allcols_2search.begin(),grid_col_send.begin(),grid_col_send.end());
    allcols_2search.insert(allcols_2search.begin(),grid_col_receive.begin(),grid_col_receive.end());
    if (exchange_type.compare("water_flux")==0) // search this extra column if it has a water flux
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

    // Get number of rows to allocate memmory 
    thisFile.seekg (0, thisFile.end);
    int length = thisFile.tellg();
    thisFile.seekg (0, thisFile.beg); // return to begining of file
   
    // create ama::Mat with correct number of dimensions
    arma::Mat<double> to_matdata(length,num_cols);

        // Read data, line by line AND save to FileData_extrac (for proper debug) and to final to_cubedata
    int colIdx,colIdx_res;
    std::array<double,9> linedata; linedata.fill(0.0f);

     if(thisFile.good())
    {

        while(std::getline(thisFile, line))
        {
            // Create a stringstream of the current line
            std::stringstream ss(line);
        
            // Keep track of the current column index
            colIdx = 0; 
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
                (to_matdata)(colIdx,0) = source;
                (to_matdata)(colIdx,1) = linedata[0];
                (to_matdata)(colIdx,2) = linedata[1];
                (to_matdata)(colIdx,3) = linedata[2];
                (to_matdata)(colIdx,4) = recipient;
                (to_matdata)(colIdx,5) = linedata[3];
                (to_matdata)(colIdx,6) = linedata[4];
                (to_matdata)(colIdx,7) = linedata[5];

                if (exchange_type.compare("water_flux")==0){
                    (to_matdata)(colIdx,8) = linedata[6] * unit_convertion_multipler; // save
                }else if (exchange_type.compare("chem_exchange")==0){ // chem exchange

                    std::string kinetics = JSONfiles.CMPI[std::to_string(iteraction+1)]["mapping_file"]["kinetics"];
                    std::vector<std::string> parameter_names = JSONfiles.CMPI[std::to_string(iteraction+1)]["mapping_file"]["parameter_names"];
                    std::vector<double> parameter_values = JSONfiles.CMPI[std::to_string(iteraction+1)]["mapping_file"]["parameter_values"];

                    linedata[6] = ChemCompExchange(JSONfiles, Prj_StateVar, source, kinetics, 
                        parameter_names, parameter_values);

                    (to_matdata)(colIdx,8) = linedata[6]; // save
                }
            }
            line_i++;
        }
    }

    (*Prj_StateVar.wchem_exch)(iteraction) = to_matdata;

    // Close file
    thisFile.close();
}



// Read all files inside the folder
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

// Read all files inside the folder
void  GetComptInteractFluxesFiles(JSONfiles& JSONfiles,std::vector<std::vector<std::string>> &compInt_filenames){

    std::string folder_path, interact_type;
    
    int numInter = JSONfiles.CMPI["interactions"].size();

    for (int it=1;it<=numInter;it++){
        
        interact_type = JSONfiles.CMPI[std::to_string(it)]["exchange_type"];
        std::vector<std::string> filenames_i;

        if (interact_type.compare("water_flux") == 0){
            folder_path = JSONfiles.CMPI[std::to_string(it)]["mapping_file"]["folder_path"];
            GetFilesInFolder(folder_path,filenames_i); // list the results files to get the last time step
            
        }else{
            filenames_i.push_back("NOT_FLUX_INTERACT");
        }
        compInt_filenames.push_back(filenames_i);
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
    try{
        free(entry);
    }catch(const std::exception& e){

    }
}

// Convert the filenames to double AND sort them
void ConvertSortFilenames2Double(int numtotal,
    std::vector<std::vector<std::string>>& filenames,
    std::vector<std::vector<double>>& filenames_num){

    std::string filename_ii;
    double simnum;

    for (int i=0;i<numtotal;i++){
        
        std::vector<std::string> filenames_i = filenames[i];
        std::vector<double> filenames_num_i;

        for(int i=0;i<filenames_i.size();i++){
            filename_ii = filenames_i[i];
           
            try{
                simnum = std::stod(filename_ii);
                filenames_num_i.push_back(simnum);
            } catch(const std::exception& e){
                std::cout << " a standard exception was caught in ConvertSortFilenames2Double, with message '"
                  << e.what() << "'\n";
            }
           
        }
        std::sort(filenames_num_i.begin(),filenames_num_i.end());
        filenames_num.push_back(filenames_num_i);
    }
}

// Indentify the mobile compartments
void IdentifyMobileCompt(std::vector<std::vector<double>> &filenames_num,
    std::vector<int> &mobileCompt){
  
    int numcmp = filenames_num.size();
    int numelem;

    // Identify the mobile compartments
    for (int icmp=0;icmp<numcmp;icmp++){
        numelem = filenames_num[icmp].size();
        if (numelem>0) mobileCompt.push_back(icmp);
    }

}


// Check if time steps match between fluxes folders
bool CheckIfCompTimeStepsMatch(std::vector<std::vector<double>> &filenames_num){

    bool tsm_flag = true;

    int num_datafolders = filenames_num.size(); // num of files with fluxes data
    int numfiles;
    std::vector<double> tmpst_num_all;
    std::vector<std::vector<double>> filenames_num_noempty;

    if (num_datafolders>1){
        tsm_flag = true;

        // Get the number of files inside each folder (intra and inter fluxes)
        for (int ifields=0;ifields<num_datafolders;ifields++){ // loop over compartmennts: compare all mobile_compartments with the first compartment
            numfiles = filenames_num[ifields].size();
            if (numfiles!=0){
                tmpst_num_all.push_back(numfiles);
                filenames_num_noempty.push_back(filenames_num[ifields]);
            }
        }
        
        for (int ifields=1;ifields<tmpst_num_all.size();ifields++){ // loop over compartmennts: compare all mobile_compartments with the first compartment
            
            for (int tmpst=0;tmpst<tmpst_num_all[ifields];tmpst++){ // loop over time steps (vector elements)
                
                if (filenames_num_noempty[ifields][tmpst] != filenames_num_noempty[0][tmpst]){
                    tsm_flag = false;
                    std::cout << "Timesteps mismatch at timestep: " + 
                        std::to_string(filenames_num_noempty[ifields][tmpst]) << std::endl;
                    break;
                }
            }
            if (tsm_flag==false) break;
        }
    }

    return tsm_flag;
}

// Get extention of extention
void GetFileExtension(std::string &str, std::string &str_extention){

    std::size_t pos = str.find(".");      // position of "." in str
    str_extention = str.substr (pos);     // get from "." to the end

}
