
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


#ifndef OPENWQ_UTILS_INCLUDED
#define OPENWQ_UTILS_INCLUDED

#include "OpenWQ_global.h"
#include "OpenWQ_solver.h"
#include "OpenWQ_output.h"

class OpenWQ_utils{

    public:

    // Get number of lines in file
    unsigned int getNumLinesfromASCIIFile(
        std::string ascii_FilePath // path to file
    );

    // Split string into vector of strings based on string-delimiter
    std::vector<std::string> StringSplit (
        std::string stringInput, 
        std::string delimiter);

    // Find index of string in vector::string
    int FindStrIndexInVectStr(
        std::vector<std::string> VectString,
        std::string Str2Find);

    // Removing leading and trailling whitespaces
    std::string RemoveStrLeadTrailWhiteSpaces(
        std::string String2RemWhiteSpace);

    // Convert string to upper case
    std::string ConvertStringToUpperCase(
        const std::string StrEntry);

    int getNumberOfDaysInMonthYear(
        const unsigned int YYYY_check,          // json: Year 
        const unsigned int MM_check);           // json: Month

    // Get timestamps from logFile
    void GetTimeStampsFromLogFile(
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_output& OpenWQ_output,
        std::string logFile_folderPath,
        std::string preOutputRow_substring,
        std::vector<std::string>& timeStamps_vec,
        std::string errMsg_LofFileIdentifier); 

};

#endif