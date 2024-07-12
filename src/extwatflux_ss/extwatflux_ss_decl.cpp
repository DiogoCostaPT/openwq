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

#include "OpenWQ_extwatflux_ss.hpp"


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
    std::string inputType) {                             // flag for SS or EWF
    
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
    unsigned int num_rowdata = 0;               // number of rows of data in JSON (YYYY, MM, DD, HH,...)
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
    double loadScheme_id = 9999;                // Load scheme id number: (1) discrete or (2) continuous
    std::string contDt_str;                     // time units of continuous load


    // Get element list (compartment for SS, and External fluxes for EWF)
    if (inputType.compare("ss")==0)     elm_list = OpenWQ_hostModelconfig.get_HydroComp_names();
    if (inputType.compare("ewf")==0)    elm_list = OpenWQ_hostModelconfig.get_HydroExtFlux_names();

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
        errorMsgIdentifier,
        true);
            
    // Type (sink or source) (only used in SS)
    if (inputType.compare("ss")==0) {
        errorMsgIdentifier = inputType + " json block";
        Type = OpenWQ_utils.RequestJsonKeyVal_str(
            OpenWQ_wqconfig, OpenWQ_output,
            EWF_SS_json_sub, "TYPE",
            errorMsgIdentifier,
            true);
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
        errorMsgIdentifier,
        true);

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
            errorMsgIdentifier,
            true);

        try{

            // Error msg identifier in case json key not found
            errorMsgIdentifier = inputType + " json block 'DATA' with DataFormat=" + DataFormat;

            // file path
            ascii_FilePath = OpenWQ_utils.RequestJsonKeyVal_str(
                OpenWQ_wqconfig, OpenWQ_output,
                EWF_SS_json_sub["DATA"], "FILEPATH",
                errorMsgIdentifier,
                true);

            // delimiter
            ascii_delimiter = OpenWQ_utils.RequestJsonKeyVal_str(
                OpenWQ_wqconfig, OpenWQ_output,
                EWF_SS_json_sub["DATA"], "DELIMITER",
                errorMsgIdentifier,
                true);

            // number of header rows
            ascii_numHeaderRows = OpenWQ_utils.RequestJsonKeyVal_int(
                OpenWQ_wqconfig, OpenWQ_output,
                EWF_SS_json_sub["DATA"], "NUMBER_OF_HEADER_ROWS",
                errorMsgIdentifier,
                true);            

            // position of key header
            ascii_headerKeyRow = OpenWQ_utils.RequestJsonKeyVal_int(
                OpenWQ_wqconfig, OpenWQ_output,
                EWF_SS_json_sub["DATA"], "HEADER_KEY_ROW",
                errorMsgIdentifier,
            true);  

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
            errorMsgIdentifier,
            true).size();
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
            errorMsgIdentifier,
            true);

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
        // Second
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
            loadScheme_id,  // only for SS: load scheme (0) not applicable, (1) discrete or (2) continuous
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
    int x_externModel, y_externModel, z_externModel;        // iteractive x,y,z info from h5 files
    int x_interface_h5, y_interface_h5, z_interface_h5;     // iteractive x,y,z from interface requested in h5 ewf files
    int nx_interface_h5, ny_interface_h5, nz_interface_h5;  // nx,ny,nz of external compartment from h5 ewf file
    int ewfName_nx, ewfName_ny, ewfName_nz;                 // nx, ny and nz of EWF associated
    std::string ss_cmp_recipient_name;                      // name of EWF recipient
    json interaction_interface_json;                        // json substructure for interface info
    int index_i;
    bool validEntryFlag;                    // flag for valid entries
    bool foundTimeStmps;                    // flag to record (un)success in finding timestamps
    bool h5_entry_found;                    // flag for successful finding of ewf h5 file
    std::string errorMsgIdentifier;         // Start/head of error message of json key not found
    std::vector<int> valid_interfaceH5rows; // vector with indexes of relevant h5 rows that contain interface data
    int rowi_val;                           // iteractive row number from valid_interfaceH5rows 
    int point_print_n;                      // iterative trackking of "." console prints (each timeStamp) for asthetics
    bool flag_newJSON_h5Request = true;      // flag for new json block for ewf-h5
    bool flag_newChem = true;               // flag for new chem from json ewf-h5 clock
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
        errorMsgIdentifier,
        true);
    // h5 ic units
    ewf_h5_units = OpenWQ_utils.RequestJsonKeyVal_str(
        OpenWQ_wqconfig, OpenWQ_output,
        EWF_SS_json_sub, "UNITS",
        errorMsgIdentifier,
        true);
    // get external compartment name (needed for both ss and ewf)
    external_compartName = OpenWQ_utils.RequestJsonKeyVal_str(
        OpenWQ_wqconfig, OpenWQ_output,
        EWF_SS_json_sub, "EXTERNAL_COMPARTMENT_NAME",
        errorMsgIdentifier,
        true);
    // Get interface between openwq models
    interaction_interface_json = OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        EWF_SS_json_sub, "INTERACTION_INTERFACE",
        errorMsgIdentifier,
        true);
    // Get external flux name
    external_waterFluxName = OpenWQ_utils.RequestJsonKeyVal_str(
        OpenWQ_wqconfig, OpenWQ_output,
        EWF_SS_json_sub, "EXTERNAL_INPUTFLUX_NAME",
        errorMsgIdentifier,
        true);
    // Get interpolation method
    OpenWQ_wqconfig.set_h5EWF_interpMethod(
                OpenWQ_utils.RequestJsonKeyVal_str(
                    OpenWQ_wqconfig, OpenWQ_output,
                    EWF_SS_json_sub, "INTERPOLATION",
                    errorMsgIdentifier,
                    true)) ;

    // ################################
    // Some pre-processing
    // ################################

    // replace "/" by "|" is needed because "/" is not compatible with directory full paths
    ewf_h5_units_file = ewf_h5_units;
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

    // Get corresponding id
    external_waterFluxName_id = 
        (double)OpenWQ_utils.FindStrIndexInVectStr(
            OpenWQ_hostModelconfig.get_HydroExtFlux_names(),
            external_waterFluxName);

    // If external compartment not found in internal list of EWF, then
    // throw warning msg and skip entry
    // Otherwise save it in (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_ewfCompID)
    if(external_waterFluxName_id==-1.0f){
        msg_string = 
            "<OpenWQ> WARNNING SS json key EXTERNAL_INPUTFLUX_NAME= "
            + external_waterFluxName
            + " not valid for this host-model coupling (entry ignored).";
        OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);
        return;
    }else{
        (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_ewfCompID).push_back(external_waterFluxName_id);
    }

    // Get num of interface elements
    ewfName_nx = OpenWQ_hostModelconfig.get_HydroExtFlux_num_cells_x_at(external_waterFluxName_id);
    ewfName_ny = OpenWQ_hostModelconfig.get_HydroExtFlux_num_cells_y_at(external_waterFluxName_id);
    ewfName_nz = OpenWQ_hostModelconfig.get_HydroExtFlux_num_cells_z_at(external_waterFluxName_id);
    
    // Generate arma::cube of compartment ewfi size
    // And reset ExtFlux_FORC_data_tStep for dimensions of 
    // ewf of index external_waterFluxName_id
    arma::Cube<double> ewfi_domain_xyz(ewfName_nx, ewfName_ny, ewfName_nz);
    (*OpenWQ_wqconfig.ExtFlux_FORC_data_tStep) = ewfi_domain_xyz;

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

        // Set new chem flag true
        flag_newChem = true;

        // ############################
        // Get and process interface H5 data 

        // Get chem name
        chemname = (OpenWQ_wqconfig.BGC_general_chem_species_list)[chemi];

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
            interaction_interface_json, 0, x_interface_h5, domain_EWF_h5(0,0));
        if (!validEntryFlag) return;
        // y
        index_i = 1;
        msg_string = "EWF Invalid 'INTERACTION_INTERFACE' array element " 
                    + std::to_string(index_i) + "for HDF5. It only accepts integers or 'all'";
        validEntryFlag = OpenWQ_utils.Convert2NegativeOneIfAll_inputInt(
            OpenWQ_wqconfig, OpenWQ_output, msg_string,
            interaction_interface_json, 1, y_interface_h5, domain_EWF_h5(0,1));
        if (!validEntryFlag) return;
        // z
        index_i = 2;
        msg_string = "EWF Invalid 'INTERACTION_INTERFACE' array element " 
                    + std::to_string(index_i) + "for HDF5. It only accepts integers or 'all'";
        validEntryFlag = OpenWQ_utils.Convert2NegativeOneIfAll_inputInt(
            OpenWQ_wqconfig, OpenWQ_output, msg_string,
            interaction_interface_json, 2, z_interface_h5, domain_EWF_h5(0,2));
        if (!validEntryFlag) return;

        // Get the domain of interface external compartment
        if(x_interface_h5==-1) nx_interface_h5 = domain_EWF_h5(0,0);
        else nx_interface_h5 = x_interface_h5;
        if(y_interface_h5==-1) ny_interface_h5 = domain_EWF_h5(0,1);
        else ny_interface_h5 = y_interface_h5;
        if(z_interface_h5==-1) nz_interface_h5 = domain_EWF_h5(0,2);
        else nz_interface_h5 = z_interface_h5;

        // ################################
        // Check if requested interface elements match
        // the dimensions of 
        // ################################

        if(nx_interface_h5!=ewfName_nx 
          || ny_interface_h5!=ewfName_ny 
          || nz_interface_h5!=ewfName_nz){

             msg_string = 
                "<OpenWQ> WARNNING " + inputType
                + " h5 file requested=" + ewf_filenamePath
                + " has been found, but the internal dimensions of the interface flux elements ("
                + std::to_string(ewfName_nx) + "," + std::to_string(ewfName_ny) + ","
                + std::to_string(ewfName_nz) + ") do not match those requested in 'INTERACTION_INTERFACE': ("
                + std::to_string(nx_interface_h5) + "," + std::to_string(ny_interface_h5) + ","
                + std::to_string(nz_interface_h5)
                + "). Make sure to load an EWF h5 file that has all the"
                " interface elements and the 'INTERACTION_INTERFACE' specifies all (and only) the interface elements."
                " This element has been defaulted to zero (entry skipped).";
            OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);

            // Skip entry
            continue;

        }

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

        // Loop over domain 
        // and check if the h5 file contains the interface elements
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
        point_print_n = 0;

        for (long unsigned int tSamp=0;tSamp<tSamp_valid.size();tSamp++){

            // Get the corresponding data
            dataEWF_h5
                .load(arma::hdf5_name(
                    ewf_filenamePath,              // file name
                    tSamp_valid[tSamp]));          // options

            // Get timestamp sting into time_t
            tSamp_valid_i_time_t = OpenWQ_units.convertTime_str2time_t(
                OpenWQ_wqconfig,
                tSamp_valid[tSamp]);

            // Reset ExtFlux_FORC_data_tStep to save next timestep
            (*OpenWQ_wqconfig.ExtFlux_FORC_data_tStep).zeros();

            // Loop over HDF5 row data referring to the interface
            // Update ExtFlux_FORC_data_tStep with new time step ewf concentrations
            for (int rowi=0;rowi<(int)valid_interfaceH5rows.size();rowi++){

                // Get valid row
                rowi_val = valid_interfaceH5rows[rowi];

                // Get element x,y,z indexes of external model
                // Using convention of external model
                // Needs to be converted into local openwq implementation
                x_externModel = xyzEWF_h5(rowi_val, 0) - 1;
                y_externModel = xyzEWF_h5(rowi_val, 1) - 1;
                z_externModel = xyzEWF_h5(rowi_val, 2) - 1;

                // Get concentration
                conc_h5_rowi = dataEWF_h5(rowi_val);

                // Convert conc to local units
                OpenWQ_units.Convert_Units(
                    conc_h5_rowi,               // value passed by reference so that it can be changed
                    unit_multiplers);           // units

                // Update ExtFlux_FORC_data_tStep
                (*OpenWQ_wqconfig.ExtFlux_FORC_data_tStep)
                    (x_externModel,y_externModel,z_externModel) 
                        = conc_h5_rowi;

            }

            AppendCube_SS_EWF_FORC_h5(
                OpenWQ_wqconfig,
                h5EWF_request_index,
                chemi,
                flag_newChem,
                flag_newJSON_h5Request,
                tSamp_valid_i_time_t);

            // Reset flags
            flag_newJSON_h5Request = false;
            flag_newChem = false;

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
        
        elemVal = OpenWQ_wqconfig.get_allSS_flag();

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

// Add new row to SinkSource_FORC or ExtFlux_FORC_jsonAscii
void OpenWQ_extwatflux_ss::AppendRow_SS_EWF_FORC_jsonAscii(
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    std::string inputType,
    arma::vec row_data_col){

    // Local variables            
    arma::Mat<double> row_data_row;                     // new row data (initially as col data)
    int int_n_elem = 0;

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
void OpenWQ_extwatflux_ss::AppendCube_SS_EWF_FORC_h5(
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    int h5EWF_request_index,        // get request index
    int chemi,                      // chem index
    bool flag_newChem,              // flag for new timestep, push back new vector row [i]
    bool flag_newJSON_h5Request,     // new json-h5-ewf request
    time_t timestamp_time_t){        // timestamp in time_t

    
    // Push_back/Create vector<vector<arma>> for every new request (ewf-h5) 
    if(flag_newJSON_h5Request==true){
        // Time
        std::vector<time_t> newEntryArma_time; 
        (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_time).push_back(newEntryArma_time);
        // Data
        std::vector<std::vector<arma::Cube<double>>> newEntryArma_data; 
        (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_data).push_back(newEntryArma_data);
    }

    // Push_back/Create vector<arma> for every new chem 
    if(flag_newChem==true){
        std::vector<arma::Cube<double>> newChemArma; // create vector<arma> for every new chem
        (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_data)[h5EWF_request_index].push_back(newChemArma);

        
    }

    // Add new timestamp to ExtFlux_FORC_HDF5vec_time
    // But only needed on first chemi pass
    if(chemi==0){
        (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_time)[h5EWF_request_index].push_back(
            timestamp_time_t);
    }

    // add timestep data to vector ExtFlux_FORC_HDF5vec_data[chemi]
    (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_data)[h5EWF_request_index][chemi].push_back(
        *OpenWQ_wqconfig.ExtFlux_FORC_data_tStep);

    // Reset ExtFlux_FORC_data_tStep
    (*OpenWQ_wqconfig.ExtFlux_FORC_data_tStep).zeros();


}