
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

#include "OpenWQ_extwatflux_ss.h"

/* #################################################
 // Prepare SS and EWF input data for use at running time: main driver
 ################################################# */
void OpenWQ_extwatflux_ss::Set_EWFandSS_drive(
    json &EWF_SS_json,                                  // SS or EWF json    
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_units& OpenWQ_units,
    OpenWQ_utils& OpenWQ_utils,
    OpenWQ_output& OpenWQ_output,
    std::string inputType){                             // flag for SS or EWF
    
    // Local variables
    bool foundflag = false;                     // iteractive boolean to identify if comp or chem was found
    unsigned int num_srcfiles;                  // number of sink-source files 
                                                // (saved as sub-structure of SinkSource)
    unsigned int num_srchem;                    // number of chemical loads per file
    std::string DataFormat;                     // from JSON file (JSON or ASCII)
    std::string main_keyName;                   // interactive json-key name
    std::string msg_string;                     // error/warning message string


    // Get number of sub-structures of SS/EWF data
    num_srcfiles = EWF_SS_json.size(); 

    /* ########################################
    // Loop over file (saved as sub-structure of SS/EWF data)
    ######################################## */

    for (unsigned int ssf=0;ssf<num_srcfiles;ssf++){
        
        // Get number of loads in each sub-structure 
        // (corresponding to different SinkSource/ExtWatFlux json files)
        num_srchem = EWF_SS_json[std::to_string(ssf+1)].size();
        
        /* ########################################
        // Loop over loads per sub-structure
        ######################################## */

        foundflag = false;      // reset to false at every 

        for (unsigned int ssi=0;ssi<num_srchem;ssi++){

            /* ########
            // Get data format
            // try-catch block because we may have other sub-structures
            // Example: Metadata 
            ###########*/
            try{
                DataFormat = EWF_SS_json // units
                    [std::to_string(ssf+1)]
                    [std::to_string(ssi+1)]
                    ["DATA_FORMAT"];
            }catch(...){}

            /* ########
            // Call appropriate function depending on data format
            ###########*/
            if (DataFormat.compare("JSON")==0 || 
                DataFormat.compare("ASCII")==0) {
                
                // if JSON or ASCII format
                Set_EWFandSS_jsonAscii(
                    OpenWQ_hostModelconfig,
                    OpenWQ_wqconfig,
                    OpenWQ_utils,
                    OpenWQ_units,
                    OpenWQ_output,
                    ssf, ssi,
                    DataFormat,
                    EWF_SS_json[std::to_string(ssf+1)][std::to_string(ssi+1)],  // relevant sub-json
                    inputType, // ss or ewf
                    foundflag);

            }else if(DataFormat.compare("HDF5")==0){
                
                // if H5 format
                Set_EWFandSS_h5(
                    OpenWQ_hostModelconfig,
                    OpenWQ_wqconfig,
                    OpenWQ_utils,
                    OpenWQ_units,
                    OpenWQ_output,
                    EWF_SS_json[std::to_string(ssf+1)][std::to_string(ssi+1)],  // relevant sub-json
                    inputType, // ss or ewf
                    foundflag);

            }else{

                // Create Message (Warning Message)
                msg_string = 
                    "<OpenWQ> WARNING: Unkown data format='" 
                    + DataFormat
                    + "' in SS or EWF json files > "
                    + " > DATA_FORMAT (only supports JSON, ASCII and HD5F) "
                    + "(entry skipped)";

                // Print it (Console and/or Log file)
                OpenWQ_output.ConsoleLog(
                    OpenWQ_wqconfig,    // for Log file name
                    msg_string,         // message
                    true,               // print in console
                    true);              // print in log file

            }
        }
    }
}

/* #################################################
 // Prepare SS and EWF input data for use at running time: 
 // Case: JSON and ASCII format
 ################################################# */
