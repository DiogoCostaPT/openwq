
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
void OpenWQ_sinksource::SetSinkSource(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_units& OpenWQ_units){ 
    
    // Local variables
    std::vector<std::string> cmp_list;      // model compartment list
    unsigned int cmpi_ssi;          // model index for compartment Compartment_name_name
    
    unsigned int num_ssfiles;       // number of sink-source files 
                                    // (saved as sub-structure of SinkSource)
    unsigned int num_sschem;        // number of chemical loads per file
    unsigned int num_rowdata;       // number of rows of data in JSON (YYYY, MM, DD, HH,...)
    int int_n_elem;                 // iterative counting of number of elements in arma:mat
    
    typedef std::tuple<
        std::string,
        std::string,
        std::string,
        std::string,
        arma::Mat<double>
        > SinkSourceTuple; // Tuple for Sink and Source Data

    std::string Chemical_name;  // from JSON file
    std::string Compartment_name;    // from JSON file
    std::string Type;           // from JSON file
    std::string Units;          // from JSON file

    unsigned int YYYY_json;         // Year in JSON-sink_source (interactive)
    unsigned int MM_json;           // Month in JSON-sink_source (interactive)
    unsigned int DD_json;           // Day in JSON-sink_source (interactive)
    unsigned int HH_json;           // Hour in JSON-sink_source (interactive)
    int ix_json;           // iteractive ix info for sink-source row data 
    int iy_json;           // iteractive ix info for sink-source row data 
    int iz_json;           // iteractive ix info for sink-source row data 
    double ss_data_json;            // data (sink or source) from row data
    std::string ss_units_json;      // units of row data
    std::vector<std::string>::iterator find_i; // iteractor used to store the position or searched element

    arma::Mat<double> sinksource_matrix; // matrix storing blocks of sink source data
    arma::vec row_data_col;     // new row data (initially as col data)
    arma::Mat<double> row_data_row;     // for conversion of row_data_col to row data
    
    // Get model comparment names list
    unsigned int num_cmp = OpenWQ_hostModelconfig.HydroComp.size();
    for (unsigned int ci=0;ci<num_cmp;ci++){
        cmp_list.push_back(
            std::get<1>(
                OpenWQ_hostModelconfig.HydroComp.at(ci))); // num of x elements 
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
            
            // needs this here because there can be entries that are not relevant e.g. COMMENTS
            try{
                Chemical_name =  OpenWQ_json.SinkSource // chemical name
                    [std::to_string(ssf+1)]
                    [std::to_string(ssi+1)]
                    ["Chemical_name"];
            }catch(json::type_error){   
                continue;
            }

            Compartment_name = OpenWQ_json.SinkSource // compartment name
                [std::to_string(ssf+1)]
                [std::to_string(ssi+1)]
                ["Compartment_name"];

            Type = OpenWQ_json.SinkSource // type (sink or source)
                [std::to_string(ssf+1)]
                [std::to_string(ssi+1)]
                ["Type"];

            Units = OpenWQ_json.SinkSource // units
                [std::to_string(ssf+1)]
                [std::to_string(ssi+1)]
                ["Type"];

            // Get number of rows of data in JSON (YYYY, MM, DD, HH,...)
            num_rowdata = OpenWQ_json.SinkSource
                [std::to_string(ssf+1)]
                [std::to_string(ssi+1)]
                ["Data"].size();

            /* ########################################
             // Loop over row data in sink-source file
            ######################################## */

            sinksource_matrix.reset(); // Reset the size to zero (the object will have no elements)

            for (unsigned int di=0;di<num_rowdata;di++){
                
                // Reset the size to zero (the object will have no elements)
                row_data_col.reset();
                row_data_row.reset();

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
                ix_json = std::max(ix_json - 1,0);
                iy_json = std::max(iy_json - 1,0);
                iz_json = std::max(iz_json - 1,0);
                
                // Get compartment index
                find_i = 
                    std::find(cmp_list.begin(), 
                    cmp_list.end(), 
                    Compartment_name);

                if (find_i != cmp_list.end()){
                    cmpi_ssi =   find_i - cmp_list.begin();
                }else{
                    std::cout 
                        << "<OpenWQ> ERROR (entry skipped): Compartment name in source-sink file unkown: " 
                        << Compartment_name
                        << std::endl;
                    return;   
                }

                // Convert units
                // Source/sink units (g -> default model mass units)
                OpenWQ_units.Convert_SS_Units(
                    ss_data_json,      // SS value
                    ss_units_json);  // SS unit

                // Get the vector with the data
                row_data_col = {
                    YYYY_json,
                    MM_json,
                    DD_json,
                    HH_json,
                    ix_json,
                    iy_json,
                    ix_json,
                    ss_data_json};

                // Transpose vector for adding to sinksource_matrix as a new row
                row_data_row = row_data_col.t();

                // Add new row_data_row to sinksource_matrix   
                int_n_elem = sinksource_matrix.n_rows;                     
                sinksource_matrix.insert_rows(
                    std::max(int_n_elem-1,0),
                    row_data_row); 
            }

            // Add to tuple
            (*OpenWQ_wqconfig.SinkSource_FORC).push_back(
                SinkSourceTuple(
                    Chemical_name,
                    Compartment_name,
                    Type,
                    Units,
                    sinksource_matrix));

        }
    }
}

