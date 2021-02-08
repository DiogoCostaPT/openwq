

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

#include "OpenWQ_sinksource.h"

/* #################################################
 // Check Sources and Sinks and Apply
 ################################################# */
void OpenWQ_sinksource::CheckApply(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    const unsigned int YYYY,                       // current model step: Year
    const unsigned int MM,                         // current model step: month
    const unsigned int DD,                         // current model step: day
    const unsigned int HH){                        // current model step: hour
    
    // Local variables
    unsigned int num_ssfiles;       // number of sink-source files 
                                    // (saved as sub-structure of SinkSource)
    unsigned int num_sschem;        // number of chemical loads per file
    std::string chemname_ssi;       // chemical name ssi
    std::string chemname_iter;      // chemical names from list (iteractive variable)
    unsigned int chemi_ssi;         // model index for chemical chemname_ssi
    std::string compartment_name;   // iteractive compartment name from JSON row data 
    unsigned int cmpi_ssi;          // model index for compartment compartment_name
    std::string ssType_ssi;         // Sink or Source
    unsigned int YYYY_json;         // Year in JSON-sink_source (interactive)
    unsigned int MM_json;           // Month in JSON-sink_source (interactive)
    unsigned int DD_json;           // Day in JSON-sink_source (interactive)
    unsigned int HH_json;           // Hour in JSON-sink_source (interactive)
    unsigned int num_rowdata;       // number of rows of data in JSON (YYYY, MM, DD, HH,...)
    unsigned int ix_json;           // iteractive ix info for sink-source row data 
    unsigned int iy_json;           // iteractive ix info for sink-source row data 
    unsigned int iz_json;           // iteractive ix info for sink-source row data 
    double ss_data_json;            // data (sink or source) from row data
    std::string ss_units_json;      // units of row data
    std::vector<std::string>::iterator find_i; // iteractor used to store the position or searched element
    std::vector<std::string> chem_species_list; // model chemical list
    std::vector<std::string> cmp_list;      // model compartment list

 
    // Get model comparment names list
    unsigned int num_cmp = OpenWQ_hostModelconfig.HydroComp.size();
    for (unsigned int ci=0;ci<num_cmp;ci++){
        cmp_list.push_back(
            std::get<1>(OpenWQ_hostModelconfig.HydroComp.at(ci))); // num of x elements 
    }
    
    // Get number of sub-structures of SinkSource
    num_ssfiles = OpenWQ_json.SinkSource.size(); 

    /* ########################################
    // Loop over file (saved as sub-structure of SinkSource)
    ######################################## */
    for (unsigned int ssf=0;ssf<num_ssfiles;ssf++){
        
        // Get number of loads in each sub-structure 
        // (corresponding to different sink_source json files)
        num_sschem = OpenWQ_json.SinkSource[std::to_string(ssf+1)].size();
        
        /* ########################################
        // Loop over loads per sub-structure
        ######################################## */
        for (unsigned int ssi=0;ssi<num_sschem;ssi++){

            // Look for chemicals, but there might be additional information such as METADATA
            try{

                // Chemical name json
                chemname_ssi = 
                    OpenWQ_json.SinkSource
                    [std::to_string(ssf+1)]
                    [std::to_string(ssi+1)]
                    ["Chemical_name"];

                // Look if that chemical name is in chem_species_list
                find_i = 
                    std::find(chem_species_list.begin(), 
                    chem_species_list.end(), 
                    chemname_ssi);

                // Find chemical model index
                if (find_i != chem_species_list.end()){
                    chemi_ssi =   find_i - chem_species_list.begin();
                }else{
                     std::cout 
                        << "ERROR (entry skipped): Chemical_name in source-sink file unkown: " 
                        << chemname_ssi 
                        << std::endl;
                }

            }catch(json::type_error){
                // If not chemical, just continue
                continue;
            }

            // Get type: source or sink
            ssType_ssi = OpenWQ_json.SinkSource
                [std::to_string(ssf+1)]
                [std::to_string(ssi+1)]
                ["Type"];
            std::transform(
                ssType_ssi.begin(),
                ssType_ssi.end(), 
                ssType_ssi.begin(), // Convert to lower case to avoid issues
                [](unsigned char c){ return std::tolower(c); });
            
            // Get number of rows of data in JSON (YYYY, MM, DD, HH,...)
            num_rowdata = OpenWQ_json.SinkSource
                    [std::to_string(ssf+1)]
                    [std::to_string(ssi+1)]
                    ["Data"].size();

            /* ########################################
             // Loop over row data in sink-source file
            ######################################## */

            for (unsigned int di=0;di<num_rowdata;di++){

                // Get Date in row di            
               YYYY_json =  OpenWQ_json.SinkSource // year
                    [std::to_string(ssf+1)]
                    [std::to_string(ssi+1)]
                    ["Data"]
                    [std::to_string(di+1)].at(0);
                MM_json = OpenWQ_json.SinkSource    // month
                    [std::to_string(ssf+1)]
                    [std::to_string(ssi+1)]
                    ["Data"]
                    [std::to_string(di+1)].at(1);
                DD_json =OpenWQ_json.SinkSource     // day
                    [std::to_string(ssf+1)]
                    [std::to_string(ssi+1)]
                    ["Data"]
                    [std::to_string(di+1)].at(2);
                HH_json =OpenWQ_json.SinkSource     // hour
                    [std::to_string(ssf+1)]
                    [std::to_string(ssi+1)]
                    ["Data"]
                    [std::to_string(di+1)].at(3);                               

                /* ########################################
                // Apply Source or Sink
                // Only if Dates match: model and Json-source-sink input
                ######################################## */

                if (YYYY == YYYY_json && MM == MM_json &&
                    DD == DD_json && HH == HH_json){
                    
                    // Get additional information about sink-source row data                    
                    compartment_name = OpenWQ_json.SinkSource // compartment name
                        [std::to_string(ssf+1)]
                        [std::to_string(ssi+1)]
                        ["Compartment"];
                    // chemname_ssi -> already obtained above // chemcial namea
                    ix_json = OpenWQ_json.SinkSource // ix 
                        [std::to_string(ssf+1)]
                        [std::to_string(ssi+1)]
                        ["Data"]
                        [std::to_string(di+1)].at(4);
                    iy_json = OpenWQ_json.SinkSource // iy
                        [std::to_string(ssf+1)]
                        [std::to_string(ssi+1)]
                        ["Data"]
                        [std::to_string(di+1)].at(5);
                    iz_json = OpenWQ_json.SinkSource // iz
                        [std::to_string(ssf+1)]
                        [std::to_string(ssi+1)]
                        ["Data"]
                        [std::to_string(di+1)].at(6);
                    ss_data_json = OpenWQ_json.SinkSource // sink/source data
                        [std::to_string(ssf+1)]
                        [std::to_string(ssi+1)]
                        ["Data"]
                        [std::to_string(di+1)].at(7);
                    ss_units_json = OpenWQ_json.SinkSource // sink/source data
                        [std::to_string(ssf+1)]
                        [std::to_string(ssi+1)]
                        ["Units"]; 

                    // Need to "- 1" for ix_json, iy_json, and iz_json because c++ starts at zero
                    ix_json -= 1;
                    iy_json -= 1;
                    iz_json -= 1;

                    // Get compartment index
                    find_i = 
                        std::find(cmp_list.begin(), 
                        cmp_list.end(), 
                        compartment_name);

                    // Find chemical model index
                    if (find_i != cmp_list.end()){
                        cmpi_ssi =   find_i - cmp_list.begin();
                    }else{
                        std::cout 
                            << "ERROR (entry skipped): Compartment name in source-sink file unkown: " 
                            << compartment_name 
                            << std::endl;
                        continue;   
                    }

                    // Convert units
                    // Source/sink units (g -> default model mass units)
                    OpenWQ_sinksource::Transform_SS_Units(
                        ss_data_json,      // SS value
                        ss_units_json);  // SS unit

                    /* ########################################
                    // if SOURCE
                    ######################################## */
                    if (ssType_ssi.compare("source") == 0){

                        OpenWQ_sinksource::Apply_Source(
                            OpenWQ_vars,
                            cmpi_ssi,           // compartment model index
                            chemi_ssi,       // chemical model index    
                            ix_json,            // compartment model ix
                            iy_json,            // compartment model iy
                            iz_json,            // compartment model iz
                            ss_data_json);      // source load

                    }
                    /* ########################################
                    // if SINK
                    ######################################## */
                    else if (ssType_ssi.compare("sink") == 0){
                        OpenWQ_sinksource::Apply_Sink(
                            OpenWQ_vars,
                            cmpi_ssi,           // compartment model index
                            chemi_ssi,          // chemical model index    
                            ix_json,            // compartment model ix
                            iy_json,            // compartment model iy
                            iz_json,            // compartment model iz
                            ss_data_json);      // source load g
                    }
                    // if none of the above (throw error)
                    else {
                        std::cout << "ERROR: Sink_Source type unkown: " << ssType_ssi << std::endl;
                        exit (EXIT_FAILURE);  
                    }
                }
            }
        }
    }
}