void OpenWQ_extwatflux_ss::Set_EWFandSS_jsonAscii(
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_utils& OpenWQ_utils,
    OpenWQ_units& OpenWQ_units,
    OpenWQ_output& OpenWQ_output,
    unsigned int ssf, unsigned int ssi,   // file-structure and substructure indexes
    std::string DataFormat,         // (JSON or ASCII)
    json EWF_SS_json_sub,           // relevant sub-json
    std::string inputType,
    bool foundflag){                // ss or ewf

    // Local variables
    std::string main_keyName;
    std::string Element_name;
    std::string Chemical_name;                  // chemical name
    std::vector<std::string> elm_list;          // model compartment list
    std::string err_text;                       // iteractive string for text to pass to error messages
    unsigned long cmpi_ssi;                     // model index for compartment Compartment_name_name
    unsigned long chem_ssi;                     // model index for compartment Compartment_name_name
    unsigned long sinksource_ssi;               // = 0 (source), = 1 (sink)

    unsigned int num_rowdata;                   // number of rows of data in JSON (YYYY, MM, DD, HH,...)
    
    std::string Type;                           // from JSON filec (only used in SS)

    std::string ascii_FilePath;                 // additional information for ASCII data input
    std::string ascii_delimiter;                // additional information for ASCII data input
    unsigned int ascii_numHeaderRows;           // additional information for ASCII data input
    unsigned int ascii_headerKeyRow;            // additional information for ASCII data input
    unsigned int lineCount;                     // row count for ASCII file
    std::string rowEntryASCII;                  // row data
    std::string headerRowASCII;                 // row with header data
    std::vector<std::string> ASCIIRowdata;      // all row data
    std::vector<std::string> ASCIIRowElemEntry; // vector with header keys
    std::vector<std::string> headerKeys;        // vector with header keys
    std::string headerWord;                     // interactive header words

    std::string elemName;                       // temporary element name
    int YYYY_json;                              // Year in JSON-sink_source (interactive)
    int MM_json;                                // Month in JSON-sink_source (interactive)
    int DD_json;                                // Day in JSON-sink_source (interactive)
    int HH_json;                                // Hour in JSON-sink_source (interactive)
    int MIN_json;                               // Minutes in JSON-sink_source (interactive)
    int SEC_json;                               // Seconds in JSON-sink_source (interactive)
    int ix_json;                                // iteractive ix info for sink-source row data 
    int iy_json;                                // iteractive iy info for sink-source row data 
    int iz_json;                                // iteractive iz info for sink-source row data

    double ss_data_json;                        // data (sink or source) from row data
    std::string ss_units_json;                  // units of row data
    std::vector<double> unit_multiplers;        // multiplers (numerator and denominator)
   
    arma::vec row_data_col;                     // new row data (initially as col data)

    std::string msg_string;                     // error/warning message string
    bool validEntryFlag;                        // valid entry flag to skip problematic row data

    std::string loadScheme_str;                 // Load scheme string: (1) discrete or (2) continuous
    double loadScheme_id;                       // Load scheme id number: (1) discrete or (2) continuous
    std::string contDt_str;                     // time units of continuous load


    // Get element list (compartment for SS, and External fluxes for EWF)
    if (inputType.compare("ss")==0)     elm_list = OpenWQ_hostModelconfig.cmpt_names;
    if (inputType.compare("ewf")==0)    elm_list = OpenWQ_hostModelconfig.ewf_names;

    // Element names: Compartment or External Flux
    if (inputType.compare("ss")==0)       main_keyName = "RECIPIENT_COMPARTMENT_NAME"; // SS
    else if (inputType.compare("ewf")==0) main_keyName = "EXTERNAL_INPUTFLUX_NAME";    // EWF

    // Get mainKeyName
    // "RECIPIENT_COMPARTMENT_NAME" in the case of "ss"
    // "EXTERNAL_INPUTFLUX_NAME" in the case of "ewf"
    // Needs try-catch because there may be other irrelevant entries e.g. COMMENTS
    try{
        Element_name = EWF_SS_json_sub[main_keyName];
    }catch(...){   
        return;
    }

    /* ########
    // Get chemical name and compartment/external-flux name 
    // if SS, then get also SS_type (source or sink)
    ###########*/

    Chemical_name = EWF_SS_json_sub["CHEMICAL_NAME"]; // chemical name
            
    // Type (sink or source) (only used in SS)
    if (inputType.compare("ss")==0) {
        Type = EWF_SS_json_sub["TYPE"];}
    else if (inputType.compare("ewf")==0) Type = "SOURCE";

    /* ########
    // Check if the requests are valid
    // chemical name, compartment name and SS_type (source or sink)
    ###########*/

    // Get chemical index
    err_text.assign("Chemical name");
    foundflag = getModIndex(
        OpenWQ_wqconfig,
        OpenWQ_output,
        OpenWQ_wqconfig.BGC_general_chem_species_list,
        Chemical_name,
        err_text,
        chem_ssi);

    // Get Units
    ss_units_json = EWF_SS_json_sub ["UNITS"]; 

    if (foundflag == false) return; // skip if chem not found

    // Get compartment/water flux index
    if (inputType.compare("ss")==0) err_text.assign("Compartment name");
    if (inputType.compare("ewf")==0) err_text.assign("External Water Flux name");

    foundflag = getModIndex(
        OpenWQ_wqconfig,
        OpenWQ_output,
        elm_list,
        Element_name,
        err_text,
        cmpi_ssi);

    if (foundflag == false) return; // skip if comp/ext-flux not found

    // Set type flag (sink or source)
    // if EWF, then Type has been defined above as "SOURCE"
    if (Type.compare("SOURCE") == 0){ sinksource_ssi = 0;
    }else if (Type.compare("SINK") == 0){ sinksource_ssi = 1;
    }else{return;} // skip if Type is unknown
    
    /* ########
    // Check if DATA_FORMAT=ASCII
    // if yes, then get file structure info
    ###########*/

    // Get additional info for ASCII
    if (DataFormat.compare("ASCII")==0){

        try{
            // file path
            ascii_FilePath = EWF_SS_json_sub["DATA"]["FILEPATH"];
            // delimiter
            ascii_delimiter = EWF_SS_json_sub["DATA"]["DELIMITER"];
            // number of header rows
            ascii_numHeaderRows = EWF_SS_json_sub["DATA"]["NUMBER_OF_HEADER_ROWS"];
            // position of key header
            ascii_headerKeyRow = EWF_SS_json_sub["DATA"]["HEADER_KEY_ROW"];

            // Get number of rows in ASCII file
            num_rowdata = OpenWQ_utils.getNumLinesfromASCIIFile(ascii_FilePath)
                - ascii_numHeaderRows;

            // Open ASCII file (read mode only)
            std::ifstream asciiFile (ascii_FilePath, std::ios::in);

            // Read skip header lines
            if (asciiFile.is_open()){
                lineCount = 1;
                while(std::getline(asciiFile, rowEntryASCII)){
                    // get header row when found
                    if(lineCount==ascii_headerKeyRow){
                        headerRowASCII = rowEntryASCII;
                        // Convert string to uppercase
                        headerRowASCII = OpenWQ_utils.ConvertStringToUpperCase(headerRowASCII);
                        // Parse the headerRowASCII to get a vector with the json entries
                        // parsing based on the delimiter ascii_delimiter defined by the user
                        headerKeys = OpenWQ_utils.StringSplit(
                            headerRowASCII,
                            ascii_delimiter);}
                    // push new entry to if actual row data
                    if(lineCount>ascii_numHeaderRows) ASCIIRowdata.push_back(rowEntryASCII);
                    // update line count
                    ++lineCount;
                }
                asciiFile.close();
            }else{
                    // If there is an issue with the ASCII input data
                // through a warning message and skip entry
                msg_string = 
                    "<OpenWQ> WARNING: SS/EWF '" 
                    " load/sink/conc ASCII file cannot be found (entry skipped): File=" 
                    + ascii_FilePath
                    + " in JSON SS file "
                    + std::to_string(ssf+1)
                    + ", Sub_structure=" + std::to_string(ssi+1);
                // Print it (Console and/or Log file)
                OpenWQ_output.ConsoleLog(
                    OpenWQ_wqconfig,    // for Log file name
                    msg_string,         // message
                    true,               // print in console
                    true);              // print in log file
                return;
            }
    
        }catch(...){
            // If there is an issue with the ASCII input data
            // through a warning message and skip entry
            msg_string = 
                "<OpenWQ> WARNING: SS/EWF '" 
                " load/sink/conc with ASCII format has an issue with json-keys or data structure (entry skipped): File=" 
                + std::to_string(ssf+1)
                + ", Sub_structure=" + std::to_string(ssi+1);
            // Print it (Console and/or Log file)
            OpenWQ_output.ConsoleLog(
                OpenWQ_wqconfig,    // for Log file name
                msg_string,         // message
                true,               // print in console
                true);              // print in log file
            return;
        }
        
    // If JSON
    }else if (DataFormat.compare("JSON")==0){
        // Get number of rows of data in JSON (YYYY, MM, DD, HH,...)
        num_rowdata = EWF_SS_json_sub["DATA"].size();
    }

    /* ########################################
        // Loop over row data in sink-source file
    ######################################## */

    for (unsigned int di=0;di<num_rowdata;di++){
        
        // Reset the size to zero (the object will have no elements)
        row_data_col.reset(); 

        // If DataFormat=ASCII, then get row data 
        // and convert-to-upper-case and split it by element entry
        if (DataFormat.compare("ASCII")==0){
            ASCIIRowElemEntry = 
                OpenWQ_utils.StringSplit(
                    OpenWQ_utils.ConvertStringToUpperCase(ASCIIRowdata[di]),
                    ascii_delimiter);}

        // ###################
        // Year
        // ###################
        elemName = "Year";
        try{

            // try entry as int
            int entryVal = 0; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub 
                ["DATA"][std::to_string(di+1)].at(0);}
            // if ASCII
            else if (DataFormat.compare("ASCII")==0){
                entryVal = std::stoi(ASCIIRowElemEntry[ 
                OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"YYYY")]);}

            YYYY_json = entryVal;

        }catch(...){

            // try as string for the cases where entry is "all"
            std::string entryVal = ""; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub
                ["DATA"][std::to_string(di+1)].at(0);}
            // if ASCII
            else if (DataFormat.compare("ASCII")==0){
                entryVal = ASCIIRowElemEntry[ 
                    OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"YYYY")];}

            // Check if "all" and return flag validEntryFlag
            validEntryFlag = getArrayElem(
                OpenWQ_wqconfig,OpenWQ_output,
                elemName,
                (std::string) entryVal,
                YYYY_json,
                ssf, ssi, di); // SS file, structure and row
                    
            if (!validEntryFlag){continue;}

        }
        
        // ###################
        // Month
        // ###################
        elemName = "Month";
        try{

            // try entry as int
            int entryVal = 0; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub
                ["DATA"][std::to_string(di+1)].at(1);}
            // if ASCII
            else if (DataFormat.compare("ASCII")==0){
                entryVal = std::stoi(ASCIIRowElemEntry[ 
                OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"MM")]);}

            MM_json = entryVal;

        }catch(...){

            // try as string for the cases where entry is "all"
            std::string entryVal = ""; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub
                ["DATA"][std::to_string(di+1)].at(1);}
            // if ASCII
            else if (DataFormat.compare("ASCII")==0){
                entryVal = ASCIIRowElemEntry[ 
                    OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"MM")];}

            validEntryFlag = getArrayElem(
                OpenWQ_wqconfig,
                OpenWQ_output,
                elemName,
                (std::string) entryVal,
                MM_json,
                ssf,    // SS file
                ssi,    // SS structure
                di);    // SS row

            if (!validEntryFlag){continue;}
        }
        
        // ###################
        // Day
        // ###################
        elemName = "Day";
        try{

            // try entry as int
            int entryVal = 0; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub
                ["DATA"][std::to_string(di+1)].at(2);}
            // if ASCII
            else if (DataFormat.compare("ASCII")==0){
                entryVal = std::stoi(ASCIIRowElemEntry[ 
                OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"DD")]);}

            DD_json = entryVal;

        }catch(...){

            // try as string for the cases where entry is "all"
            std::string entryVal = ""; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub
                ["DATA"][std::to_string(di+1)].at(2);}
            // if ASCII
            else if (DataFormat.compare("ASCII")==0){
                entryVal = ASCIIRowElemEntry[ 
                    OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"DD")];}

            validEntryFlag = getArrayElem(
                OpenWQ_wqconfig,
                OpenWQ_output,
                elemName,
                (std::string) entryVal,
                DD_json,
                ssf,    // SS file
                ssi,    // SS structure
                di);    // SS row

            if (!validEntryFlag){continue;}
        }

        // ###################
        // Hour
        // ###################
        elemName = "Hour";
        try{

            // try entry as int
            int entryVal = 0; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub
                ["DATA"][std::to_string(di+1)].at(3);}
            // if ASCII
            else if (DataFormat.compare("ASCII")==0){
                entryVal = std::stoi(ASCIIRowElemEntry[ 
                OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"HH")]);}

            HH_json = entryVal;

        }catch(...){

            // try as string for the cases where entry is "all"
            std::string entryVal = ""; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub
                ["DATA"][std::to_string(di+1)].at(3);}
            // if ASCII
            else if (DataFormat.compare("ASCII")==0){
                entryVal = ASCIIRowElemEntry[ 
                    OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"HH")];}

            validEntryFlag = getArrayElem(
                OpenWQ_wqconfig,
                OpenWQ_output,
                elemName,
                (std::string) entryVal,
                HH_json,
                ssf,    // SS file
                ssi,    // SS structure
                di);    // SS row

            if (!validEntryFlag){continue;}
        }        
        
        // ###################
        // Minute
        // ###################
        elemName = "Minute";
        try{

            // try entry as int
            int entryVal = 0; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub
                ["DATA"][std::to_string(di+1)].at(4);}
            // if ASCII
            else if (DataFormat.compare("ASCII")==0){
                entryVal = std::stoi(ASCIIRowElemEntry[ 
                OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"MIN")]);}

            MIN_json = entryVal;

        }catch(...){

            // try as string for the cases where entry is "all"
            std::string entryVal = ""; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub
                ["DATA"][std::to_string(di+1)].at(4);}
            // if ASCII
            else if (DataFormat.compare("ASCII")==0){
                entryVal = ASCIIRowElemEntry[ 
                    OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"MIN")];}

            validEntryFlag = getArrayElem(
                OpenWQ_wqconfig,
                OpenWQ_output,
                elemName,
                (std::string) entryVal,
                MIN_json,
                ssf,    // SS file
                ssi,    // SS structure
                di);    // SS row

            if (!validEntryFlag){continue;}
        }

        // ###################
        // Year
        // ###################
        elemName = "Second";
        try{

            // try entry as int
            int entryVal = 0; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub
                ["DATA"][std::to_string(di+1)].at(5);}
            // if ASCII
            else if (DataFormat.compare("ASCII")==0){
                entryVal = std::stoi(ASCIIRowElemEntry[ 
                OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"SEC")]);}

            SEC_json = entryVal;

        }catch(...){

            // try as string for the cases where entry is "all"
            std::string entryVal = ""; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub
                ["DATA"][std::to_string(di+1)].at(5);}
            // if ASCII
            else if (DataFormat.compare("ASCII")==0){
                entryVal = ASCIIRowElemEntry[ 
                    OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"SEC")];}

            validEntryFlag = getArrayElem(
                OpenWQ_wqconfig,
                OpenWQ_output,
                elemName,
                (std::string) entryVal,
                SEC_json,
                ssf,    // SS file
                ssi,    // SS structure
                di);    // SS row

            if (!validEntryFlag){continue;}
        }

        // chemname_ssi -> already obtained above // chemical name

        // ###################
        // ix
        // ###################
        elemName = "ix";
        try{

            // try entry as int
            int entryVal = 0; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub
                ["DATA"][std::to_string(di+1)].at(6);}
            // if ASCII
            else if (DataFormat.compare("ASCII")==0){
                entryVal = std::stoi(ASCIIRowElemEntry[ 
                OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"IX")]);}

            ix_json = entryVal;

            // Need to do "- 1" because C++ starts in zero
            ix_json--;

            // If entry in json is zero, we get here -1
            // which is wrong, so need to send warning messsage
            // and skip entry
            if (ix_json == -1){
                // Through a warning invalid entry           
                msg_string = 
                    "<OpenWQ> WARNING: SS '" 
                    + elemName 
                    + "' cannot be zero. It needs to start in one (entry skipped): File=" 
                    + std::to_string(ssf+1)
                    + ", Sub_structure=" + std::to_string(ssi+1)
                    + ", Data_row=" + std::to_string(di + 1);   

                OpenWQ_output.ConsoleLog(   // Print it (Console and/or Log file)
                    OpenWQ_wqconfig,        // for Log file name
                    msg_string,             // message
                    true,                   // print in console
                    true);                  // print in log file

                continue; // skip entry
            }

        }catch(...){

            // try as string for the cases where entry is "all"
            std::string entryVal = ""; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub
                ["DATA"][std::to_string(di+1)].at(6);}
            // if ASCII
            else if (DataFormat.compare("ASCII")==0){
                entryVal = ASCIIRowElemEntry[ 
                    OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"IX")];}

            validEntryFlag = getArrayElem(
                OpenWQ_wqconfig,
                OpenWQ_output,
                elemName,
                (std::string) entryVal,
                ix_json,
                ssf,    // SS file
                ssi,    // SS structure
                di);    // SS row

            if (!validEntryFlag){continue;}
        }

        // ###################
        // iy
        // ###################
        elemName = "iy";
        try{

            // try entry as int
            int entryVal = 0; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub
                ["DATA"][std::to_string(di+1)].at(7);}
            // if ASCII
            else if (DataFormat.compare("ASCII")==0){
                entryVal = std::stoi(ASCIIRowElemEntry[ 
                OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"IY")]);}

            iy_json = entryVal;

            // Need to do "- 1" because C++ starts in zero
            iy_json--;

            // If entry in json is zero, we get here -1
            // which is wrong, so need to send warning messsage
            // and skip entry
            if (iy_json == -1){
                // Through a warning invalid entry           
                msg_string = 
                    "<OpenWQ> WARNING: SS '" 
                    + elemName 
                    + "' cannot be zero. It needs to start in one (entry skipped): File=" 
                    + std::to_string(ssf+1)
                    + ", Sub_structure=" + std::to_string(ssi+1)
                    + ", Data_row=" + std::to_string(di + 1);   

                OpenWQ_output.ConsoleLog(   // Print it (Console and/or Log file)
                    OpenWQ_wqconfig,        // for Log file name
                    msg_string,             // message
                    true,                   // print in console
                    true);                  // print in log file

                continue; // skip entry
            }

        }catch(...){

            // try as string for the cases where entry is "all"
            std::string entryVal = ""; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub
                ["DATA"][std::to_string(di+1)].at(7);}
            // if ASCII
            else if (DataFormat.compare("ASCII")==0){
                entryVal = ASCIIRowElemEntry[ 
                    OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"IY")];}

            validEntryFlag = getArrayElem(
                OpenWQ_wqconfig,
                OpenWQ_output,
                elemName,
                (std::string) entryVal,
                iy_json,
                ssf,    // SS file
                ssi,    // SS structure
                di);    // SS row

            if (!validEntryFlag){continue;}
        }

        // ###################
        // iz
        // ###################
        elemName = "iz";
        try{

            // try entry as int
            int entryVal = 0; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub
                ["DATA"][std::to_string(di+1)].at(8);}
            // if ASCII
            else if (DataFormat.compare("ASCII")==0){
                entryVal = std::stoi(ASCIIRowElemEntry[ 
                OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"IZ")]);}

            iz_json = entryVal;

            // Need to do "- 1" because C++ starts in zero
            iz_json--;

            // If entry in json is zero, we get here -1
            // which is wrong, so need to send warning messsage
            // and skip entry
            if (iz_json == -1){
                // Through a warning invalid entry           
                msg_string = 
                    "<OpenWQ> WARNING: SS '" 
                    + elemName 
                    + "' cannot be zero. It needs to start in one (entry skipped): File=" 
                    + std::to_string(ssf+1)
                    + ", Sub_structure=" + std::to_string(ssi+1)
                    + ", Data_row=" + std::to_string(di + 1); 

                OpenWQ_output.ConsoleLog(   // Print it (Console and/or Log file)
                    OpenWQ_wqconfig,        // for Log file name
                    msg_string,             // message
                    true,                   // print in console
                    true);                  // print in log file

                continue; // skip entry
            }

        }catch(...){

            // try as string for the cases where entry is "all"
            std::string entryVal = ""; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub
                ["DATA"][std::to_string(di+1)].at(8);}
            // if ASCII
            else if (DataFormat.compare("ASCII")==0){
                entryVal = ASCIIRowElemEntry[ 
                    OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"IZ")];}

            validEntryFlag = getArrayElem(
                OpenWQ_wqconfig,
                OpenWQ_output,
                elemName,
                (std::string) entryVal,
                iz_json,
                ssf,    // SS file
                ssi,    // SS structure
                di);    // SS row

            if (!validEntryFlag){continue;}
        }

        // ###################
        // SS sink/source load or EWF conc
        // cannot have negative values
        // ###################

        double entryVal = 0.0f; // dummy variable
        // if JSON
        if (DataFormat.compare("JSON")==0){
            entryVal = EWF_SS_json_sub
            ["DATA"][std::to_string(di+1)].at(9);}
        // if ASCII
        else if (DataFormat.compare("ASCII")==0){
            entryVal = std::stod(ASCIIRowElemEntry[ 
            OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"LOAD")]);}

        ss_data_json = entryVal;

        // skip if negative value in SS load/sink or EWF conc 
        // throw warning msg
        if(ss_data_json < 0.0f){
            // Create Warning Message
            msg_string = 
                    "<OpenWQ> WARNING: SS/EWF '" 
                    " load/sink/conc cannot be negative (entry skipped): File=" 
                    + std::to_string(ssf+1)
                    + ", Sub_structure=" + std::to_string(ssi+1)
                    + ", Data_row=" + std::to_string(di + 1);  
            // Print it (Console and/or Log file)
            OpenWQ_output.ConsoleLog(
                OpenWQ_wqconfig,    // for Log file name
                msg_string,         // message
                true,               // print in console
                true);              // print in log file
            validEntryFlag = false;
            if (!validEntryFlag){continue;}
        }

        // ###################
        // Load scheme (discrete, continuous)
        // ###################

        elemName = "Load/Sink Scheme";

        try{

            // try as string for the cases where entry is "all"
            std::string entryVal = ""; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub
                ["DATA"][std::to_string(di+1)].at(10);}
            // if ASCII
            else if (DataFormat.compare("ASCII")==0){
                entryVal = ASCIIRowElemEntry[ 
                    OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"LOAD_TYPE")];}

            loadScheme_str = entryVal;

            // loading scheme only needed if SS
            // EWF is in concentration and associated with fluxes
            if (inputType.compare("ss")==0){

                // Set loadScheme_id
                // 1) discrete
                // 2) continuous (needs time units)
                if (loadScheme_str.compare("DISCRETE") == 0) loadScheme_id = 0;
                else if (loadScheme_str.compare("CONTINUOUS") == 0 && MM_json != -1){ 
                    // continuous option needs MIN = 'all' to allow a minimum continuous period
                    loadScheme_id = 0;
                    // Create Warning Message
                    msg_string = 
                            "<OpenWQ> WARNING: SS/EWF '" 
                            + elemName 
                            + "' was defaulted to 'discrete'. The 'continuous' option is only valid with MIN set as 'all' for a minimum continuous period: File=" 
                            + std::to_string(ssf+1)
                            + ", Sub_structure=" + std::to_string(ssi+1)
                            + ", Data_row=" + std::to_string(di + 1);  
                    // Print it (Console and/or Log file)
                    OpenWQ_output.ConsoleLog(
                        OpenWQ_wqconfig,    // for Log file name
                        msg_string,         // message
                        true,               // print in console
                        true);              // print in log file

                }else if (loadScheme_str.compare("CONTINUOUS") == 0 && MM_json == -1){
                    // continuous option needs MIN = 'all' (otherwise it's discrete input)
                    loadScheme_id = 1;
                    // get time units
                    try{

                        std::string entryVal = ""; // dummy variable
                        // if JSON
                        if (DataFormat.compare("JSON")==0){
                            entryVal = EWF_SS_json_sub
                            ["DATA"][std::to_string(di+1)].at(11);}
                        // if ASCII
                        else if (DataFormat.compare("ASCII")==0){
                            entryVal = ASCIIRowElemEntry[ 
                                OpenWQ_utils.FindStrIndexInVectStr(headerKeys,"TIME_UNITS")];}

                        contDt_str = entryVal;

                        // Concatenate the time units to the load
                        ss_units_json += "/";
                        ss_units_json += contDt_str;

                    }catch(...){ 
                        // Create Warning Message
                        msg_string = 
                            "<OpenWQ> WARNING: SS/EWF '" 
                            + elemName 
                            + "'continuous' requires an additional array element with the load/sink time units (entry skipped): File=" 
                            + std::to_string(ssf+1)
                            + ", Sub_structure=" + std::to_string(ssi+1)
                            + ", Data_row=" + std::to_string(di + 1); 
                        // Print it (Console and/or Log file)
                        OpenWQ_output.ConsoleLog(
                            OpenWQ_wqconfig,    // for Log file name
                            msg_string,         // message
                            true,               // print in console
                            true);              // print in log file
                        continue; // skip entry
                    }
                }else{

                    // Print it (Console and/or Log file)
                    OpenWQ_output.ConsoleLog(
                        OpenWQ_wqconfig,    // for Log file name
                        msg_string,         // message
                        true,               // print in console
                        true);              // print in log file

                    continue;
                }

            }else{
                // if EWF, send a warning message saying that the 
                // load scheme and period are not used in EWF inputs

                // Create Warning Message
                msg_string = 
                    "<OpenWQ> WARNING: EWF '" 
                    + elemName 
                    + "' is not used in EWF entries because these are" 
                    " concentrations associated with external fluxes (entry ignored): File=" 
                    + std::to_string(ssf+1)
                    + ", Sub_structure=" + std::to_string(ssi+1)
                    + ", Data_row=" + std::to_string(di + 1); 

                // Print it (Console and/or Log file)
                OpenWQ_output.ConsoleLog(
                    OpenWQ_wqconfig,    // for Log file name
                    msg_string,         // message
                    true,               // print in console
                    true);              // print in log file

            }

        }catch(...){  
            
            // load scheme is needed for SS, but not for EWF
            if (inputType.compare("ss")==0){

                // Create Warning Message
                // only printed if entry is not valid
                msg_string = 
                    "<OpenWQ> WARNING: SS '" 
                    + elemName 
                    + "' is not valid. It can only be 'discrete' or 'continuous' (entry skipped): File=" 
                    + std::to_string(ssf+1)
                    + ", Sub_structure=" + std::to_string(ssi+1)
                    + ", Data_row=" + std::to_string(di + 1); 

                // Print it (Console and/or Log file)
                OpenWQ_output.ConsoleLog(
                    OpenWQ_wqconfig,    // for Log file name
                    msg_string,         // message
                    true,               // print in console
                    true);              // print in log file

                continue;
            }
        }

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
            ss_data_json,       // value passed by reference so that it can be changed
            unit_multiplers);   // units

        // Get the vector with the data
        row_data_col = {
            (double) chem_ssi,
            (double) cmpi_ssi,
            (double) sinksource_ssi,
            (double) YYYY_json,
            (double) MM_json,
            (double) DD_json,
            (double) HH_json,
            (double) MIN_json,
            (double) SEC_json,
            (double) ix_json,
            (double) iy_json,
            (double) iz_json,
            ss_data_json,
            loadScheme_id,  // load scheme (0) not applicable, (1) discrete or (2) continuous
            0,0,0,0,0,0     // field to specify the number of times it has been used aleady
            };              // in the case of and "all" element (YYYY, MM, DD, HH, MIN, SEC)
                            // it starts with 0 (zero), meaning that has not been used
                            // if not an "all" element, then it's set to -1

        // Add new row to SinkSource_FORC or ExtFlux_FORC
        AppendRow_SS_EWF_FORC(
            OpenWQ_wqconfig,
            inputType,
            row_data_col);

    }

 }

