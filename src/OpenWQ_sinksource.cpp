
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
    OpenWQ_units& OpenWQ_units,
    OpenWQ_output& OpenWQ_output){ 
    
    // Local variables
    bool foundflag = false;                 // iteractive boolean to identify if comp or chem was found
    std::vector<std::string> cmp_list;      // model compartment list
    std::vector<std::string> chem_list;     // model chemical list
    std::string err_text;                   // iteractive string for text to pass to error messages
    unsigned long cmpi_ssi;                 // model index for compartment Compartment_name_name
    unsigned long chem_ssi;                 // model index for compartment Compartment_name_name
    unsigned long sinksource_ssi;           // = 0 (source), = 1 (sink)

    unsigned int num_ssfiles;               // number of sink-source files 
                                            // (saved as sub-structure of SinkSource)
    unsigned int num_sschem;                // number of chemical loads per file
    unsigned int num_rowdata;               // number of rows of data in JSON (YYYY, MM, DD, HH,...)
    int int_n_elem;                         // iterative counting of number of elements in arma:mat
    
    std::string Chemical_name;              // from JSON file
    std::string Compartment_name;           // from JSON file
    std::string Type;                       // from JSON file
    std::string Units;                      // from JSON file

    unsigned int YYYY_json;                 // Year in JSON-sink_source (interactive)
    unsigned int MM_json;                   // Month in JSON-sink_source (interactive)
    unsigned int DD_json;                   // Day in JSON-sink_source (interactive)
    unsigned int HH_json;                   // Hour in JSON-sink_source (interactive)
    unsigned int MIN_json;                  // Minutes in JSON-sink_source (interactive)
    int ix_json;                            // iteractive ix info for sink-source row data 
    int iy_json;                            // iteractive iy info for sink-source row data 
    int iz_json;                            // iteractive iz info for sink-source row data 
    double ss_data_json;                    // data (sink or source) from row data
    std::string ss_units_json;              // units of row data
    std::vector<double> unit_multiplers;    // multiplers (numerator and denominator)
   
    arma::vec row_data_col;                 // new row data (initially as col data)
    arma::Mat<double> row_data_row;         // for conversion of row_data_col to row data

    
    // Get model comparment names list
    unsigned int num_cmp = OpenWQ_hostModelconfig.HydroComp.size();
    for (unsigned int ci=0;ci<num_cmp;ci++){
        cmp_list.push_back(
            std::get<1>(
                OpenWQ_hostModelconfig.HydroComp.at(ci))); // num of x elements 
    }

    // Get model chemical names list
    unsigned int BGC_general_num_chem = OpenWQ_wqconfig.BGC_general_num_chem;
    for (unsigned int chemi=0;chemi<BGC_general_num_chem;chemi++){
        chem_list.push_back(
            (OpenWQ_wqconfig.BGC_general_chem_species_list)[chemi]);
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

        foundflag = false;      // reset to false at every 

        for (unsigned int ssi=0;ssi<num_sschem;ssi++){
            
            /* ########
            // Get chemical name, compartment name and SS_type (source or sink)
            ###########*/

            // needs this here because there can be entries that are not relevant e.g. COMMENTS
            try{
                Chemical_name =  OpenWQ_json.SinkSource // chemical name
                    [std::to_string(ssf+1)]
                    [std::to_string(ssi+1)]
                    ["CHEMICAL_NAME"];

            }catch(json::type_error){   
                continue;
            }

            Compartment_name = OpenWQ_json.SinkSource // compartment name
                [std::to_string(ssf+1)]
                [std::to_string(ssi+1)]
                ["COMPARTMENT_NAME"];

            Type = OpenWQ_json.SinkSource // type (sink or source)
                [std::to_string(ssf+1)]
                [std::to_string(ssi+1)]
                ["TYPE"];
            

            /* ########
            // Check if the requests are valid
            // chemical name, compartment name and SS_type (source or sink)
            ###########*/

            // Get chemical index
            err_text.assign("Chemical name");
            foundflag = getModIndex(
                OpenWQ_wqconfig,
                OpenWQ_output,
                chem_list,
                Chemical_name,
                err_text,
                chem_ssi);

            if (foundflag == false) continue; // skip if comp not found

            // Get compartment index
            err_text.assign("Compartment name");
            foundflag = getModIndex(
                OpenWQ_wqconfig,
                OpenWQ_output,
                cmp_list,
                Compartment_name,
                err_text,
                cmpi_ssi);

            if (foundflag == false) continue; // skip if comp not found

            // Set flag for sink or source
            if (Type.compare("SOURCE") == 0){
                sinksource_ssi = 0;
            }else if (Type.compare("SINK") == 0){
                sinksource_ssi = 1;
            }else{
                continue; // skip if Type is unknown
            }
            

            /* ########
            // Get units and actual input data
            ###########*/

            Units = OpenWQ_json.SinkSource // units
                [std::to_string(ssf+1)]
                [std::to_string(ssi+1)]
                ["UNITS"];

            // Get number of rows of data in JSON (YYYY, MM, DD, HH,...)
            num_rowdata = OpenWQ_json.SinkSource
                [std::to_string(ssf+1)]
                [std::to_string(ssi+1)]
                ["DATA"].size();

            /* ########################################
             // Loop over row data in sink-source file
            ######################################## */

            for (unsigned int di=0;di<num_rowdata;di++){
                
                // Reset the size to zero (the object will have no elements)
                row_data_col.reset();
                row_data_row.reset();

                // Get Date in row di            
               YYYY_json =  OpenWQ_json.SinkSource // year
                    [std::to_string(ssf+1)]
                    [std::to_string(ssi+1)]
                    ["DATA"]
                    [std::to_string(di+1)].at(0);
   
                MM_json = OpenWQ_json.SinkSource    // month
                    [std::to_string(ssf+1)]
                    [std::to_string(ssi+1)]
                    ["DATA"]
                    [std::to_string(di+1)].at(1);
                
                DD_json =OpenWQ_json.SinkSource     // day
                    [std::to_string(ssf+1)]
                    [std::to_string(ssi+1)]
                    ["DATA"]
                    [std::to_string(di+1)].at(2);
                         
                HH_json =OpenWQ_json.SinkSource     // hour
                    [std::to_string(ssf+1)]
                    [std::to_string(ssi+1)]
                    ["DATA"]
                    [std::to_string(di+1)].at(3);

                MIN_json =OpenWQ_json.SinkSource     // hour
                    [std::to_string(ssf+1)]
                    [std::to_string(ssi+1)]
                    ["DATA"]
                    [std::to_string(di+1)].at(4);            
                
                // chemname_ssi -> already obtained above // chemcial namea
                ix_json = OpenWQ_json.SinkSource // ix 
                    [std::to_string(ssf+1)]
                    [std::to_string(ssi+1)]
                    ["DATA"]
                    [std::to_string(di+1)].at(5);

                iy_json = OpenWQ_json.SinkSource // iy
                    [std::to_string(ssf+1)]
                    [std::to_string(ssi+1)]
                    ["DATA"]
                    [std::to_string(di+1)].at(6);

                iz_json = OpenWQ_json.SinkSource // iz
                    [std::to_string(ssf+1)]
                    [std::to_string(ssi+1)]
                    ["DATA"]
                    [std::to_string(di+1)].at(7);

                ss_data_json = OpenWQ_json.SinkSource // sink/source data
                    [std::to_string(ssf+1)]
                    [std::to_string(ssi+1)]
                    ["DATA"]
                    [std::to_string(di+1)].at(8);

                ss_units_json = OpenWQ_json.SinkSource // sink/source data
                    [std::to_string(ssf+1)]
                    [std::to_string(ssi+1)]
                    ["UNITS"]; 

                // Need to "- 1" for ix_json, iy_json, and iz_json because c++ starts at zero
                ix_json = std::max(ix_json - 1,0);
                iy_json = std::max(iy_json - 1,0);
                iz_json = std::max(iz_json - 1,0);
                
                // Convert SS units
                // Source/sink units (g -> default model mass units)
                // 1) Calculate unit multiplers
                std::vector<std::string> units;          // units (numerator and denominator)
                OpenWQ_units.Calc_Unit_Multipliers(
                    OpenWQ_wqconfig,
                    OpenWQ_output,
                    unit_multiplers,    // multiplers (numerator and denominator)
                    ss_units_json,      // input units
                    units,
                    true);              // direction of the conversion: 
                                        // to native (true) or 
                                        // from native to desired output units (false)

                // 2) Calculate value with new units
                OpenWQ_units.Convert_Units(
                    ss_data_json,       // ic_value passed by reference so that it can be changed
                    unit_multiplers);   // units

                // Get the vector with the data
                row_data_col = {
                    (double)chem_ssi,
                    (double)cmpi_ssi,
                    (double)sinksource_ssi,
                    (double)YYYY_json,
                    (double)MM_json,
                    (double)DD_json,
                    (double)HH_json,
                    (double)MIN_json,
                    (double)ix_json,
                    (double)iy_json,
                    (double)iz_json,
                    ss_data_json,
                    0               // field to specify if the load has aleady been used
                    };              // it starts with 0 (zero), meaning that has not been used

                // Transpose vector for adding to SinkSource_FORC as a new row
                row_data_row = row_data_col.t();

                // Add new row_data_row to SinkSource_FORC   
                int_n_elem = (*OpenWQ_wqconfig.SinkSource_FORC).n_rows;  
                                   
                (*OpenWQ_wqconfig.SinkSource_FORC).insert_rows(
                    std::max(int_n_elem-1,0),
                    row_data_row); 
            }
        }
    }
}