/* #################################################
 // Apply Source
 ################################################# */
void OpenWQ_sinksource::Apply_Source(
    OpenWQ_vars& OpenWQ_vars,
    const unsigned int cmpi,             // compartment model index
    const unsigned int chemi,            // chemical model index    
    const unsigned int ix,          // compartment model ix
    const unsigned int iy,          // compartment model iy
    const unsigned int iz,          // compartment model iz
    const double ss_data_json){          // source load g

    // Add mass load (already converted to g units)
    (*OpenWQ_vars.chemass)(cmpi)(chemi)(ix,iy,iz) += ss_data_json;

}

/* #################################################
 // Apply Sink
 ################################################# */
void OpenWQ_sinksource::Apply_Sink(
    OpenWQ_vars& OpenWQ_vars,
    const unsigned int cmpi,             // compartment model index
    const unsigned int chemi,            // chemical model index    
    const unsigned int ix,          // compartment model ix
    const unsigned int iy,          // compartment model iy
    const unsigned int iz,          // compartment model iz
    const double ss_data_json){          // source load g

    // Local Variables
    double mass_sink;

    mass_sink = std::fmax(ss_data_json,(*OpenWQ_vars.chemass)(cmpi)(chemi)(ix,iy,iz));
    
    // Add mass load (already converted to g units)
    (*OpenWQ_vars.chemass)(cmpi)(chemi)(ix,iy,iz) -= mass_sink;

}