/* #################################################
 // Prepare SS and EWF input data for use at running time: 
 // Case: JSON and ASCII format
 ################################################# */
void OpenWQ_extwatflux_ss::Set_EWFandSS_h5(
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_utils& OpenWQ_utils,
    OpenWQ_units& OpenWQ_units,
    OpenWQ_output& OpenWQ_output,
    json EWF_SS_json_sub,  // relevant sub-json
    std::string inputType,
    bool foundflag){
    
    // Local variables
    std::string msg_string;
    std::size_t it;
    std::string ewf_h5_folderPath;
    std::string ewf_filenamePath;
    std::string ewf_h5_units;
    std::string ewf_h5_units_file;
    bool volume_unit_flag;
    std::vector<std::string> units;
    std::string ewf_external_compartName;
    std::string chemname;
    arma::mat xyzEWF_h5;
    arma::mat dataEWF_h5;
    std::vector<double> unit_multiplers;  // multiplers (numerator and denominator)
    std::vector<std::string> tSamp_valid; // save valid time stamps
    time_t tSamp_valid_i_time_t; 
    arma::vec row_data_col;                     // new row data (initially as col data)
    int x_externModel, y_externModel, z_externModel;
    double cmp_recipient;
    std::string ss_cmp_recipient_name;
   
    // h5 IC folder path
    ewf_h5_folderPath = EWF_SS_json_sub["FOLDERPATH"];
    
    // h5 ic units
    ewf_h5_units = EWF_SS_json_sub["UNITS"];
    ewf_h5_units_file = ewf_h5_units;

    // replace "/" by "|" is needed because "/" is not compatible with directory full paths
    it = (int) ewf_h5_units_file.find("/");
    if (it <= ewf_h5_units_file.size()){
        ewf_h5_units_file.replace(it,1, "|");
    }

    // Get unit conversion multipliers
    volume_unit_flag = OpenWQ_units.Calc_Unit_Multipliers(
        OpenWQ_wqconfig,
        OpenWQ_output,
        unit_multiplers,    // multiplers (numerator and denominator)
        ewf_h5_units,       // input units
        units,
        true);              // direction of the conversion: 
                            // to native (true) or 
                            // from native to desired output units (false)

    // Get specific field for SS and EWF
    // 1) if SS, get RECIPIENT_COMPARTMENT_NAME
    try{
        // Try to find the key
        ss_cmp_recipient_name = EWF_SS_json_sub["RECIPIENT_COMPARTMENT_NAME"];

        // get index
        cmp_recipient = OpenWQ_utils.FindStrIndexInVectStr(
            OpenWQ_hostModelconfig.cmpt_names,
            ss_cmp_recipient_name);

        // if comparment not found
        if(inputType.compare("ss")==0 && cmp_recipient==-1.0f){

            // If key not found, throw warning and skip
            msg_string = 
                "<OpenWQ> WARNNING SS json key RECIPIENT_COMPARTMENT_NAME= "
                + ss_cmp_recipient_name
                + " not valid for this host-model coupling (entry ignored).";

            // Print it (Console and/or Log file)
            OpenWQ_output.ConsoleLog(
                OpenWQ_wqconfig,    // for Log file name
                msg_string,         // message
                true,               // print in console
                true);              // print in log file

            return;
        }

        // not needed for ewf
        if(inputType.compare("ewf")==0){

            // If key not found, throw warning and skip
            msg_string = 
                "<OpenWQ> WARNNING json key 'RECIPIENT_COMPARTMENT_NAME' is not used "
                "for SS requests. (key option ignored)";

            // Print it (Console and/or Log file)
            OpenWQ_output.ConsoleLog(
                OpenWQ_wqconfig,    // for Log file name
                msg_string,         // message
                true,               // print in console
                true);              // print in log file
        }

    }catch(...){

        if(inputType.compare("ss")==0){

            // If key not found, throw warning and skip
            msg_string = 
                "<OpenWQ> WARNNING json key 'RECIPIENT_COMPARTMENT_NAME' is needed "
                "for all SS requests when using HDF5 input. "
                "Check all your SS json files and try again.";

            // Print it (Console and/or Log file)
            OpenWQ_output.ConsoleLog(
                OpenWQ_wqconfig,    // for Log file name
                msg_string,         // message
                true,               // print in console
                true);              // print in log file

            return;

        }

    }

    // 2) if EWF, get EXTERNAL_COMPARTMENT_NAME
    try{

        // Try to find the key
        ewf_external_compartName = EWF_SS_json_sub["EXTERNAL_COMPARTMENT_NAME"];
    
    }catch(...){

        // If key not found, throw warning and skip
        msg_string = 
            "<OpenWQ> WARNNING json key 'EXTERNAL_COMPARTMENT_NAME' is needed "
            "for all SS and EWF requests when using HDF5 input. This is needed to identify the file name"
            "Check all your EWF json files and try again (entry skipped).";

        // Print it (Console and/or Log file)
        OpenWQ_output.ConsoleLog(
            OpenWQ_wqconfig,    // for Log file name
            msg_string,         // message
            true,               // print in console
            true);              // print in log file

        return;

    }

    // Get valid time steps from the logFile of EWF simulation
    OpenWQ_utils.GetTimeStampsFromLogFile(
        OpenWQ_wqconfig,
        OpenWQ_output,
        ewf_h5_folderPath,
        "<OpenWQ> Output export successful (HDF5): ", // Substring of the output to search
        tSamp_valid,
        "SS/EWF load/sink/conc H5 supporting logFile"); // Logfile errMsg identifier

    // Loop over chemical species 
    for (unsigned int chemi=0;chemi<OpenWQ_wqconfig.BGC_general_num_chem;chemi++){
        
        chemname = (OpenWQ_wqconfig.BGC_general_chem_species_list)[chemi];

        // Generate full ic filename
        ewf_filenamePath = ewf_h5_folderPath;

        ewf_filenamePath.append("/");
        ewf_filenamePath.append(ewf_external_compartName); // compartment
        ewf_filenamePath.append("@");
        ewf_filenamePath.append(chemname);     // chemical name
        ewf_filenamePath.append("#");
        ewf_filenamePath.append(ewf_h5_units_file); // units
        ewf_filenamePath.append("-main.h5"); 

        // Get x,y,z elements in h5 ic data
        xyzEWF_h5
            .load(arma::hdf5_name(
                ewf_filenamePath,          // file name
                "xyz_elements"));          // options

        // Get valid timestamps
        for (long unsigned int tSamp=0;tSamp<tSamp_valid.size();tSamp++){

            // Get the corresponding data
            dataEWF_h5
                .load(arma::hdf5_name(
                    ewf_filenamePath,            // file name
                    tSamp_valid[tSamp]));          // options

            // Get timestamp sting into time_t
            tSamp_valid_i_time_t = OpenWQ_units.convertTime_str2time_t(
                tSamp_valid[tSamp]);

            for (int rowi=0;rowi<(int)xyzEWF_h5.n_cols;rowi++){

                // Get element x,y,z indexes of external model
                // Using convention of external model
                // Needs to be converted into local openwq implementation
                x_externModel = xyzEWF_h5(rowi, 0);
                y_externModel = xyzEWF_h5(rowi, 1);
                z_externModel = xyzEWF_h5(rowi, 2);

                // Extract H5 row to row_data_col
                // Unit conversion performed inside this function
                row_data_col = ConvertH5row2ArmaVec(
                    OpenWQ_units,
                    unit_multiplers,
                    tSamp_valid_i_time_t,
                    x_externModel, 
                    y_externModel, 
                    z_externModel,
                    cmp_recipient,
                    dataEWF_h5, 
                    rowi,
                    chemi);

                // Add new row to SinkSource_FORC or ExtFlux_FORC
                AppendRow_SS_EWF_FORC(
                    OpenWQ_wqconfig,
                    inputType,
                    row_data_col);

            }

        }

    }

}

