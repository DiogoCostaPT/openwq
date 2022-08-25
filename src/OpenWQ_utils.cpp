
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


#include "OpenWQ_utils.h"

// ########################################                                                          
// Get number of lines in file
// ########################################                                                          
unsigned int OpenWQ_utils::getNumLinesfromASCIIFile(
    std::string ascii_FilePath){ // path to file

    std::ifstream aFile (ascii_FilePath);   
    std::size_t lines_count=0;
    std::string line;

    // loop to get number of lines
    while (std::getline(aFile , line))
            ++lines_count;

    // Close file
    aFile.close();

    // return number of lines
    return lines_count;

}

// ########################################
// Split string into vector of strings based on string-delimiter
// ########################################
std::vector<std::string> OpenWQ_utils::StringSplit (
    std::string stringInput, 
    std::string delimiter) {

    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> stringVect;

    // Check if it contains \r,
    // if yes, remove it
    if (stringInput.find("\r") != std::string::npos){
        stringInput.erase(stringInput.find("\r"),2);}

    while ((pos_end = stringInput.find (delimiter, pos_start)) != std::string::npos) {
        // Get token
        token = stringInput.substr (pos_start, pos_end - pos_start);
        // Removing leading and trailling whitespaces
        token = RemoveStrLeadTrailWhiteSpaces(token);
        // update pos_start location
        pos_start = pos_end + delim_len;
        // add string to string-vector
        stringVect.push_back (token);
    }

    // Last sub-string
    token = stringInput.substr(pos_start);
    // Remove whitespaces
    token = RemoveStrLeadTrailWhiteSpaces(token);
    // Add last string
    stringVect.push_back (token);
    return stringVect;
}

// ########################################
// Find index of string in vector::string
// ########################################
unsigned int OpenWQ_utils::FindStrIndexInVectStr(
    std::vector<std::string> VectString,
    std::string Str2Find){

    // Local variables
    unsigned int strIndex; // index of the Str2Find in VectString

    std::vector<std::string>::iterator strInter = std::find(
        VectString.begin(), 
        VectString.end(), 
        Str2Find);
        
    // determine index
    strIndex = distance(VectString.begin(), strInter);
    return strIndex;
}

// ########################################
// Removing leading and trailling whitespaces
// ########################################
std::string OpenWQ_utils::RemoveStrLeadTrailWhiteSpaces(
    std::string String2RemWhiteSpace){

    // Remove Leading Spaces
    String2RemWhiteSpace.erase(
        String2RemWhiteSpace.begin(), 
        std::find_if(String2RemWhiteSpace.begin(), 
        String2RemWhiteSpace.end(), 
        std::bind1st(std::not_equal_to<char>(), 
        ' ')));

    // Remove trailling spaces
    String2RemWhiteSpace.erase(
        std::find_if(
            String2RemWhiteSpace.rbegin(), 
            String2RemWhiteSpace.rend(), 
            std::bind1st(std::not_equal_to<char>(), 
            ' ')).base(), String2RemWhiteSpace.end());

    return String2RemWhiteSpace;

}

// ########################################
// Converts string text to lower case
// ########################################

std::string OpenWQ_utils::ConvertStringToUpperCase(
    const std::string StrEntry){

    // Local variable
    std::string NewStr = StrEntry;

    // Try to transform to upper case if text
    try{
        std::transform(
        NewStr.begin(),
        NewStr.end(),
        NewStr.begin(), // Convert to lower case to avoid issues
        [](unsigned char c) { return std::toupper(c); });
        
    }catch (...){}

    return NewStr;

}