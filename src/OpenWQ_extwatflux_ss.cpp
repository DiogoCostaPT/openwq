
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
            }catch(...){continue;}

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
                
                // h5 format only supported for ewf
                if(inputType.compare("ewf")==0){

                    // if H5 format
                    Set_EWF_h5(
                        OpenWQ_hostModelconfig,
                        OpenWQ_wqconfig,
                        OpenWQ_utils,
                        OpenWQ_units,
                        OpenWQ_output,
                        EWF_SS_json[std::to_string(ssf+1)][std::to_string(ssi+1)],  // relevant sub-json
                        inputType, // ss or ewf
                        foundflag);

                }else if(inputType.compare("ss")==0){

                    // Create Message (Warning Message)
                    msg_string = 
                        "<OpenWQ> WARNING: HDF5 input only supported for'" 
                        " EWF forcing. For SS forcing, please use JSON or "
                        "ASCII format (entry skipped).";

                    // Print it (Console and/or Log file)
                    OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true); 

                }

            }else{

                // Create Message (Warning Message)
                msg_string = 
                    "<OpenWQ> WARNING: Unkown data format='" 
                    + DataFormat
                    + "' in SS or EWF json files > "
                    + " > DATA_FORMAT (only supports JSON, ASCII and HD5F) "
                    + "(entry skipped)";

                // Print it (Console and/or Log file)
                OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true); 

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
    json EWF_SS_json_sub_rowi;                  // json row of json-substructure EWF_SS_json_sub 
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
    std::string errorMsgIdentifier;             // error message section identifier
    bool validEntryFlag;                        // valid entry flag to skip problematic row data
    std::string loadScheme_str;                 // Load scheme string: (1) discrete or (2) continuous
    double loadScheme_id;                       // Load scheme id number: (1) discrete or (2) continuous
    std::string contDt_str;                     // time units of continuous load


    // Get element list (compartment for SS, and External fluxes for EWF)
    if (inputType.compare("ss")==0)     elm_list = OpenWQ_hostModelconfig.cmpt_names;
    if (inputType.compare("ewf")==0)    elm_list = OpenWQ_hostModelconfig.ewf_names;

    // Element names: Compartment or External Flux
    if (inputType.compare("ss")==0)       main_keyName = "COMPARTMENT_NAME"; // SS
    else if (inputType.compare("ewf")==0) main_keyName = "EXTERNAL_INPUTFLUX_NAME";    // EWF

    // Get mainKeyName
    // "COMPARTMENT_NAME" in the case of "ss"
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
    errorMsgIdentifier = inputType + " json block";
    Chemical_name = OpenWQ_utils.RequestJsonKeyVal_str(
        OpenWQ_wqconfig, OpenWQ_output,
        EWF_SS_json_sub, "CHEMICAL_NAME",
        errorMsgIdentifier);
            
    // Type (sink or source) (only used in SS)
    if (inputType.compare("ss")==0) {
        errorMsgIdentifier = inputType + " json block";
        Type = OpenWQ_utils.RequestJsonKeyVal_str(
            OpenWQ_wqconfig, OpenWQ_output,
            EWF_SS_json_sub, "TYPE",
            errorMsgIdentifier);
    }else if (inputType.compare("ewf")==0){
        Type = "SOURCE";}

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
    errorMsgIdentifier = inputType + " json block";
    ss_units_json= OpenWQ_utils.RequestJsonKeyVal_str(
        OpenWQ_wqconfig, OpenWQ_output,
        EWF_SS_json_sub, "UNITS",
        errorMsgIdentifier);

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

        // Check if substructure DATA exists
        errorMsgIdentifier = inputType + " json block with DataFormat=" + DataFormat;
        OpenWQ_utils.RequestJsonKeyVal_json(
            OpenWQ_wqconfig, OpenWQ_output,
            EWF_SS_json_sub, "DATA",
            errorMsgIdentifier);

        try{

            // Error msg identifier in case json key not found
            errorMsgIdentifier = inputType + " json block 'DATA' with DataFormat=" + DataFormat;

            // file path
            ascii_FilePath = OpenWQ_utils.RequestJsonKeyVal_str(
                OpenWQ_wqconfig, OpenWQ_output,
                EWF_SS_json_sub["DATA"], "FILEPATH",
                errorMsgIdentifier);

            // delimiter
            ascii_delimiter = OpenWQ_utils.RequestJsonKeyVal_str(
                OpenWQ_wqconfig, OpenWQ_output,
                EWF_SS_json_sub["DATA"], "DELIMITER",
                errorMsgIdentifier);

            // number of header rows
            ascii_numHeaderRows = OpenWQ_utils.RequestJsonKeyVal_int(
                OpenWQ_wqconfig, OpenWQ_output,
                EWF_SS_json_sub["DATA"], "NUMBER_OF_HEADER_ROWS",
                errorMsgIdentifier);            

            // position of key header
            ascii_headerKeyRow = OpenWQ_utils.RequestJsonKeyVal_int(
                OpenWQ_wqconfig, OpenWQ_output,
                EWF_SS_json_sub["DATA"], "HEADER_KEY_ROW",
                errorMsgIdentifier);  

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
                OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);
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
            OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);
            return;
        }
        
    // If JSON
    }else if (DataFormat.compare("JSON")==0){
        // Get number of rows of data in JSON (YYYY, MM, DD, HH,...)
        errorMsgIdentifier = inputType + " json block 'DATA' with DataFormat=" + DataFormat;
        // check if field exists, return if not with error message
        num_rowdata = OpenWQ_utils.RequestJsonKeyVal_json(
            OpenWQ_wqconfig, OpenWQ_output,
            EWF_SS_json_sub, "DATA",
            errorMsgIdentifier).size();
    }

    /* ########################################
    // Loop over row data in sink-source file
    ######################################## */

    for (unsigned int di=0;di<num_rowdata;di++){
        
        // Reset the size to zero (the object will have no elements)
        row_data_col.reset(); 

        // Get row-json di from EWF_SS_json_sub ["DATA"]
        // Abort if not found
        errorMsgIdentifier = inputType + " json block 'DATA', row " 
                            + std::to_string(di) + " with DataFormat=" 
                            + DataFormat;
        EWF_SS_json_sub_rowi = OpenWQ_utils.RequestJsonKeyVal_json(
            OpenWQ_wqconfig, OpenWQ_output,
            EWF_SS_json_sub["DATA"], std::to_string(di+1),
            errorMsgIdentifier);

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
                entryVal = EWF_SS_json_sub_rowi.at(0);}
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
                entryVal = EWF_SS_json_sub_rowi.at(0);}
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
                entryVal = EWF_SS_json_sub_rowi.at(1);}
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
                entryVal = EWF_SS_json_sub_rowi.at(1);}
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
                entryVal = EWF_SS_json_sub_rowi.at(2);}
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
                entryVal = EWF_SS_json_sub_rowi.at(2);}
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
                entryVal = EWF_SS_json_sub_rowi.at(3);}
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
                entryVal = EWF_SS_json_sub_rowi.at(3);}
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
                entryVal = EWF_SS_json_sub_rowi.at(4);}
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
                entryVal = EWF_SS_json_sub_rowi.at(4);}
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
                entryVal = EWF_SS_json_sub_rowi.at(5);}
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
                entryVal = EWF_SS_json_sub_rowi.at(5);}
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
                entryVal = EWF_SS_json_sub_rowi.at(6);}
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

                OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);

                continue; // skip entry
            }

        }catch(...){

            // try as string for the cases where entry is "all"
            std::string entryVal = ""; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub_rowi.at(6);}
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
                entryVal = EWF_SS_json_sub_rowi.at(7);}
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

                OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);

                continue; // skip entry
            }

        }catch(...){

            // try as string for the cases where entry is "all"
            std::string entryVal = ""; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub_rowi.at(7);}
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
                entryVal = EWF_SS_json_sub_rowi.at(8);}
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

                OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);

                continue; // skip entry
            }

        }catch(...){

            // try as string for the cases where entry is "all"
            std::string entryVal = ""; // dummy variable
            // if JSON
            if (DataFormat.compare("JSON")==0){
                entryVal = EWF_SS_json_sub_rowi.at(8);}
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
            entryVal = EWF_SS_json_sub_rowi.at(9);}
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
            OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);
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
                entryVal = EWF_SS_json_sub_rowi.at(10);}
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
                    OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);

                }else if (loadScheme_str.compare("CONTINUOUS") == 0 && MM_json == -1){
                    // continuous option needs MIN = 'all' (otherwise it's discrete input)
                    loadScheme_id = 1;
                    // get time units
                    try{

                        std::string entryVal = ""; // dummy variable
                        // if JSON
                        if (DataFormat.compare("JSON")==0){
                            entryVal = EWF_SS_json_sub_rowi.at(11);}
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
                        OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);
                        continue; // skip entry
                    }
                }else{

                    // Print it (Console and/or Log file)
                    OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);

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
                OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);

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
                OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);

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

        // Add new row to SinkSource_FORC or ExtFlux_FORC_jsonAscii
        AppendRow_SS_EWF_FORC_jsonAscii(
            OpenWQ_wqconfig,
            inputType,
            row_data_col);

    }

 }