/* #################################################
 // Check Sources and Sinks and Apply
 ################################################# */
void OpenWQ_extwatflux_ss::CheckApply_EWFandSS(
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_utils& OpenWQ_utils,
    OpenWQ_units& OpenWQ_units,
    OpenWQ_output& OpenWQ_output,
    const unsigned int YYYY,                            // current model step: Year
    const unsigned int MM,                              // current model step: month
    const unsigned int DD,                              // current model step: day
    const unsigned int HH,                              // current model step: hour
    const unsigned int MIN,                             // current model step: min
    const unsigned int SEC,                             // current model step: sec
    std::string inputType,                              // flag for SS or EWF
    std::unique_ptr<arma::Mat<double>>& array_FORC){    // array FORC arma (SS or EWF)
    
    // Local variables
    unsigned int num_rowdata;       // number of rows of data in JSON (YYYY, MM, DD, HH,...)
    int YYYY_json;                  // Year in JSON-sink_source (interactive)
    int MM_json;                    // Month in JSON-sink_source (interactive)
    int DD_json;                    // Day in JSON-sink_source (interactive)
    int HH_json;                    // Hour in JSON-sink_source (interactive)
    int MIN_json;                   // Hour in JSON-sink_source (interactive)
    int SEC_json;                   // Hour in JSON-sink_source (interactive)
    int DD_max;                     // max number of days for a given month and year

    long sinksource_flag;           // source (=0) or sink (=1)
    time_t jsonTime;                // to get time as time_t for easier comparison with simTime
    time_t simTime;                 // to get time as time_t for easier comparison with jsonTime
    double timeDiffSecs;            // dummy variable to calculate time diference in seconds

    bool anyAll_flag;               // Flag to indicate when at least one "all" is present in row elements
    bool YYYYall_flag, MMall_flag, DDall_flag, \
         HHall_flag, MINall_flag, SECall_flag;   // Flags "all" flags for specific date units
    bool addAnyIncrem_flag=true;     // flag to guarantee increment is added only in one time field (YYYY_json or MM_json or ...) 
    double value_adjust;            // adjusted load (needed when load is continuous or discrete with timestep > MIN)

    /* ########################################
    // Data update/clean-up at 1st timestep
    // Applicable to both SS and EWF
    ######################################## */
    if (OpenWQ_wqconfig.tstep1_flag){

        // Remove requested loads that are prior to the simulation start datetime
        RemoveLoadBeforeSimStart(
            array_FORC,
            OpenWQ_units,
            YYYY,           // current model step: Year
            MM,             // current model step: month
            DD,             // current model step: day
            HH,             // current model step: hour
            MIN,            // current model step: min
            SEC             // current model step: sec
        );

        // Update time increments for rows with "all" elements
        UpdateAllElemTimeIncremts(
            array_FORC,
            OpenWQ_utils,
            OpenWQ_units,
            YYYY,         // current model step: Year
            MM,           // current model step: month
            DD,           // current model step: day
            HH,           // current model step: hour
            MIN,          // current model step: min
            SEC           // current model step: sec
        );

    }

    // Convert sim time to time_t
    simTime = OpenWQ_units.convertTime_ints2time_t(YYYY, MM, DD, HH, MIN, SEC);

    // Get number of rows in SinkSource_FORC
    num_rowdata = (*array_FORC).n_rows; 

    /* ########################################
    // Loop over row data in sink-source file
    ######################################## */

    for (unsigned int ri=0;ri<num_rowdata;ri++){

        // First check if row has already been used
        // Do not skip if continuous load scheme is selected
        // Only applicable to rows without "all" in any datetime row element
        // YYYY, MM, DD, HH, MIN
        if ((*array_FORC)(ri,14) == -2) continue;

        // Reset anyAll_flag
        anyAll_flag = false, YYYYall_flag = false, MMall_flag = false, \
        DDall_flag = false, HHall_flag = false, SECall_flag = false;
        
        // ########################################
        // Check if time in array_FORC row ri matches the current model time
        // Applicable to both SS and EWF

        // Get requested JSON datetime
        YYYY_json = (*array_FORC)(ri,3);
        MM_json =   (*array_FORC)(ri,4);  
        DD_json =   (*array_FORC)(ri,5);  
        HH_json =   (*array_FORC)(ri,6);  
        MIN_json =  (*array_FORC)(ri,7);
        SEC_json =  (*array_FORC)(ri,8);

        // Add the appropriate year step to row elements
        // with "all" flag (= -1)
        if (YYYY_json == -1){YYYY_json  += (*array_FORC)(ri,14); anyAll_flag = true; YYYYall_flag = true;}
        if (MM_json == -1){MM_json      += (*array_FORC)(ri,15); anyAll_flag = true; MMall_flag = true;}
        if (DD_json == -1){DD_json      += (*array_FORC)(ri,16); anyAll_flag = true; DDall_flag = true;}
        if (HH_json == -1){HH_json      += (*array_FORC)(ri,17); anyAll_flag = true; HHall_flag = true;}
        if (MIN_json == -1){MIN_json    += (*array_FORC)(ri,18); anyAll_flag = true; MINall_flag = true;}
        if (SEC_json == -1){SEC_json    += (*array_FORC)(ri,19); anyAll_flag = true; SECall_flag = true;}

        // jsonTime in time_t
        jsonTime = OpenWQ_units.convertTime_ints2time_t(YYYY_json, MM_json, DD_json, HH_json, MIN_json, SEC_json);

        // Skip if not time to load yet
        if (simTime < jsonTime) continue;

        // ########################################
        // If reached here, then it's time to 
        // Apply source or sink 
        // OR 
        // Update EWF concentration: ewf_conc
        // ########################################

        // Determine adjusted SSload based on time increment.
        // This is needed if SECall_flag = "all"
        // And is applicable to both "discrete" and "continous load" because
        // 1) discrete: multiple sub-time step loads
        // 2) continuous: because it's a continuous load that has M/T units
        // Multiplication is by number of seconds between simTime and jsonTime

        value_adjust = (*array_FORC)(ri,12);

        // Adjust load if SS and continuous/discrete-seconds
        if(SECall_flag && inputType.compare("ss") == 0){
            // Time difference between jsonTime and simTime in seconds
            timeDiffSecs = difftime(simTime, jsonTime);
            value_adjust *= timeDiffSecs;
        }

        // Get SS type (source or sink)
        sinksource_flag = (*array_FORC)(ri,2);

        // if SS and SOURCE
        if (inputType.compare("ss") == 0 && sinksource_flag == 0){

            Apply_Source(
                OpenWQ_vars,
                OpenWQ_wqconfig,
                OpenWQ_hostModelconfig,
                OpenWQ_output,
                (*array_FORC)(ri,1),       // compartment model index
                (*array_FORC)(ri,0),       // chemical model index    
                (*array_FORC)(ri,9),       // compartment model ix
                (*array_FORC)(ri,10),      // compartment model iy
                (*array_FORC)(ri,11),      // compartment model iz
                value_adjust);                                 // source load

        }
        // if SS and SINK
        else if (inputType.compare("ss") == 0 && sinksource_flag == 1){

            Apply_Sink(
                OpenWQ_vars,
                OpenWQ_wqconfig,
                OpenWQ_hostModelconfig,
                OpenWQ_output,
                (*array_FORC)(ri,1),       // compartment model index
                (*array_FORC)(ri,0),       // chemical model index    
                (*array_FORC)(ri,9),       // compartment model ix
                (*array_FORC)(ri,10),      // compartment model iy
                (*array_FORC)(ri,11),      // compartment model iz
                value_adjust);            // source load
        }
        // if EWF
        else if (inputType.compare("ewf") == 0){

            // Update ewf_conc
            Update_EWFconc(
                OpenWQ_vars,
                OpenWQ_wqconfig,
                OpenWQ_hostModelconfig,
                OpenWQ_output,
                (*array_FORC)(ri,1),       // EWF model index
                (*array_FORC)(ri,0),       // chemical model index    
                (*array_FORC)(ri,9),       // compartment model ix
                (*array_FORC)(ri,10),      // compartment model iy
                (*array_FORC)(ri,11),      // compartment model iz
                value_adjust);             // source load

        }

        // ########################################
        // Prepare the time increments for the next load
        // Critical step for both 1-time use of "all" fields
        // Applicable to both SS and EWF
        // ########################################

        // Set it as "used" (not for use anymore) if this load
        // has no "all" (only use one time)
        if (!anyAll_flag) {
            (*array_FORC)(ri,14) = -2;
            continue;
        }

        // Update increment if "row" has all
        // Needed for setting the time for the next load
        // addAnyIncrem_flag makes sure increment is added only in YYYY_json or MM_json or ...
        // limit incremenets to max number of MIN, HH, DD, MM and YYYY
        // Also reset time elements to 1 when they reach their max value

        // Get max no of days in a given month
        DD_max = OpenWQ_utils.getNumberOfDaysInMonthYear(YYYY_json, MM_json);   

        while (simTime >= jsonTime){

            // Reset the addAnyIncrem_flag to enable time increment
            addAnyIncrem_flag = true;
             
            if (SECall_flag && addAnyIncrem_flag && SEC_json<59){           // max = 59 secs
                // next SEC step
                SEC_json++;
                addAnyIncrem_flag = false;
            }else if (MINall_flag && addAnyIncrem_flag && MIN_json<59){     // max = 59 min
                // next MIN step
                MIN_json++;
                if (SECall_flag && SEC_json==59)    SEC_json = 0;
                addAnyIncrem_flag = false;
            }else if (HHall_flag && addAnyIncrem_flag && HH_json<23){       // max = 23 h
                // next HH step
                HH_json++;
                if (SECall_flag && SEC_json==59)    SEC_json = 0;
                if (MINall_flag && MIN_json==59)    MIN_json = 0;
                addAnyIncrem_flag = false;
            }else if (DDall_flag && addAnyIncrem_flag && DD_json<DD_max){   // max DD depends on month: DD_max
                // next DD step
                DD_json++;
                if (SECall_flag && SEC_json==59)    SEC_json = 0;
                if (MINall_flag && MIN_json==59)    MIN_json = 0;
                if (HHall_flag && HH_json==23)      HH_json = 0;
                addAnyIncrem_flag = false;
            }else if (MMall_flag && addAnyIncrem_flag && MM_json<12){       // max = 12 months
                // next MM step
                MM_json++;
                if (SECall_flag && SEC_json==59)    SEC_json = 0;
                if (MINall_flag && MIN_json==59)    MIN_json = 0;
                if (HHall_flag && HH_json==23)      HH_json = 0;
                if (DDall_flag && DD_json==DD_max)  DD_json = 1;
                addAnyIncrem_flag = false;
            }else if (YYYYall_flag && addAnyIncrem_flag){                   // max = no limit
                // next YYYY step
                YYYY_json++;
                if (SECall_flag && SEC_json==59)    SEC_json = 0;
                if (MINall_flag && MIN_json==59)    MIN_json = 0;
                if (HHall_flag && HH_json==23)      HH_json = 0;
                if (DDall_flag && DD_json==DD_max)  DD_json = 1;
                if (MMall_flag && MM_json==12)      MM_json = 1;
                addAnyIncrem_flag = false;
            }

            // Exit while loop if not increment has been added => addAnyIncrem_flag=true
            // it means that the continuous or dicrete-sec load is out of range
            if(addAnyIncrem_flag) break;

            // Determine new jsonTime for checking in while loop
            jsonTime = OpenWQ_units.convertTime_ints2time_t(YYYY_json, MM_json, DD_json, HH_json, MIN_json, SEC_json);

        }

        // Update increment
        // Set it as "used" (not for use anymore) if addAnyIncrem_flag=true 
        // so, no time increment was possible exceeded possible increments in "all" elements
        if (addAnyIncrem_flag) (*array_FORC)(ri,14) = -2;
        else {
            // Otherwise, update the increment
            // needs +1 because the "all" fields use -1 as numeric value (and as flag)
            // so, adding 1 will set it to start from zero to get to the correct time
            if (YYYYall_flag){  (*array_FORC)(ri,14) = YYYY_json + 1;}
            if (MMall_flag){    (*array_FORC)(ri,15) = MM_json   + 1;}
            if (DDall_flag){    (*array_FORC)(ri,16) = DD_json   + 1;}
            if (HHall_flag){    (*array_FORC)(ri,17) = HH_json   + 1;}
            if (MINall_flag){   (*array_FORC)(ri,18) = MIN_json  + 1;}
            if (SECall_flag){   (*array_FORC)(ri,19) = SEC_json  + 1;}
        }

    }

}


