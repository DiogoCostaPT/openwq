
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

#include "OpenWQ_units.h"

/* #################################################
// Transform units of IC
################################################# */
void OpenWQ_units::Convert_IC_Units(
    double &ic_value, // IC value of chemical (passed by reference)
    std::string ic_type, // IC value type of chemical (mass or concentration)
    std::string ic_unit){ // // IC value units of chemical (e.g, kg/m3, mg/l))

    // Local Variales
    double unit_convert_k = 1.0f;
    bool type_unkown_flag = false;
    bool unit_unkown_flag = false;


    // Convert ic_type and ic_unit to lower case to guarantee match
    std::transform(ic_type.begin(), ic_type.end(), ic_type.begin(),
        [](unsigned char c){ return std::tolower(c); });
    std::transform(ic_unit.begin(), ic_unit.end(), ic_unit.begin(),
        [](unsigned char c){ return std::tolower(c); });


    // Check type of IC (concentration or mass)
    /* ########################################
    // CONCENTRATION (Goal: convert to mg/l or g/m3 => openWQ internal units)
    ######################################## */
    if(ic_type.compare("concentration") == 0){
        
        // Default concentration units = mg/l or g/m3 (openWQ internal units)
        if (ic_unit.compare("mg/l") == 0 || ic_unit.compare("g/m3") == 0){
            unit_convert_k = 1.0f;
        }else if (ic_unit.compare("ug/l") == 0){
            unit_convert_k = 0.001;
        }else{ // ERROR: ic_unit unkown
            unit_unkown_flag = true;
        }
    }
    /* ########################################
    // MASS (Goal: convert to g => openWQ internal units)
    ######################################## */
    else if(ic_type.compare("mass") == 0){
        
        // Default mass units = g (openWQ internal units)
        if (ic_unit.compare("g/m3") == 0){ 
            unit_convert_k = 1.0f;
        }else if (ic_unit.compare("kg/m3") == 0 ||
            ic_unit.compare("kg/m2") == 0){
            unit_convert_k = 1000;
        }else{ // ERROR: ic_tunit unkown
            unit_unkown_flag = true;
        }
    } 
    /* ########################################
    // ERROR: ic_type nkown
    ######################################## */
    else{
        type_unkown_flag = true;
    }

    /* ########################################
    // Error Handling
    ########################################## */
    if (type_unkown_flag || unit_unkown_flag){
        
        if (type_unkown_flag) 
            std::cout << "<OpenWQ> ERROR: Initial conditions type: unkown (" << ic_type << ")" << std::endl;
        if (unit_unkown_flag) 
            std::cout << "<OpenWQ> ERROR: Initial conditions unit: unkown (" << ic_unit << ")" << std::endl;
        exit (EXIT_FAILURE);
    }   

    /* ########################################
    // Convert units using unit_convert_k (ic_value passed by reference)
     ########################################## */
    ic_value *= unit_convert_k;

}

/* #################################################
// Transform SS (Sink & Source) Units
################################################# */
void OpenWQ_units::Convert_SS_Units(
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
            << "<OpenWQ> ERROR: Initial conditions unit: unkown (" 
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


/* #################################################
 // Convert timesteps (to host model units)
 ################################################# */
void OpenWQ_units::Convert_Time_Units(
        double &time_value,      // SS value
        std::string time_unit){  // SS unit{   

        // Local Variales
    double unit_convert_k = 1.0f;
    bool unit_unkown_flag = false;

    // Convert ss_unit to lower case to guarantee match
    std::transform(time_unit.begin(), time_unit.end(), time_unit.begin(),
    [](unsigned char c){ return std::tolower(c); });


        /* ########################################
    // Check Units
    ######################################### */
    // Default source/sink loads units = g (openWQ internal units of mass)
    if (time_unit.compare("s") == 0){
        unit_convert_k = 1.0f;
    }else if (time_unit.compare("h") == 0){
        unit_convert_k = 3600.0f;
    }else if (time_unit.compare("d") == 0){
        unit_convert_k = 86400.0f;   
    }else{ // ERROR: time_unit unkown
        unit_unkown_flag = true;
    }

    /* ########################################
    // Error Handling
    ########################################## */
    if (unit_unkown_flag){
        std::cout 
            << "<OpenWQ> ERROR: Initial conditions unit: unkown (" 
            << time_unit 
            << ")"
            << std::endl;
        exit (EXIT_FAILURE);
    }

     /* ########################################
    // Convert units using unit_convert_k (ic_value passed by reference)
     ########################################## */
    time_value *= unit_convert_k;

}