/* #################################################
 // Prepare SS and EWF input data for use at running time: 
 // Case: JSON and ASCII format
 ################################################# */
void OpenWQ_extwatflux_ss::Set_EWF_h5(
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_utils& OpenWQ_utils,
    OpenWQ_units& OpenWQ_units,
    OpenWQ_output& OpenWQ_output,
    json EWF_SS_json_sub,  // relevant sub-json
    std::string inputType,
    bool foundflag){
    
    // Local variables
    std::string msg_string;                 // console/logFile message
    std::size_t it;                         // interactor
    std::string ewf_h5_folderPath;          // json input: folder with EWF input *.h5 files
    std::string ewf_filenamePath;           // full path for *.h5 files
    std::string ewf_h5_units;               // json input: units of EWF h5 files
    std::string ewf_h5_units_file;          // Same as above, but "\" replaced by "|" for file search
    double conc_h5_rowi;                    // iteractive concentration extracted for h5 files
    bool volume_unit_flag;                  // flag to identify the existence of volume in units, i.e., concentration
    std::vector<std::string> units;         // Vector with units information
    std::string external_compartName;       // json input: External compartment name
    std::string external_waterFluxName;     // EWF name (needs to match name hard coded in hydrolink)
    double external_waterFluxName_id;       // index of external_waterFluxName
    std::string chemname;                   // iteractive chemical name during h5 search
    arma::mat xyzEWF_h5;                    // h5 xyz field 
    arma::mat domain_EWF_h5;                // h5 information about external compartment domain (nx,ny,nz)
    arma::mat dataEWF_h5;                   // h5 data
    std::vector<double> unit_multiplers;    // multiplers (numerator and denominator)
    std::vector<std::string> tSamp_valid;   // save valid time stamps
    time_t tSamp_valid_i_time_t;            // iteractive time_t from h5 timestamp
    arma::vec row_data_col;                                 // new row data (initially as col data)
    int x_externModel, y_externModel, z_externModel;        // iteractive x,y,z info from h5 files
    int x_interface_h5, y_interface_h5, z_interface_h5;     // iteractive x,y,z from interface
    int nx_interface_h5, ny_interface_h5, nz_interface_h5;  // nx,ny,nz of external compartment
    std::string ss_cmp_recipient_name;                      // name of EWF recipient
    json interaction_interface_json;                        // json substructure for interface info
    int index_i;
    bool validEntryFlag;                    // flag for valid entries
    bool foundTimeStmps;                    // flag to record (un)success in finding timestamps
    bool h5_entry_found;                    // flag for successful finding of ewf h5 file
    std::string errorMsgIdentifier;         // Start/head of error message of json key not found
    std::vector<int> valid_interfaceH5rows; // vector with indexes of relevant h5 rows that contain interface data
    int rowi_val;                           // iteractive row number from valid_interfaceH5rows 
    bool flag_newChem;                      // bool to flag new chem, which will create new vector row in ExtFlux_FORC_HDF5vec_data
    bool flag_newTimeStamp;                 // bool to flag new timestep, which will save data into ExtFlux_FORC_HDF5vec_data and ExtFlux_FORC_HDF5vec_time
    int point_print_n;                      // iterative trackking of "." console prints (each timeStamp) for asthetics
    bool flg_newJSON_h5Request = true;      // flag for new json block for ewf-h5
    int h5EWF_request_index;                // Index of ewf-h5 index

    // Get request index
    h5EWF_request_index = (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_time).size();

    // ################################
    // Get JSON info
    // ################################

    errorMsgIdentifier = inputType + " json block with DataFormat=HDF5" ;

    // h5 IC folder path
    ewf_h5_folderPath = OpenWQ_utils.RequestJsonKeyVal_str(
        OpenWQ_wqconfig, OpenWQ_output,
        EWF_SS_json_sub, "FOLDERPATH",
        errorMsgIdentifier);
    // h5 ic units
    ewf_h5_units = OpenWQ_utils.RequestJsonKeyVal_str(
        OpenWQ_wqconfig, OpenWQ_output,
        EWF_SS_json_sub, "UNITS",
        errorMsgIdentifier);
    // get external compartment name (needed for both ss and ewf)
    external_compartName = OpenWQ_utils.RequestJsonKeyVal_str(
        OpenWQ_wqconfig, OpenWQ_output,
        EWF_SS_json_sub, "EXTERNAL_COMPARTMENT_NAME",
        errorMsgIdentifier);
    // Get interface between openwq models
    interaction_interface_json = OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        EWF_SS_json_sub, "INTERACTION_INTERFACE",
        errorMsgIdentifier);
    // Get external flux name
    external_waterFluxName = OpenWQ_utils.RequestJsonKeyVal_str(
        OpenWQ_wqconfig, OpenWQ_output,
        EWF_SS_json_sub, "EXTERNAL_INPUTFLUX_NAME",
        errorMsgIdentifier);
    // Get interpolation method
    (OpenWQ_wqconfig.h5EWF_interpMethod) = OpenWQ_utils.RequestJsonKeyVal_str(
        OpenWQ_wqconfig, OpenWQ_output,
        EWF_SS_json_sub, "INTERPOLATION",
        errorMsgIdentifier);
    
    // replace "/" by "|" is needed because "/" is not compatible with directory full paths
    ewf_h5_units_file = ewf_h5_units;
    it = (int) ewf_h5_units_file.find("/");
    if (it <= ewf_h5_units_file.size()){
        ewf_h5_units_file.replace(it,1, "|");
    }

    // ################################
    // Some pre-processing
    // ################################

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

    // Get corresponding id
    external_waterFluxName_id = 
        (double)OpenWQ_utils.FindStrIndexInVectStr(
            OpenWQ_hostModelconfig.ewf_names,
            external_waterFluxName);

    // If external compartment not found in internal list of EWF, then
    // throw warning msg and skip entry
    if(external_waterFluxName_id==-1.0f){
        msg_string = 
            "<OpenWQ> WARNNING SS json key EXTERNAL_INPUTFLUX_NAME= "
            + external_waterFluxName
            + " not valid for this host-model coupling (entry ignored).";
        OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);
        return;
    }

    // Get valid time steps from the logFile of EWF simulation
    // if timeStamps not found, then return 
    // warning message alerady printed in GetTimeStampsFromLogFile
    foundTimeStmps = OpenWQ_utils.GetTimeStampsFromLogFile(
        OpenWQ_wqconfig,
        OpenWQ_output,
        ewf_h5_folderPath,
        "<OpenWQ> Output export successful (HDF5): ", // Substring of the output to search
        tSamp_valid,
        "SS/EWF load/sink/conc H5 supporting logFile"); // Logfile errMsg identifier
    if (foundTimeStmps==false) return;

    // Throw console message to say it's processing the h5 interface data
    msg_string = "<OpenWQ> EWF HDF5 interface requested.\n"
                 "         Processing and checking interface...";
    OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, false);


    // ################################
    // Loop over EWF h5 files
    // ################################
    // Each chemical species is in different files
    for (unsigned int chemi=0;chemi<OpenWQ_wqconfig.BGC_general_num_chem;chemi++){

        // ############################
        // Get and process interface H5 data 

        // Get chem name
        chemname = (OpenWQ_wqconfig.BGC_general_chem_species_list)[chemi];

        // Set flag to push_back new vector row for chemi ewf data
        flag_newChem = true;

        // Throw consolde update
        msg_string = "         " + external_waterFluxName + " => " + chemname + " .";
        std::string whiteSpacing(msg_string.size()-1,' ');
        std::cout << msg_string << std::flush;

        // Generate full ic filename
        ewf_filenamePath = ewf_h5_folderPath;
        ewf_filenamePath.append("/");
        ewf_filenamePath.append(external_compartName); // compartment
        ewf_filenamePath.append("@");
        ewf_filenamePath.append(chemname);     // chemical name
        ewf_filenamePath.append("#");
        ewf_filenamePath.append(ewf_h5_units_file); // units
        ewf_filenamePath.append("-main.h5"); 

        // Get x,y,z elements in h5 ewf data
        xyzEWF_h5
            .load(arma::hdf5_name(
                ewf_filenamePath,          // file name
                "xyz_elements"));          // options

        // xyzEWF_h5 is empty, 
        // it means that the h5 file requested was not found
        // Throw warning and skip
        if(xyzEWF_h5.is_empty()){
            msg_string = 
                "<OpenWQ> WARNNING " + inputType
                + " h5 file requested=" + ewf_filenamePath
                + " was not found. Revise the json inputs and "
                "corresponding h5 files (entry skipped).";
            OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);
            continue;
        }

        // Get entire entire domain nx, ny, nz from the h5 ewf data
        // This corresponds to the EWF external compartment
        domain_EWF_h5
            .load(arma::hdf5_name(
                ewf_filenamePath,          // file name
                "xyz_elements_total"));          // options

        // Check if entries of INTERACTION_INTERFACE are valid
        // Returns if not a valid interface
        // Error messages are provided inside Convert2NegativeOneIfAll_inputInt()
        // "all" entry is converted into -1, "end" is converted into nx, iy o nz provided in domain_EWF_h5
        // x_interface_h5, y_interface_h5, and z_interface_h5 passed by reference and updated inside function
        // x
        index_i = 0;
        msg_string = "EWF Invalid 'INTERACTION_INTERFACE' array element " 
                    + std::to_string(index_i) + "for HDF5. It only accepts integers or 'all'";
        validEntryFlag = OpenWQ_utils.Convert2NegativeOneIfAll_inputInt(
            OpenWQ_wqconfig, OpenWQ_output, msg_string,
            interaction_interface_json, index_i, x_interface_h5, domain_EWF_h5(0,0));
        if (!validEntryFlag) return;
        // y
        index_i = 1;
        msg_string = "EWF Invalid 'INTERACTION_INTERFACE' array element " 
                    + std::to_string(index_i) + "for HDF5. It only accepts integers or 'all'";
        validEntryFlag = OpenWQ_utils.Convert2NegativeOneIfAll_inputInt(
            OpenWQ_wqconfig, OpenWQ_output, msg_string,
            interaction_interface_json, index_i, y_interface_h5, domain_EWF_h5(0,1));
        if (!validEntryFlag) return;
        // z
        index_i = 2;
        msg_string = "EWF Invalid 'INTERACTION_INTERFACE' array element " 
                    + std::to_string(index_i) + "for HDF5. It only accepts integers or 'all'";
        validEntryFlag = OpenWQ_utils.Convert2NegativeOneIfAll_inputInt(
            OpenWQ_wqconfig, OpenWQ_output, msg_string,
            interaction_interface_json, index_i, z_interface_h5, domain_EWF_h5(0,2));
        if (!validEntryFlag) return;

        // Get the domain of interface external compartment
        if(x_interface_h5==-1) nx_interface_h5 = domain_EWF_h5(0,0);
        else nx_interface_h5 = x_interface_h5;
        if(y_interface_h5==-1) ny_interface_h5 = domain_EWF_h5(0,1);
        else ny_interface_h5 = y_interface_h5;
        if(z_interface_h5==-1) nz_interface_h5 = domain_EWF_h5(0,2);
        else nz_interface_h5 = z_interface_h5;


        // ################################
        // Check if necessary external compartment elements exists in EWF h5 file
        // Check is carried out for the first timestamp
        // Interface h5 rows saved in: valid_interfaceH5rows
        // ################################

        // Get the corresponding data
        dataEWF_h5
            .load(arma::hdf5_name(
                ewf_filenamePath,              // file name
                tSamp_valid[0]));          // options

        // Loop over domain and check if 
        for (int x_intrf=0;x_intrf<nx_interface_h5;x_intrf++){
            for (int y_intrf=0;y_intrf<ny_interface_h5;y_intrf++){
                for (int z_intrf=0;z_intrf<nz_interface_h5;z_intrf++){
                    
                    h5_entry_found = false;
                        
                    // Seach for interface element entry row by row
                    for (int rowi=0;rowi<(int)xyzEWF_h5.n_rows;rowi++){

                        // Get element x,y,z indexes of external model
                        // Using convention of external model
                        // Needs to be converted into local openwq implementation
                        x_externModel = xyzEWF_h5(rowi, 0);
                        y_externModel = xyzEWF_h5(rowi, 1);
                        z_externModel = xyzEWF_h5(rowi, 2);

                        // Save ewf conc data if at the interface
                        if ((x_externModel == x_intrf + 1)
                            && (y_externModel == y_intrf + 1)
                            && (z_externModel == z_intrf + 1)){
                            
                            // Save valid row index
                            valid_interfaceH5rows.push_back(rowi);
                            h5_entry_found = true;
                            break;
                        }
                    }

                    // Through warning message if the interface element 
                    // is not available in the ewf h5 file
                    if (h5_entry_found==false){
                        msg_string = 
                            "<OpenWQ> WARNNING " + inputType
                            + " h5 file requested=" + ewf_filenamePath
                            + " has been found, but it does not contain the interface element ("
                            + std::to_string(x_intrf) + "," + std::to_string(y_intrf) + ","
                            + std::to_string(z_intrf) + "). Make sure to load an EWF h5 file that has all the"
                            " interface elements. This element has been defaulted to zero (entry skipped).";
                        OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);
                    }
                }
            }
        }

        // ############################
        // Loop over H5 timeSteps data and save interface cells conc
        // in ExtFlux_FORC_HDF5vec_data and ExtFlux_FORC_HDF5vec_time
        // ############################
        flag_newTimeStamp = false;
        point_print_n = 0;

        for (long unsigned int tSamp=0;tSamp<tSamp_valid.size();tSamp++){

            // Get the corresponding data
            dataEWF_h5
                .load(arma::hdf5_name(
                    ewf_filenamePath,              // file name
                    tSamp_valid[tSamp]));          // options

            // Get timestamp sting into time_t
            tSamp_valid_i_time_t = OpenWQ_units.convertTime_str2time_t(
                tSamp_valid[tSamp]);

            // Loop over HDF5 row data referring to the interface
            for (int rowi=0;rowi<(int)valid_interfaceH5rows.size();rowi++){

                // Get valid row
                rowi_val = valid_interfaceH5rows[rowi];

                // Get element x,y,z indexes of external model
                // Using convention of external model
                // Needs to be converted into local openwq implementation
                x_externModel = xyzEWF_h5(rowi_val, 0);
                y_externModel = xyzEWF_h5(rowi_val, 1);
                z_externModel = xyzEWF_h5(rowi_val, 2);

                // Get concentration
                conc_h5_rowi = dataEWF_h5(rowi_val);

                // Convert conc to local units
                OpenWQ_units.Convert_Units(
                    conc_h5_rowi,               // value passed by reference so that it can be changed
                    unit_multiplers);           // units
                
                // Extract H5 row to row_data_col
                row_data_col = ConvertH5row2ArmaVec(
                    external_waterFluxName_id,
                    x_externModel, y_externModel,z_externModel,
                    conc_h5_rowi,
                    chemi);

                // Add new row to ExtFlux_FORC_data_tStep
                // if last row, then append ExtFlux_FORC_data_tStep
                // into ExtFlux_FORC_HDF5vec

                if (rowi==(int)valid_interfaceH5rows.size()-1){
                     flag_newTimeStamp = true;}

                AppendRow_SS_EWF_FORC_h5(
                    OpenWQ_wqconfig,
                    h5EWF_request_index,
                    chemi,
                    flg_newJSON_h5Request,
                    flag_newChem,
                    flag_newTimeStamp,
                    tSamp_valid_i_time_t,
                    row_data_col);

                // Reset flags
                flg_newJSON_h5Request = false;
                flag_newTimeStamp = false;
                flag_newChem = false;

            }

            // Throw a point in console to show progress
            // One point per timeStep
            point_print_n++;
            if (point_print_n==80){
                point_print_n=0; 
                std::cout << "\n" + whiteSpacing << std::flush;
            }
            std::cout << "." << std::flush;
            
        }

        // Print the number of timesteps in the console
        std::cout << " => TimeSteps processed: " + std::to_string(tSamp_valid.size()) + "\n" << std::flush;
    }
}