/* #################################################
 // Apply Source
 ################################################# */
void OpenWQ_extwatflux_ss::Apply_Source(
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_output& OpenWQ_output,
    const unsigned int cmpi,             // compartment model index
    const unsigned int chemi,            // chemical model index    
    int ix,          // compartment model ix
    int iy,          // compartment model iy
    int iz,          // compartment model iz
    const double ss_data_json){          // source load g

    // Local Variables
    std::string msg_string;             // error/warning message string
    unsigned int spX_min, spX_max, spY_min, spY_max, spZ_min, spZ_max;
    unsigned int nx = std::get<2>(OpenWQ_hostModelconfig.HydroComp[cmpi]);
    unsigned int ny = std::get<3>(OpenWQ_hostModelconfig.HydroComp[cmpi]);
    unsigned int nz = std::get<4>(OpenWQ_hostModelconfig.HydroComp[cmpi]);

    // #####################
    // Determine domain region (or simple grid cells) to add load
    // ix
    if(ix != -1){spX_min = ix; spX_max = ix;}
    else{spX_min = 0; spX_max = nx - 1;}
    // iy
    if(iy != -1){spY_min = iy; spY_max = iy;}
    else{spY_min = 0; spY_max = ny - 1;}
    // iz
    if(iz != -1){spZ_min = iz; spZ_max = iz;}
    else{spZ_min = 0; spZ_max = nz - 1;}

    try{
        // Add mass load (already converted to g units)
        (*OpenWQ_vars.d_chemass_ss)(cmpi)(chemi)(
            arma::span(spX_min, spX_max), 
            arma::span(spY_min, spY_max),
            arma::span(spZ_min, spZ_max)) += ss_data_json;

    }catch (...){
        
        // Through a warning if request out of boundaries
        //Create message
        msg_string = 
            "<OpenWQ> WARNING: Sink/Source load out of boundaries."
            "Requested load ignored: "
            "Compartment=" + OpenWQ_hostModelconfig.cmpt_names[cmpi]
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
void OpenWQ_extwatflux_ss::Apply_Sink(
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_output& OpenWQ_output,
    const unsigned int cmpi,            // compartment model index
    const unsigned int chemi,           // chemical model index    
    int ix,                             // compartment model ix
    int iy,                             // compartment model iy
    int iz,                             // compartment model iz
    const double ss_data_json){         // source load g

    // Local Variables
    std::string msg_string;             // error/warning message string
    unsigned int spX_min, spX_max, spY_min, spY_max, spZ_min, spZ_max;
    unsigned int nx = std::get<2>(OpenWQ_hostModelconfig.HydroComp[cmpi]);
    unsigned int ny = std::get<3>(OpenWQ_hostModelconfig.HydroComp[cmpi]);
    unsigned int nz = std::get<4>(OpenWQ_hostModelconfig.HydroComp[cmpi]);

    // #####################
    // Determine domain region (or simple grid cells) to add load
    // ix
    if(ix != -1){spX_min = ix; spX_max = ix;}
    else{spX_min = 0; spX_max = nx - 1;}
    // iy
    if(iy != -1){spY_min = iy; spY_max = iy;}
    else{spY_min = 0; spY_max = ny - 1;}
    // iz
    if(iz != -1){spZ_min = iz; spZ_max = iz;}
    else{spZ_min = 0; spZ_max = nz - 1;}

    try{

        // Remove mass load (already converted to g units)
        (*OpenWQ_vars.d_chemass_ss)(cmpi)(chemi)(
            arma::span(spX_min, spX_max), 
            arma::span(spY_min, spY_max),
            arma::span(spZ_min, spZ_max)) -= ss_data_json;

        // Replace all negative values by zero
        // Needed because ss_data_json can be larger than available mass
        (*OpenWQ_vars.d_chemass_ss)(cmpi)(chemi).transform( [](double val) { return (val < 0.0) ? 0.0 : val; });

    }catch (...){

        // Through a warning if request out of boundaries
        // Create Message
        msg_string = 
            "<OpenWQ> WARNING: Sink/Source load out of boundaries."
            "Requested load ignored: "
            "Compartment=" + OpenWQ_hostModelconfig.cmpt_names[cmpi]
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
 // Apply Sink
 ################################################# */
void OpenWQ_extwatflux_ss::Update_EWFconc(
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_output& OpenWQ_output,
    const unsigned int ewfi,            // compartment model index
    const unsigned int chemi,           // chemical model index    
    int ix,                             // compartment model ix
    int iy,                             // compartment model iy
    int iz,                             // compartment model iz
    const double new_concVal){          // new EWF conc value

    // Local Variables
    std::string msg_string;             // error/warning message string
    unsigned int spX_min, spX_max, spY_min, spY_max, spZ_min, spZ_max;
    unsigned int nx = std::get<2>(OpenWQ_hostModelconfig.HydroExtFlux[ewfi]);
    unsigned int ny = std::get<3>(OpenWQ_hostModelconfig.HydroExtFlux[ewfi]);
    unsigned int nz = std::get<4>(OpenWQ_hostModelconfig.HydroExtFlux[ewfi]);

    // #####################
    // Determine domain region (or simple grid cells) to add load
    // ix
    if(ix != -1){spX_min = ix; spX_max = ix;}
    else{spX_min = 0; spX_max = nx - 1;}
    // iy
    if(iy != -1){spY_min = iy; spY_max = iy;}
    else{spY_min = 0; spY_max = ny - 1;}
    // iz
    if(iz != -1){spZ_min = iz; spZ_max = iz;}
    else{spZ_min = 0; spZ_max = nz - 1;}

    try{

        // First reset all values of ewf_conc to ZERO for new time step
        #pragma omp parallel for collapse(2) num_threads(OpenWQ_wqconfig.num_threads_requested)
        for (unsigned int ewfi=0;ewfi<OpenWQ_hostModelconfig.num_EWF;ewfi++){ // 
            for (unsigned int chemi=0;chemi<(OpenWQ_wqconfig.BGC_general_num_chem);chemi++){
                (*OpenWQ_vars.ewf_conc)(ewfi)(chemi).zeros();}}
        
        // Now update the elements with information in the EWF file
        (*OpenWQ_vars.ewf_conc)(ewfi)(chemi)(
            arma::span(spX_min, spX_max), 
            arma::span(spY_min, spY_max),
            arma::span(spZ_min, spZ_max)) = new_concVal;

        // Replace all negative values by zero
        // Needed because can have negative values
        (*OpenWQ_vars.ewf_conc)(ewfi)(chemi).transform( [](double val) { return (val < 0.0) ? 0.0 : val; });

    }catch (...){

        // Through a warning if request out of boundaries
        // Create Message
        msg_string = 
            "<OpenWQ> WARNING: EWF conc out of boundaries."
            "Requested EWF concentration update ignored and set to zero: "
            "EWF=" + OpenWQ_hostModelconfig.cmpt_names[ewfi]
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
bool OpenWQ_extwatflux_ss::getModIndex(
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


/* #################################################
 // Get SS vector element
 // CASE IF: elemEntry as string "all"
 ################################################# */
 bool OpenWQ_extwatflux_ss::getArrayElem(
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_output& OpenWQ_output,
    std::string elemName,
    std::__cxx11::basic_string<char> elemEntry,
    int& elemVal,
    unsigned int& file_i,
    unsigned int& struc_i,
    unsigned int& row_i){

    // Local Variable
    bool validEntryFlag = true;
    std::string msg_string;
    
    
    if(elemEntry.compare("ALL") == 0){
        
        elemVal = OpenWQ_wqconfig.allSS_flag;

    }else{ 
        
        // Through a warning invalid entry           
        msg_string = 
            "<OpenWQ> WARNING: SS '" 
                            + elemName 
                            + "' cannot be zero. It needs to start in one (entry skipped): File=" 
                            + std::to_string(file_i+1)
                            + ", Sub_structure=" + std::to_string(struc_i+1)
                            + ", Data_row=" + std::to_string(row_i + 1);   

        OpenWQ_output.ConsoleLog(   // Print it (Console and/or Log file)
            OpenWQ_wqconfig,        // for Log file name
            msg_string,             // message
            true,                   // print in console
            true);                  // print in log file

        validEntryFlag = false;
    }

    return validEntryFlag;

}

/* #################################################
// At timestep 1, 
// remove requested loads that are prior to the simulation start datetime
// only do this for rows that don't have any "all" elements
#################################################*/
void OpenWQ_extwatflux_ss::RemoveLoadBeforeSimStart(
    std::unique_ptr<arma::Mat<double>>& array_FORC,
    OpenWQ_units& OpenWQ_units,
    const int YYYY,         // current model step: Year
    const int MM,           // current model step: month
    const int DD,           // current model step: day
    const int HH,           // current model step: hour
    const int MIN,          // current model step: min
    const int SEC){         // current model step: sec

    // Local variables
    bool all_flag = false, allinYYYY_flag = false;
    int YYYY_json, MM_json, DD_json, HH_json, MIN_json, SEC_json;
    time_t jsonTime, simTime;
    unsigned int num_rowdata, n_rows2remove;
    std::vector<int> rows2Remove;  // List of rows indexes to remove     

    // Convert sim time to time_t
    simTime = OpenWQ_units.convertTime_ints2time_t(YYYY, MM, DD, HH, MIN, SEC);

    // Get number of rows in SinkSource_FORC
    num_rowdata = (*array_FORC).n_rows; 

    /* ########################################
    // Loop over row data in sink-source file
    ######################################## */

    for (unsigned int ri=0;ri<num_rowdata;ri++){

        // Reset all entry exists flag
        all_flag = false;
        allinYYYY_flag = false; // this is helpful because we can exclude the rows that may have "all" in rows other than
                                // the YYYY. If that YYYY is lower than the simulation, then the row can also be removed

        // Get requested JSON datetime
        YYYY_json = (*array_FORC)(ri,3);
        MM_json = (*array_FORC)(ri,4);  
        DD_json = (*array_FORC)(ri,5);  
        HH_json = (*array_FORC)(ri,6);  
        MIN_json = (*array_FORC)(ri,7);
        SEC_json = (*array_FORC)(ri,8);

        // Skip any entry = -1 ('all' flag, then replace by current sim time)
        if (YYYY_json == -1){YYYY_json = YYYY; all_flag=true; allinYYYY_flag=true;}
        if (MM_json == -1){MM_json = MM; all_flag=true;}
        if (DD_json == -1){DD_json = DD; all_flag=true;}
        if (HH_json == -1){HH_json = HH; all_flag=true;}
        if (MIN_json == -1){MIN_json = MIN; all_flag=true;}
        if (SEC_json == -1){SEC_json = SEC; all_flag=true;}

        // jsonTime in time_t
        jsonTime = OpenWQ_units.convertTime_ints2time_t(
            YYYY_json, 
            MM_json, 
            DD_json, 
            HH_json, 
            MIN_json,
            SEC_json);
        
        // Save index of row to remove
        // only for the !all_flag rows
        if (!all_flag && jsonTime < simTime){ // cases without any "all" (in any row element)
            rows2Remove.push_back(ri);
        }else if(!allinYYYY_flag && jsonTime < simTime 
                && YYYY_json < YYYY){ // case without "all" in YYYY element where YYYY_json < YYYY)
            rows2Remove.push_back(ri);
        }

    }

    // arma .shed_row only accepts uvec
    // so, need to convert std::vector into arma::uvec
    // initiate this vector
    n_rows2remove = rows2Remove.size();
    arma::uvec rows2Remove_uvec(n_rows2remove);  

    // Loop over number of to-remove indexes
    for (unsigned int ri=0;ri<n_rows2remove;ri++){
        rows2Remove_uvec(ri) = rows2Remove[ri];
    }

    // Remove the identified past row data 
    (*array_FORC).shed_rows(rows2Remove_uvec); 

}

/* #################################################
// At timestep 1, 
// adjust time increments for YYYY, MM, DD, HH, MIN
#################################################*/
void OpenWQ_extwatflux_ss::UpdateAllElemTimeIncremts(
    std::unique_ptr<arma::Mat<double>>& array_FORC,
    OpenWQ_utils& OpenWQ_utils,
    OpenWQ_units& OpenWQ_units,
    const int YYYY,         // current model step: Year
    const int MM,           // current model step: month
    const int DD,           // current model step: day
    const int HH,           // current model step: hour
    const int MIN,          // current model step: min
    const int SEC){          // current model step: sec

    // Local variables
    unsigned num_rowdata;                                       // number of SS row data
    bool all_YYYY_flag = false, all_MM_flag = false, all_DD_flag = false, \
         all_HH_flag = false, all_MIN_flag = false, all_SEC_flag = false;
    int YYYY_json, MM_json, DD_json, HH_json, MIN_json, SEC_json;
    time_t jsonTime, simTime;
    unsigned int increm1, increm2, increm3, increm4, increm5, increm6;  // for interactive trial-error to get mininum increment
    std::vector<int> rows2Remove;                                       // List of rows indexes to remove
    unsigned int DD_max;                                                // max number of days for a given month and year
    
    // Convert sim time to time_t
    simTime = OpenWQ_units.convertTime_ints2time_t(YYYY, MM, DD, HH, MIN, SEC);

    // Get number of rows in SinkSource_FORC
    num_rowdata = (*array_FORC).n_rows; 

    /* ########################################
        // Loop over row data in sink-source file
    ######################################## */

    for (unsigned int ri=0;ri<num_rowdata;ri++){

        // Reset all entry exists flag
        all_YYYY_flag = false; all_MM_flag = false; all_DD_flag = false;
        all_HH_flag = false; all_MIN_flag = false;
        increm1=0, increm2=0, increm3=0, increm4=0, increm5=0, increm6=0;

        // Get requested JSON datetime
        YYYY_json = (*array_FORC)(ri,3);
        MM_json = (*array_FORC)(ri,4);  
        DD_json = (*array_FORC)(ri,5);  
        HH_json = (*array_FORC)(ri,6);  
        MIN_json = (*array_FORC)(ri,7);
        SEC_json = (*array_FORC)(ri,8);

        // Determine the "all" elements, which will be our degrees of freedom
        if (YYYY_json == -1){all_YYYY_flag=true;}
        if (MM_json == -1){all_MM_flag=true;}
        if (DD_json == -1){all_DD_flag=true;}
        if (HH_json == -1){all_HH_flag=true;}
        if (MIN_json == -1){all_MIN_flag=true;}
        if (SEC_json == -1){all_SEC_flag=true;}

        // If there aren't any "all" elements, then set it to zero
        // and go to the next row
        if (!all_YYYY_flag && !all_MM_flag && !all_DD_flag 
            && !all_HH_flag && !all_MIN_flag && !all_SEC_flag){
                (*array_FORC)(ri,14) = -1;
                continue;
        }

        // First interation to get closer to current timestep based on simtime
        // and degrees of freedom from "all" elements. Only changes the min step possible
        if (!all_YYYY_flag & (YYYY_json > YYYY)){
            // if YYYY_json is not "all" and is higher than the current sim YYYY,
            // then we just need to set the values of MM, DD, HH, MIN have to the min values
            // that means, Jan-1 00:00 of that yeat
            // for that, we need to add 1 or or 2 because the "all" flag is -1, 
            // add 1 for SEC, MIN and HH because they should start at 0
            // add 2 for DD and MM because they should start at 1
            if (all_SEC_flag){increm6 = 1;}
            if (all_MIN_flag){increm5 = 1;}
            if (all_HH_flag){increm4 = 1;}
            if (all_DD_flag){increm3 = 2;}
            if (all_MM_flag){increm2 = 2;}
        }else if (!all_YYYY_flag & (YYYY_json == YYYY) 
              && (!all_MM_flag & (MM_json > MM))){
            if (all_SEC_flag){increm6 = 1;}
            if (all_MIN_flag){increm5 = 1;}
            if (all_HH_flag){increm4 = 1;}
            if (all_DD_flag){increm3 = 2;}
        }else if (!all_YYYY_flag & (YYYY_json == YYYY) 
              && (!all_MM_flag & (MM_json == MM))
              && (!all_DD_flag & (DD_json == DD))){
            if (all_SEC_flag){increm6 = 1;}
            if (all_MIN_flag){increm5 = 1;}
            if (all_HH_flag){increm4 = 1;}
        }else if (!all_YYYY_flag & (YYYY_json == YYYY) 
              && (!all_MM_flag & (MM_json == MM))
              && (!all_DD_flag & (DD_json == DD))
              && (!all_HH_flag & (HH_json == HH))){
            if (all_SEC_flag){increm6 = 1;}
            if (all_MIN_flag){increm5 = 1;}
        }else if (!all_YYYY_flag & (YYYY_json == YYYY) 
              && (!all_MM_flag & (MM_json == MM))
              && (!all_DD_flag & (DD_json == DD))
              && (!all_HH_flag & (HH_json == HH))
              && (!all_MIN_flag & (MIN_json == MIN))){
            if (all_SEC_flag){increm6 = 1;}
        }else{
            // if YYYY_json is in same year as YYYY, then we need to look for the closest month, day, hour and mib
            if (all_SEC_flag){increm6 = SEC - SEC_json;}
            if (all_MIN_flag){increm5 = MIN - MIN_json;}
            if (all_HH_flag){increm4 = HH - HH_json;}
            if (all_DD_flag){increm3 = DD - DD_json;}
            if (all_MM_flag){increm2 = MM - MM_json;}
            if (all_YYYY_flag){increm1 = YYYY - YYYY_json;}
        }

        // Determine new jsonTime if using the first guess for the increment
        jsonTime = OpenWQ_units.convertTime_ints2time_t(
            YYYY_json + increm1, 
            MM_json + increm2, 
            DD_json + increm3, 
            HH_json + increm4, 
            MIN_json + increm5,
            SEC_json + increm6);

        // ###############################################
        // Find the minimum increment needed in the "all" elements, so that
        // we go ahead of the current simulation timestep
        
        if (jsonTime < simTime){

            // Try changing 
            // at MIN scale (if it is an "all" element)
            if (all_SEC_flag){
                while(jsonTime < simTime && (SEC_json + increm6) < 59){
                    increm6++;
                    jsonTime = OpenWQ_units.convertTime_ints2time_t(
                        YYYY_json + increm1, MM_json + increm2, DD_json + increm3, 
                        HH_json + increm4, MIN_json + increm5, SEC_json + increm6);
                }
                // If MIN increment not sufficient, then set it to 1
                // so that it gets MIN_json=0, and then try the next 
                // datetime level
                if (jsonTime < simTime){increm6 = 1;}
            }else{
                increm6 = 1;}

            // Try changing 
            // at MIN scale (if it is an "all" element)
            if (all_MIN_flag){
                while(jsonTime < simTime && (MIN_json + increm5) < 59){
                    increm5++;
                    jsonTime = OpenWQ_units.convertTime_ints2time_t(
                        YYYY_json + increm1, MM_json + increm2, DD_json + increm3, 
                        HH_json + increm4, MIN_json + increm5, SEC_json + increm6);
                }
                // If MIN increment not sufficient, then set it to 1
                // so that it gets MIN_json=0, and then try the next 
                // datetime level
                if (jsonTime < simTime){increm5 = 1;}
            }else{
                increm5 = 1;}
            
            // Try changing 
            // at HH scale (if it is an "all" element)
            if (all_HH_flag){
                while(jsonTime < simTime && (HH_json + increm4) < 23){
                    increm4++;
                    jsonTime = OpenWQ_units.convertTime_ints2time_t(
                        YYYY_json + increm1, MM_json + increm2, DD_json + increm3, 
                        HH_json + increm4, MIN_json + increm5, SEC_json + increm6);
                }
                // If MIN increment not sufficient, then set it to 1
                // so that it gets MIN_json=0, and then try the next 
                // datetime level
                if (jsonTime < simTime){increm4 = 1;}
            }else{
                increm4 = 1;}
            
            // Try changing 
            // at DD scale (if it is an "all" element)
            DD_max = OpenWQ_utils.getNumberOfDaysInMonthYear(YYYY_json, MM_json);
            if (all_DD_flag){
                while(jsonTime < simTime && (DD_json + increm3) < DD_max){
                    increm3++;
                    jsonTime = OpenWQ_units.convertTime_ints2time_t(
                        YYYY_json + increm1, MM_json + increm2, DD_json + increm3, 
                        HH_json + increm4, MIN_json + increm5, SEC_json + increm6);
                }
                // If MIN increment not sufficient, then set it to 1
                // so that it gets MIN_json=0, and then try the next 
                // datetime level
                if (jsonTime < simTime){increm3 = 2;}
            }else{
                increm3 = 1;}

            // Try changing 
            // at MM scale (if it is an "all" element)
            if (all_MM_flag){
                while(jsonTime < simTime && (MM_json + increm2) < 12){
                    increm2++;
                    jsonTime = OpenWQ_units.convertTime_ints2time_t(
                        YYYY_json + increm1, MM_json + increm2, DD_json + increm3, 
                        HH_json + increm4, MIN_json + increm5, SEC_json + increm6);
                }
                // If MIN increment not sufficient, then set it to 1
                // so that it gets MIN_json=0, and then try the next 
                // datetime level
                if (jsonTime < simTime){increm2 = 2;}
            }else{
                increm2 = 1;}

            // Try changing 
            // at YYYY scale (if it is an "all" element)
            if (all_YYYY_flag){
                while(jsonTime < simTime){
                    increm1++;
                    jsonTime = OpenWQ_units.convertTime_ints2time_t(
                        YYYY_json + increm1, MM_json + increm2, DD_json + increm3, 
                        HH_json + increm4, MIN_json + increm5, SEC_json + increm6);
                }
            }

        }

        // Update increments in SinkSource_FORC
        (*array_FORC)(ri,14) = increm1;
        (*array_FORC)(ri,15) = increm2;
        (*array_FORC)(ri,16) = increm3;
        (*array_FORC)(ri,17) = increm4;
        (*array_FORC)(ri,18) = increm5;
        (*array_FORC)(ri,19) = increm6;

    }

}

// Add new row to SinkSource_FORC or ExtFlux_FORC
void OpenWQ_extwatflux_ss::AppendRow_SS_EWF_FORC(
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    std::string inputType,
    arma::vec row_data_col){

    // Local variables            
    arma::Mat<double> row_data_row;                     // new row data (initially as col data)
    int int_n_elem;

    // Transpose vector for adding to SinkSource_FORC as a new row
    row_data_row = row_data_col.t();

    // Add new row_data_row to SinkSource_FORC   
    if (inputType.compare("ss")==0)     int_n_elem = (*OpenWQ_wqconfig.SinkSource_FORC).n_rows;
    if (inputType.compare("ewf")==0)    int_n_elem = (*OpenWQ_wqconfig.ExtFlux_FORC).n_rows;
    
    if (inputType.compare("ss")==0){ (*OpenWQ_wqconfig.SinkSource_FORC).insert_rows(
        std::max(int_n_elem-1,0),
        row_data_row);}
    else if (inputType.compare("ewf")==0){ (*OpenWQ_wqconfig.ExtFlux_FORC).insert_rows(
        std::max(int_n_elem-1,0),
        row_data_row);}

}

// Extract H5 row to row_data_col
arma::vec OpenWQ_extwatflux_ss::ConvertH5row2ArmaVec(
    OpenWQ_units& OpenWQ_units,
    std::vector<double> unit_multiplers,
    time_t timestamp_time_t,
    int x_externModel, 
    int y_externModel, 
    int z_externModel,
    double cmp_recipient,
    arma::mat dataEWF_h5, 
    int rowi,
    int chemi){

    // Local variables
    arma::vec row_data_col;                     // new row data (initially as col data)
    double conc_h5;
    double ix_h5, iy_h5, iz_h5;
    std::tm* timeStructure_tm;           // time stucture

    // Generate time structure from time_t
    timeStructure_tm = gmtime(&timestamp_time_t);

    // Get concentration
    conc_h5 = dataEWF_h5(rowi);

    // Convert conc to local units
    OpenWQ_units.Convert_Units(
        conc_h5,               // value passed by reference so that it can be changed
        unit_multiplers);       // units

    // TO ASSIGN PROPERLY
    double sinksource_ssi = 1;
    ix_h5 = 1;
    iy_h5 = 1;
    iz_h5 = 1;
    double loadScheme_id = 0;

    // Generate the arma::vec row_data_col
    row_data_col = {
        (double) chemi,
        (double) cmp_recipient,
        (double) sinksource_ssi,
        (double) timeStructure_tm->tm_year,
        (double) timeStructure_tm->tm_mon,
        (double) timeStructure_tm->tm_hour,
        (double) timeStructure_tm->tm_mday,
        (double) timeStructure_tm->tm_min,
        (double) timeStructure_tm->tm_sec,
        (double) ix_h5,
        (double) iy_h5,
        (double) iz_h5,
        conc_h5,
        loadScheme_id,  // load scheme (0) not applicable, (1) discrete or (2) continuous
        0,0,0,0,0,0     // field to specify the number of times it has been used aleady
        };              // in the case of and "all" element (YYYY, MM, DD, HH, MIN, SEC)
                        // it starts with 0 (zero), meaning that has not been used
                        // if not an "all" element, then it's set to -1

    return row_data_col;
}