/* #################################################
 // Check Sources and Sinks and Apply
 ################################################# */
void OpenWQ_sinksource::CheckApply(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_units& OpenWQ_units,
    const unsigned int YYYY,                       // current model step: Year
    const unsigned int MM,                         // current model step: month
    const unsigned int DD,                         // current model step: day
    const unsigned int HH){                        // current model step: hour
    
    // Local variables
    unsigned int num_ssfiles;       // number of sink-source files 
                                    // (saved as sub-structure of SinkSource)
    unsigned int num_sschem;        // number of chemical loads per file
    unsigned int num_rowdata;       // number of rows of data in JSON (YYYY, MM, DD, HH,...)
    unsigned int YYYY_json;         // Year in JSON-sink_source (interactive)
    unsigned int MM_json;           // Month in JSON-sink_source (interactive)
    unsigned int DD_json;           // Day in JSON-sink_source (interactive)
    unsigned int HH_json;           // Hour in JSON-sink_source (interactive)
    
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

                if (YYYY == YYYY_json){
                        
                    MM_json = OpenWQ_json.SinkSource    // month
                        [std::to_string(ssf+1)]
                        [std::to_string(ssi+1)]
                        ["Data"]
                        [std::to_string(di+1)].at(1);
                    
                        if (MM == MM_json){

                        DD_json =OpenWQ_json.SinkSource     // day
                            [std::to_string(ssf+1)]
                            [std::to_string(ssi+1)]
                            ["Data"]
                            [std::to_string(di+1)].at(2);
                        
                            if (DD == DD_json){

                            HH_json =OpenWQ_json.SinkSource     // hour
                                [std::to_string(ssf+1)]
                                [std::to_string(ssi+1)]
                                ["Data"]
                                [std::to_string(di+1)].at(3);      

                            if (HH == HH_json){   

                                Apply(
                                    OpenWQ_json,
                                    OpenWQ_vars,
                                    OpenWQ_hostModelconfig,
                                    OpenWQ_wqconfig,
                                    OpenWQ_units,
                                    ssf,    // source-sink file
                                    ssi,    // load section
                                    di);    // data row

                            }

                        }
                    }
                }
            }
        }
    }
}

void OpenWQ_sinksource::Apply(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_units& OpenWQ_units,
    const unsigned int ssf,     // source-sink file
    const unsigned int ssi,     // load section                 
    const unsigned int di){     // data row
    
    // Local variables
    std::string chemname_ssi;       // chemical name ssi
    unsigned int chemi_ssi;         // model index for chemical chemname_ssi
    std::string compartment_name;   // iteractive compartment name from JSON row data 
    unsigned int cmpi_ssi;          // model index for compartment compartment_name
    std::string ssType_ssi;         // Sink or Source
    unsigned int ix_json;           // iteractive ix info for sink-source row data 
    unsigned int iy_json;           // iteractive ix info for sink-source row data 
    unsigned int iz_json;           // iteractive ix info for sink-source row data 
    double ss_data_json;            // data (sink or source) from row data
    std::string ss_units_json;      // units of row data
    std::vector<std::string>::iterator find_i; // iteractor used to store the position or searched element
    std::vector<std::string> cmp_list;      // model compartment list

                              
    /* ########################################
    // Check if chemical exists
    ######################################## */
    try{

        // Chemical name json
        chemname_ssi = 
            OpenWQ_json.SinkSource
            [std::to_string(ssf+1)]
            [std::to_string(ssi+1)]
            ["Chemical_name"];

        // Look if that chemical name is in chem_species_list
        find_i = 
            std::find((OpenWQ_wqconfig.chem_species_list).begin(), 
            (OpenWQ_wqconfig.chem_species_list).end(), 
            chemname_ssi);

        // Find chemical model index
        if (find_i != (OpenWQ_wqconfig.chem_species_list).end()){
            chemi_ssi =   find_i - (OpenWQ_wqconfig.chem_species_list).begin();
        }else{
            std::cout 
                << "<OpenWQ> ERROR (entry skipped): Chemical_name in source-sink file unkown: " 
                << chemname_ssi 
                << std::endl;
        }

    }catch(json::type_error){
        // If not chemical, just continue
        return;
    }            

    /* ########################################
    // Apply Source or Sink
    // Only if Dates match: model and Json-source-sink input
    ######################################## */

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
    
    // Get model comparment names list
    unsigned int num_cmp = OpenWQ_hostModelconfig.HydroComp.size();
    for (unsigned int ci=0;ci<num_cmp;ci++){
        cmp_list.push_back(
            std::get<1>(OpenWQ_hostModelconfig.HydroComp.at(ci))); // num of x elements 
    }

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
            << "<OpenWQ> ERROR (entry skipped): Compartment name in source-sink file unkown: " 
            << compartment_name 
            << std::endl;
        return;   
    }

    // Convert units
    // Source/sink units (g -> default model mass units)
    OpenWQ_units.Convert_SS_Units(
        ss_data_json,      // SS value
        ss_units_json);  // SS unit

    /* ########################################
    // if SOURCE
    ######################################## */
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
        std::cout << "<OpenWQ> ERROR: Sink_Source type unkown: " << ssType_ssi << std::endl;
        exit (EXIT_FAILURE);  
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