/* #################################################
 // Check Sources and Sinks and Apply
 ################################################# */
void OpenWQ_sinksource::CheckApply(
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_output& OpenWQ_output,
    const unsigned int YYYY,                       // current model step: Year
    const unsigned int MM,                         // current model step: month
    const unsigned int DD,                         // current model step: day
    const unsigned int HH,                         // current model step: hour
    const unsigned int MIN){                        // current model step: min
    
    // Local variables
    unsigned int num_rowdata;       // number of rows of data in JSON (YYYY, MM, DD, HH,...)
    unsigned int used_flag;         // flag to say if load has been used (=1) or not (=0) 
    unsigned int YYYY_json;         // Year in JSON-sink_source (interactive)
    unsigned int MM_json;           // Month in JSON-sink_source (interactive)
    unsigned int DD_json;           // Day in JSON-sink_source (interactive)
    unsigned int HH_json;           // Hour in JSON-sink_source (interactive)
    unsigned int MIN_json;           // Hour in JSON-sink_source (interactive)

    long sinksource_flag;           // source (=0) or sink (=1)
    
    // Get number of rows in SinkSource_FORC
    num_rowdata = (*OpenWQ_wqconfig.SinkSource_FORC).n_rows; 

    /* ########################################
        // Loop over row data in sink-source file
    ######################################## */

    for (unsigned int ri=0;ri<num_rowdata;ri++){

        // First check if row has already been used (used_flag = 1)
        // Skip it if yes
        used_flag = (*OpenWQ_wqconfig.SinkSource_FORC)(ri,12);
        if (used_flag == 1) continue;

        // ########################################
        // Check if time in SinkSource_FORC row ri matches the current model time

        // Year
        YYYY_json = (*OpenWQ_wqconfig.SinkSource_FORC)(ri,3);
        if (YYYY < YYYY_json) continue;
        
        // Month
        MM_json = (*OpenWQ_wqconfig.SinkSource_FORC)(ri,4);  
        if (MM < MM_json) continue;

        // Day
        DD_json = (*OpenWQ_wqconfig.SinkSource_FORC)(ri,5);  
        if (DD < DD_json) continue;

        // Hour
        HH_json = (*OpenWQ_wqconfig.SinkSource_FORC)(ri,6);  
        if (HH < HH_json) continue;

        // Minute
        MIN_json = (*OpenWQ_wqconfig.SinkSource_FORC)(ri,7);  
        if (MIN < MIN_json) continue;


        // ########################################
        // Apply source or sink
        // ########################################

        sinksource_flag = (*OpenWQ_wqconfig.SinkSource_FORC)(ri,2);

        // if SOURCE
        if (sinksource_flag == 0){

            OpenWQ_sinksource::Apply_Source(
                OpenWQ_vars,
                OpenWQ_wqconfig,
                OpenWQ_hostModelconfig,
                OpenWQ_output,
                (*OpenWQ_wqconfig.SinkSource_FORC)(ri,1),       // compartment model index
                (*OpenWQ_wqconfig.SinkSource_FORC)(ri,0),       // chemical model index    
                (*OpenWQ_wqconfig.SinkSource_FORC)(ri,8),       // compartment model ix
                (*OpenWQ_wqconfig.SinkSource_FORC)(ri,9),       // compartment model iy
                (*OpenWQ_wqconfig.SinkSource_FORC)(ri,10),       // compartment model iz
                (*OpenWQ_wqconfig.SinkSource_FORC)(ri,11));     // source load

        }
        // if SINK
        else if (sinksource_flag == 1){
            OpenWQ_sinksource::Apply_Sink(
                OpenWQ_vars,
                OpenWQ_wqconfig,
                OpenWQ_hostModelconfig,
                OpenWQ_output,
                (*OpenWQ_wqconfig.SinkSource_FORC)(ri,1),       // compartment model index
                (*OpenWQ_wqconfig.SinkSource_FORC)(ri,0),       // chemical model index    
                (*OpenWQ_wqconfig.SinkSource_FORC)(ri,8),       // compartment model ix
                (*OpenWQ_wqconfig.SinkSource_FORC)(ri,9),       // compartment model iy
                (*OpenWQ_wqconfig.SinkSource_FORC)(ri,10),      // compartment model iz
                (*OpenWQ_wqconfig.SinkSource_FORC)(ri,11));     // source load
        } 

        // Set row load as used (used_flag = 1)
        // So that it is not used multiple times
        used_flag = 1;
        (*OpenWQ_wqconfig.SinkSource_FORC)(ri,12) = used_flag;

    }
}


