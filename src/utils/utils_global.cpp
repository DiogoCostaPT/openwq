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

// For VTK support:
// This was built for hexahedrons (see line 66) for now (cubes, Rectangular cuboid, Trigonal trapezohedron, etc)
// But line 33:33 makes determining cubes of side lenght = 1 (but this can all be changed)
// based on https://lorensen.github.io/VTKExamples/site/Cxx/IO/WriteVTU/


#include "OpenWQ_utils.hpp"

/*
* JSON key null error
*/
std::string OpenWQ_utils::get_jsonKeyNull_msg_start_abort()
{
    return this->jsonKeyNull_msg_start_abort;
}
std::string OpenWQ_utils::get_jsonKeyNull_msg_end_abort()
{
    return this->jsonKeyNull_msg_end_abort;
}
std::string OpenWQ_utils::get_jsonKeyNull_msg_start_NOabort()
{
    return this->jsonKeyNull_msg_start_NOabort;
}
std::string OpenWQ_utils::get_jsonKeyNull_msg_end_NOabort()
{
    return this->jsonKeyNull_msg_end_NOabort;
}


// ########################################
// Find index of string in vector::string
// ########################################
int OpenWQ_utils::FindStrIndexInVectStr(
    std::vector<std::string> VectString,
    std::string Str2Find){

    // Local variables
    int strIndex; // index of the Str2Find in VectString

    std::vector<std::string>::iterator strInter = std::find(
        VectString.begin(), 
        VectString.end(), 
        Str2Find);
        
    // determine index
    strIndex = distance(VectString.begin(), strInter);
    
    // if not found, return -1
    if(strIndex==(int)VectString.size()) strIndex=-1;

    return strIndex;
}

// Read JSON keyVal with type: String
std::string OpenWQ_utils::RequestJsonKeyVal_str(
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_output& OpenWQ_output,
    json json_struct,
    std::string jsonKey,
    std::string msgIndetifier,
    bool abort_flag){

    // Local variables
    std::string jsonVal_str;
    std::string varType = "string";

    try{

        // Try to access json key
        jsonVal_str = json_struct[jsonKey]; 

    }catch(...){

        // Abort and through error message
        RequestJsonKeyVal_errorAbort(
            OpenWQ_wqconfig, 
            OpenWQ_output,
            jsonKey,
            msgIndetifier,
            varType,
            abort_flag);
        
    } 

    // If jsonVal found, return it
    return jsonVal_str;

}

// Abort program if JSON key not found
void OpenWQ_utils::RequestJsonKeyVal_errorAbort(
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_output& OpenWQ_output,
    std::string jsonKey,
    std::string msgIndetifier,
    std::string varType,
    bool abort_flag){

    // Local variables
    std::string jsonKeyNull_msg;
    std::string msg_start;
    std::string msg_end;

    // Get appropriate start and end message string
    // depending on if to abort or not
    if (abort_flag == true){
        msg_start = this->get_jsonKeyNull_msg_start_abort();
        msg_end = this->get_jsonKeyNull_msg_end_abort();
    }else if (abort_flag == false){
        msg_start = this->get_jsonKeyNull_msg_start_NOabort();
        msg_end = this->get_jsonKeyNull_msg_end_NOabort();
    }
    // If results is NULL, throw error message and abort 
    jsonKeyNull_msg = "####################\n"
                    + msg_start
                    + jsonKey
                    + " of type=" + varType
                    + " at " + msgIndetifier
                    + msg_end
                    + "\n####################";

    // Print in console and logFile
    OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, jsonKeyNull_msg, true,true); 
    
    // Abort program
    if (abort_flag == true){
        exit(EXIT_FAILURE);
    }

}