/* #################################################
// Check Sources/Sinks and EWF and Apply
// only for JSON and ASCII input
################################################# */
void OpenWQ_extwatflux_ss::CheckApply_EWFandSS_jsonAscii(
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
        RemoveLoadBeforeSimStart_jsonAscii(
            OpenWQ_units,
            array_FORC,
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
            Update_EWFconc_jsonAscii(
                OpenWQ_vars,
                OpenWQ_wqconfig,
                OpenWQ_hostModelconfig,
                OpenWQ_output,
                (*array_FORC)(ri,1),       // EWF model index
                (*array_FORC)(ri,0),       // chemical model index    
                (*array_FORC)(ri,9),       // compartment model ix
                (*array_FORC)(ri,10),      // compartment model iy
                (*array_FORC)(ri,11),      // compartment model iz
                value_adjust);             // new concentration

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
// Check EWF and apply
// only H5 input
################################################# */
void OpenWQ_extwatflux_ss::CheckApply_EWF_h5(
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
    const unsigned int SEC){                            // current model step: sec

    // Local variables
    time_t simTime;                         // current simulation time in time_t
    time_t h5EWF_time;                      // iteractive time extraction from FORC_vec_time_t
    unsigned long long num_ewfh5_requests;  // number of json ewf-h5 requests
    unsigned long long num_chems;           // number of chems in h5 stucture
    unsigned long long num_timeStamps;      // number of timesteps in h5 stucture
    arma::dmat h5Conc_chemi_before;         // iteractive h5-chemi concentration at previous timestep 
    arma::dmat h5Conc_chemi_next;           // iteractive h5-chemi concentration at current or next timestep 
    arma::dmat h5Conc_chemi_interp;         // iteractive h5-chemi concentration at interpolated timestep 


    // Get number of json ewf-h5 requests
    num_ewfh5_requests = (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_time).size();

    // Convert sim time to time_t
    simTime = OpenWQ_units.convertTime_ints2time_t(YYYY, MM, DD, HH, MIN, SEC);

    /* ########################################
    // Data update/clean-up at 1st timestep
    ######################################## */
    if (OpenWQ_wqconfig.tstep1_flag){

        // Loop over all requests
        for (int reqi=0;reqi<num_ewfh5_requests;reqi++){
            // Loop over all of them to removee load before simtime
            RemoveLoadBeforeSimStart_h5(
                OpenWQ_units,
                OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_data,  // vec with h5 interface data
                OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_time,  // vec with h5 interface timestamps
                reqi,           // index of json request for ewf-h5
                YYYY,           // current model step: Year
                MM,             // current model step: month
                DD,             // current model step: day
                HH,             // current model step: hour
                MIN,            // current model step: min
                SEC);           // current model step: sec
        }
    }

    // Loop over all requests
    for (int reqi=0;reqi<num_ewfh5_requests;reqi++){

        // Number of timestamps
        num_chems = (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_data)[reqi].size();
        num_timeStamps = (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_time)[reqi].size();

        /* ########################################
        // Loop over timeStamps
        // to find the row at (or above) the current simTime timeStamp
        ######################################## */

        for (unsigned long long tStamp=0;tStamp<num_timeStamps;tStamp++){

            // Get timestamp tStamp
            h5EWF_time =  (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_time)[reqi][tStamp];
            
            // if at next timestep
            // this will then get upper and lower timesteps
            if (h5EWF_time > simTime){
                
                // Loop over all chemical species
                for (unsigned long long chemi=0;chemi<num_chems;chemi++){
                    
                    // Get h5-ewf for chemical chemi at tStamp-1 timestep
                    // if at tStamp==0, then need to get the next step
                    if(tStamp!=0){
                        h5Conc_chemi_before = (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_data)[reqi][chemi][tStamp-1];
                    }else{
                        h5Conc_chemi_before = (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_data)[reqi][chemi][tStamp];
                    }

                    // ########################################
                    // Interpolation options

                    // Option: "STEP"
                    if((OpenWQ_wqconfig.h5EWF_interpMethod).compare("STEP")==0){
                        h5Conc_chemi_interp = h5Conc_chemi_before;}

                    // Option: "Linear"
                    /*
                    if((OpenWQ_wqconfig.h5EWF_interpMethod).compare("LINEAR")){
                        
                        // Get h5-ewf for chemical chemi at tStamp+1 timestep
                        h5Conc_chemi_next = (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_data)[reqi][chemi][tStamp];
                        h5Conc_chemi_interp = (h5Conc_chemi_before % h5Conc_chemi_next)

                    }
                    

                // Update h5Conc_chemi_interp for all elements
                    Update_EWFconc_h5(          <==== this function still doesnt exist
                        OpenWQ_vars,
                        OpenWQ_wqconfig,
                        OpenWQ_hostModelconfig,
                        OpenWQ_output,
                        ewf_id, <====================== currently this is not saved in ExtFlux_FORC_HDF5vec_data => need to extend the stucture to another outter vector to include multiple EWFs
                        chemi
                        h5Conc_chemi_interp); 

                        */

                }

                // if founded step and updated results
                // then break loop
                break;
                
            }

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
        OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);
                        
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
        OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);

    }
}

/* #################################################
 // Apply Sink
 ################################################# */
void OpenWQ_extwatflux_ss::Update_EWFconc_jsonAscii(
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
        OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);

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
        OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);

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

        OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);

        validEntryFlag = false;
    }

    return validEntryFlag;

}