/* #################################################
 // Apply Source
 ################################################# */
void OpenWQ_sinksource::Apply_Source(
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_output& OpenWQ_output,
    const unsigned int cmpi,             // compartment model index
    const unsigned int chemi,            // chemical model index    
    const unsigned int ix,          // compartment model ix
    const unsigned int iy,          // compartment model iy
    const unsigned int iz,          // compartment model iz
    const double ss_data_json){          // source load g

    // Local Variables
    std::string msg_string;             // error/warning message string

    try{
        // Add mass load (already converted to g units)
        (*OpenWQ_vars.d_chemass_ss)(cmpi)(chemi)(ix,iy,iz) += ss_data_json;

    }catch (...){
        
        // Through a warning if request out of boundaries
        //Create message
        msg_string = 
            "<OpenWQ> WARNING: Sink/Source load out of boundaries."
            "Requested load ignored: Compartment=" 
            + std::get<1>(OpenWQ_hostModelconfig.HydroComp.at(cmpi))
            + ", Chemical=" + OpenWQ_wqconfig.BGC_general_chem_species_list[chemi]
            + ", ix=" + std::to_string(ix)
            + ", iy=" + std::to_string(iy)
            + ", iz=" + std::to_string(iz)
            + "";

        // Print it (Console and/or Log file)
        OpenWQ_output.ConsoleLog(
            OpenWQ_wqconfig,    // for Log file name
            msg_string,         // message
            true,               // print in console
            true);              // print in log file
                        
    }

}