/* #################################################
// Transform units of Source & Sinks
################################################# */
void OpenWQ_sinksource::Transform_SS_Units(
        double &ss_value,      // SS value
        std::string ss_unit){  // SS unit{      // model time step (in seconds)

    // Local Variales
    double unit_convert_k = 1.0f;
    bool unit_unkown_flag = false;

    // Convert ss_unit to lower case to guarantee match
    std::transform(ss_unit.begin(), ss_unit.end(), ss_unit.begin(),
        [](unsigned char c){ return std::tolower(c); });

    /* ########################################
    // Check Units
    ######################################### */
    // Default source/sink loads units = g (openWQ internal units of mass)
    if (ss_unit.compare("g") == 0){
        unit_convert_k = 1.0f;
    }else if (ss_unit.compare("kg") == 0){
        unit_convert_k = 1000.0f;;
    }else{ // ERROR: ss_unit unkown
        unit_unkown_flag = true;
    }

    /* ########################################
    // Error Handling
    ########################################## */
    if (unit_unkown_flag){
        std::cout 
            << "ERROR: Initial conditions unit: unkown (" 
            << ss_unit 
            << ")"
            << std::endl;
        exit (EXIT_FAILURE);
    }
  

    /* ########################################
    // Convert units using unit_convert_k (ic_value passed by reference)
     ########################################## */
    ss_value *= unit_convert_k;

}