/* #################################################
// At timestep 1, 
// remove requested loads that are prior to the simulation start datetime
// only do this for rows that don't have any "all" elements
#################################################*/
// JSON and ASCII entries
void OpenWQ_extwatflux_ss::RemoveLoadBeforeSimStart_jsonAscii(
    OpenWQ_units& OpenWQ_units,
    std::unique_ptr<arma::Mat<double>>& array_FORC,
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

// Same as above but for EWF H5 entries
void OpenWQ_extwatflux_ss::RemoveLoadBeforeSimStart_h5(
    OpenWQ_units& OpenWQ_units,
    std::unique_ptr<std::vector<std::vector<std::vector<arma::Mat<double>>>>>& FORC_vec_data, // H5 interface data
    std::unique_ptr<std::vector<std::vector<time_t>>>& FORC_vec_time_t,          // H5 interface timestamps
    const int reqi,
    const int YYYY,         // current model step: Year
    const int MM,           // current model step: month
    const int DD,           // current model step: day
    const int HH,           // current model step: hour
    const int MIN,          // current model step: min
    const int SEC){         // current model step: sec

    // Local variables
    time_t simTime;                         // current simulation time in time_t
    time_t h5EWF_time;                      // iteractive time extraction from FORC_vec_time_t
    long long num_timeStamps;               // number of timesteps in h5 stucture
    long long num_chems;                    // number of timesteps in h5 stucture
    unsigned long long n_rows2remove;       // number of rows to remove
    std::vector<int> timStampsIndex2Remove; // List of rows indexes to remove  
    long long ri2remove;                    // index of row to remove   

    // Convert sim time to time_t
    simTime = OpenWQ_units.convertTime_ints2time_t(YYYY, MM, DD, HH, MIN, SEC);

    // Number of timestamps
    num_chems = (*FORC_vec_data)[reqi].size();
    num_timeStamps = (*FORC_vec_time_t)[reqi].size();

    /* ########################################
    // Loop over timeStamps
    // to find the rows with timestamps before simTime
    ######################################## */

    for (unsigned long long tStamp=0;tStamp<num_timeStamps;tStamp++){
        // Get timestamp tStamp
        h5EWF_time = (*FORC_vec_time_t)[reqi][tStamp];
        // Add to rows2Remove if the h5-time is before simTime
        if (h5EWF_time < simTime){
            timStampsIndex2Remove.push_back(tStamp);}
    }

     /* ########################################
    // Loop over rows2Remove
    // to remove the rows corresponding to timestamps
    // occuring before simulation start
    ######################################## */

    // Number of rows to remove
    n_rows2remove = timStampsIndex2Remove.size();

    // Loop over number of to-remove indexes (timestamps)
    for (unsigned long long tstep=0;tstep<n_rows2remove;tstep++){

        // Index of row to remove
        ri2remove = timStampsIndex2Remove[tstep];
        // Remove row from FORC_vec_data and 
        (*FORC_vec_time_t)[reqi].erase((*FORC_vec_time_t)[reqi].begin()+(ri2remove-1));

        // Loop over all chemical species
        for (unsigned long long chemi=0;chemi<num_chems;chemi++){

            (*FORC_vec_data)[reqi][chemi].erase((*FORC_vec_data)[reqi][chemi].begin()+(ri2remove-1));

        }
    }
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

// Add new row to SinkSource_FORC or ExtFlux_FORC_jsonAscii
void OpenWQ_extwatflux_ss::AppendRow_SS_EWF_FORC_jsonAscii(
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    std::string inputType,
    arma::vec row_data_col){

    // Local variables            
    arma::Mat<double> row_data_row;                     // new row data (initially as col data)
    int int_n_elem;

    // Transpose vector for adding to SinkSource_FORC as a new row
    row_data_row = row_data_col.t();

    // Get index of last element
    if (inputType.compare("ss")==0)     int_n_elem = (*OpenWQ_wqconfig.SinkSource_FORC).n_rows;
    if (inputType.compare("ewf")==0)    int_n_elem = (*OpenWQ_wqconfig.ExtFlux_FORC_jsonAscii).n_rows;
    
    // Add new row_data_row to SinkSource_FORC
    if (inputType.compare("ss")==0){ 
        (*OpenWQ_wqconfig.SinkSource_FORC).insert_rows(
            std::max(int_n_elem-1,0),
            row_data_row);}
    else if (inputType.compare("ewf")==0){
        (*OpenWQ_wqconfig.ExtFlux_FORC_jsonAscii).insert_rows(
            std::max(int_n_elem-1,0),
            row_data_row);}
   
}

// Add new row to SinkSource_FORC or ExtFlux_FORC_jsonAscii
void OpenWQ_extwatflux_ss::AppendRow_SS_EWF_FORC_h5(
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    int h5EWF_request_index,        // get request index
    int chemi,                      // chem index
    bool flg_newJSON_h5Request,     // new json-h5-ewf request
    bool flag_newChem,              // flag for new timestep, push back new vector row [i]
    bool flag_newTimeStamp,         // flag for new chem, push_back new vector row [i][j]
    time_t timestamp_time_t,        // timestamp in time_t
    arma::vec row_data_col){

    // Local variables        
    arma::Mat<double> row_data_row;             // new row data (initially as col data)
    int int_n_elem;                             // number of elements/timesteps
    
     // Push_back/Create vector<vector<arma>> for every new request (ewf-h5) 
    if(flg_newJSON_h5Request==true){
        // Time
        std::vector<time_t> newEntryArma_time; 
        (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_time).push_back(newEntryArma_time);
        // Data
        std::vector<std::vector<arma::Mat<double>>> newEntryArma_data; 
        (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_data).push_back(newEntryArma_data);
    }

    // Push_back/Create vector<arma> for every new chem 
    if(flag_newChem==true){
        std::vector<arma::Mat<double>> newChemArma; // create vector<arma> for every new chem
        (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_data)[h5EWF_request_index].push_back(newChemArma);}


    // Transpose vector for adding to SinkSource_FORC as a new row
    row_data_row = row_data_col.t();

    // Append row
    int_n_elem = (*OpenWQ_wqconfig.ExtFlux_FORC_data_tStep).n_rows;
    (*OpenWQ_wqconfig.ExtFlux_FORC_data_tStep).insert_rows(
        std::max(int_n_elem-1,0),
        row_data_row);

    // if new timestep, add to vector and reset ExtFlux_FORC_data_tStep
    if (flag_newTimeStamp==true){

        // Add new timestamp to ExtFlux_FORC_HDF5vec_time
        // But only needed on first chemi pass
        if (chemi==0){
        (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_time)[h5EWF_request_index].push_back(
            timestamp_time_t);}
    
        // add timestep data to vector ExtFlux_FORC_HDF5vec_data[chemi]
        (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_data)[h5EWF_request_index][chemi].push_back(
            *OpenWQ_wqconfig.ExtFlux_FORC_data_tStep);

        // Reset ExtFlux_FORC_data_tStep
        (*OpenWQ_wqconfig.ExtFlux_FORC_data_tStep).reset();

    }

}

// Extract H5 row to row_data_col
arma::vec OpenWQ_extwatflux_ss::ConvertH5row2ArmaVec(
    double ewf_id,
    int x_externModel, 
    int y_externModel, 
    int z_externModel,
    double conc_h5_rowi,
    int chemi){

    // Local variables
    arma::vec row_data_col;              // new row data (initially as col data)

    // Generate the arma::vec row_data_col
    row_data_col = {
        (double) chemi,
        (double) ewf_id,
        (double) x_externModel - 1,
        (double) y_externModel - 1,
        (double) z_externModel - 1,
        conc_h5_rowi,
        };                  // in the case of and "all" element (YYYY, MM, DD, HH, MIN, SEC)
                            // it starts with 0 (zero), meaning that has not been used

    return row_data_col;
}