/* #################################################
 // Apply Sink
 ################################################# */
void OpenWQ_sinksource::Apply_Sink(
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_output& OpenWQ_output,
    const unsigned int cmpi,             // compartment model index
    const unsigned int chemi,            // chemical model index    
    const unsigned int ix,          // compartment model ix
    const unsigned int iy,          // compartment model iy
    const unsigned int iz,          // compartment model iz
    const double ss_data_json){          // source load g

    // Local Variables
    double mass_sink;
    std::string msg_string;             // error/warning message string

    try{
        // Limit the mass removed to the available mass
        mass_sink = std::fmin(
            ss_data_json,
            (*OpenWQ_vars.chemass)(cmpi)(chemi)(ix,iy,iz));

        // Add mass load (already converted to g units)
        (*OpenWQ_vars.d_chemass_ss)(cmpi)(chemi)(ix,iy,iz) -= mass_sink;

    }catch (...){

        // Through a warning if request out of boundaries
        // Create Message
        msg_string = 
            "<OpenWQ> WARNING: Sink/Source load out of boundaries."
            "Requested load ignored: Compartment=" 
            + std::get<1>(OpenWQ_hostModelconfig.HydroComp.at(cmpi))
            + ", Chemical=" + OpenWQ_wqconfig.BGC_general_chem_species_list[chemi]
            + ", ix=" + std::to_string(ix)
            + ", iy=" + std::to_string(iy)
            + ", iz=" + std::to_string(iz);

        // Print it (Console and/or Log file)
        OpenWQ_output.ConsoleLog(
            OpenWQ_wqconfig,    // for Log file name
            msg_string,         // message
            true,               // print in console
            true);              // print in log file

    }

}

/* #################################################
 // Get model structure indexes for compartments and chemicals
 ################################################# */
bool OpenWQ_sinksource::getModIndex(
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_output& OpenWQ_output,
    std::vector<std::string> &vec_list,
    std::string &obj_name,
    std::string &obj_text,
    unsigned long &vec_obj_index){
    
    // Local Variables
    bool foundflag = false;
    std::vector<std::string>::iterator find_i;  // iteractor used to store the position or searched element
    std::string msg_string;             // error/warning message string

    // Try to find index
    find_i = 
        std::find(vec_list.begin(), 
        vec_list.end(), 
        obj_name);

    // If requested index exists, then okay 
    // (otherwise, throw warning and skip entry)

    if (find_i != vec_list.end()){
        vec_obj_index =   find_i - vec_list.begin();
        foundflag = true;
    }else{
        
        // Create Message (WARNING: entry skipped)
        msg_string = 
            "<OpenWQ> WARNNING (entry skipped): " 
            + obj_text 
            + " in source-sink file unkown: " 
            + obj_name;

        // Print it (Console and/or Log file)
        OpenWQ_output.ConsoleLog(
            OpenWQ_wqconfig,    // for Log file name
            msg_string,         // message
            true,               // print in console
            true);              // print in log file

    }
    return foundflag;

}
