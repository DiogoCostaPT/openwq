
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
// Transform units of input data to OpenWQ native units
// Mass: kg
// Concentration: mg/l or kg/m3
// input argument can be double, arma::mat or arma::cube
################################################# */
void OpenWQ_units::Convert_Units(
    double& input_value,                      // input value (passed by reference)
    std::vector<double> unit_multiplers){    // multiplers (numerator and denominator)

    // Convert input_value 
    input_value = input_value 
        * unit_multiplers[0]    // numerator multiplier
        / unit_multiplers[1];   // denominator multiplier

}


/* #################################################
// Parse units of input data and calculate data multiplier
################################################# */
bool OpenWQ_units::Calc_Unit_Multipliers(
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_output& OpenWQ_output,
    std::vector<double>& unit_multiplers,   // multiplers (numerator and denominator)
    std::string input_unit,                 // input units
    std::vector<std::string>& units,        // units (numerator and denominator)
    bool direction_2native){                // direction of the conversion: 
                                            // to native (true) or 
                                            // from native to desired output units (false)

    // Local Variales
    //std::vector<std::string> units;         // units (numerator and denominator)
    std::size_t loc_divider = 0;            // location of the "/" symbol in input units
    unsigned int m;                         // interactor
    bool mass_unit_flag;                    // flag to note if unit of mass
    bool volume_unit_flag;                  // flag to note if unit of volume
    bool time_unit_flag;                    // flag to note if unit of time
    bool empty_flag;                        // flag to note if uempty (applied to denominator)
    std::string msg_string;                 // error/warning message string

    // Clear unit_multiplers in case it is called multiple times from same routine
    unit_multiplers.clear();
    
    /* ########################################
    // Check the input units (numerator and denominator)
    ######################################## */
    
    // If only numerator
    if (input_unit.find("/") == std::string::npos) {
        units.push_back(input_unit);
        units.push_back("EMPTY");
    }
    else{// if denominator exists too
        loc_divider = input_unit.find("/");
        units.push_back(input_unit.substr (0,loc_divider));
        units.push_back(input_unit.substr (loc_divider+1,input_unit.size()));
    }

    // Create unit_multiplers vector of ones
    unit_multiplers.push_back(1.0f);    // numerator
    unit_multiplers.push_back(1.0f);    // denominator
    

    /* ########################################
    // Identify units (Mass, Vol, time) and determine unit multiplers 
    // for native units (numerator and denominator)
    // Mass = kg; Concentration = mg/l = g/m3; Time = seconds
    ######################################## */

    try{
        // loop of size = 2 
        // (numerator and denominator)
        for (m=0;m<2;m++){

            // Reset flags
            mass_unit_flag = false;                  // flag to note if unit of mass
            volume_unit_flag = false;                // flag to note if unit of volume
            time_unit_flag = false;                  // flag to note if unit of time
            empty_flag = false;                      // flag to note if empty (applied to denominator)

            // ########################################
            // Check if a MASS unit
            // if YES, get multipliers for natirve units (g)

            mass_unit_flag = Check_Mass_Units(
                units[m],
                unit_multiplers[m]);

            if (mass_unit_flag == true)
                continue;
            
            // ########################################
            // Check if a Volume unit
            // if YES, get multipliers for native units (m3)

            volume_unit_flag = Check_Volume_Units(
                units[m],
                unit_multiplers[m]);

            if (volume_unit_flag == true)
                continue;

            // ########################################
            // Check if a TIME unit
            // if YES, get multipliers for native units (sec)
            
            time_unit_flag = Check_Time_Units(
                units[m],
                unit_multiplers[m]);

            if (time_unit_flag == true)
                continue;

            // ########################################
            // If EMPTY pr =1 (applies to denominators)
            // set to multipler to 1
            // this is redundant, but just to make sure this value doesn't get wild
            if (units[m].compare("EMPTY") == 0 ||
                units[m].compare("1") == 0 ){
                empty_flag = true;
                unit_multiplers[m] = 1.0f;
            }

            // ########################################
            // Unit not found 
            if (mass_unit_flag == false
                && volume_unit_flag == false
                && time_unit_flag == false
                && empty_flag == false){
               
                // Create Message
                msg_string = 
                    "<OpenWQ> ERROR: Input unit: unkown (" 
                    + units[m] + ")";

                // Print it (Console and/or Log file)
                OpenWQ_output.ConsoleLog(
                    OpenWQ_wqconfig,    // for Log file name
                    msg_string,         // message
                    true,               // print in console
                    true);              // print in log file

                exit (EXIT_FAILURE);
            }
        }

    }catch(...){ 
        
        // Create Message (error)
        msg_string = 
            "<OpenWQ> ERROR: Input unit: problem with input data (" 
            + units[m] + ")" ;

        // Print it (Console and/or Log file)
        OpenWQ_output.ConsoleLog(
            OpenWQ_wqconfig,    // for Log file name
            msg_string,         // message
            true,               // print in console
            true);              // print in log file

        // Fatal failure (Abort) 
        exit (EXIT_FAILURE);
    }

    // if direction_2native = true (convert to native), then order is okay
    // if direction_2native = false (convert from native), then reverse order
    if (direction_2native == false){

        std::reverse(unit_multiplers.begin(),unit_multiplers.end());

    }

    // Return true if denominator is a unit of volume
    // this will be the flag for the denominator because its the last 
    // iteraction (m=2)
    return volume_unit_flag;

}

/* #################################################
// Check if Mass Units
// Get the multiplers if YES
################################################# */
bool OpenWQ_units::Check_Mass_Units(
    std::string unit_i,         // unit of numerator or denominator
    double& unit_multipler_i){  // multiplier of numerator or denominator

    // Local variables
    bool mass_unit_flag = true;

    // Mass units supported

    if (unit_i.compare("GT") == 0){
        // gigatonne (Gt)
        unit_multipler_i = 1000000000000000.0f;
    }else if (unit_i.compare("MT") == 0){
        // megatonne (Mt)
        unit_multipler_i = 1000000000000.0f;
    }else if (unit_i.compare("T") == 0){
        // tonne (t)
        unit_multipler_i = 1000000.0f;
    }else if (unit_i.compare("KG") == 0){
        // kilogram (kg)
        unit_multipler_i =1000.0f;
    }else if (unit_i.compare("G") == 0){
        // gram (g)
        unit_multipler_i = 1.0f;
    }else if (unit_i.compare("MG") == 0){
        // miligram (mg)
        unit_multipler_i = 0.001f;
    }else if (unit_i.compare("UG") == 0){
        // micro gram (ug)
        unit_multipler_i = 0.000001f;
    }else if (unit_i.compare("NG") == 0){
        // nano gram
        unit_multipler_i = 0.000000001f;
    }else if (unit_i.compare("PG") == 0){
        // pico gram (pg)
        unit_multipler_i = 0.000000000001f;
    }else if (unit_i.compare("TON_US") == 0){
        // tonne US
        unit_multipler_i = 0.907f * 1000000.0f;
    }else if (unit_i.compare("TON_UK") == 0){
        // tonne UK
        unit_multipler_i = 1.016f * 1000000.0f;
    }else if (unit_i.compare("LB") == 0){
        // pound (lb)
        unit_multipler_i = 453.59f;
    }else if (unit_i.compare("OZ") == 0){
        // ounce (oz)
        unit_multipler_i = 28.35f;
    }else{
        mass_unit_flag = false;
    }

    // Return true if unts were found
    return mass_unit_flag;

}

/* #################################################
// Check if Volume Units
// Get the multiplers if YES
################################################# */
bool OpenWQ_units::Check_Volume_Units(
    std::string unit_i,         // unit of numerator or denominator
    double& unit_multipler_i){  // multiplier of numerator or denominator

    // Local variables
    bool volume_unit_flag = true;

    // Volume units supported

    if (unit_i.compare("KM3") == 0){ 
        // cubic kilometer 
        unit_multipler_i = 1000000000.0f;
    }else if (unit_i.compare("M3") == 0){ 
        // cubic meter 
        unit_multipler_i = 1.0f;
    }else if (unit_i.compare("CM3") == 0){ 
        // cubic centimeter
        unit_multipler_i = 1.0E-6;
    }else if (unit_i.compare("MM3") == 0){   
        // cubic millimeter 
        unit_multipler_i = 1.0E-9;
    }else if (unit_i.compare("L") == 0){
        // liter 
        unit_multipler_i = 0.001f;
    }else if (unit_i.compare("ML") == 0){
        // milliliter 
        unit_multipler_i = 1.0E-6;
    }else if (unit_i.compare("GAL_US") == 0){
        // gallon (US)
        unit_multipler_i = 0.0037854118f;
    }else if (unit_i.compare("QT_US") == 0){
        // quart (US)
        unit_multipler_i = 0.0009463529f;
    }else if (unit_i.compare("MI3") == 0){
        // cubic mile
        unit_multipler_i = 4168181825.4406f;
    }else if (unit_i.compare("YD3") == 0){
        // cubic yard 
        unit_multipler_i = 0.764554858f;
    }else if (unit_i.compare("FT3") == 0){
        // cubic foot
        unit_multipler_i = 0.0283168466f;
    }else if (unit_i.compare("IN3") == 0){
        // cubic inch
        unit_multipler_i = 1.63871E-5;
    }else if (unit_i.compare("DM3") == 0){
        // cubic decimeter 
        unit_multipler_i = 0.001f;
    }else if (unit_i.compare("EL") == 0){
        // exaliter
        unit_multipler_i  = 1.0E+15;
    }else if (unit_i.compare("PL") == 0){
        // petaliter 
        unit_multipler_i  = 1000000000000.0f;
    }else if (unit_i.compare("TL") == 0){
        // teraliter
        unit_multipler_i = 1000000000.0f;
    }else if (unit_i.compare("GL") == 0){
        // gigaliter
        unit_multipler_i = 1000000.0f;
    }else if (unit_i.compare("MEGL") == 0){
        // megaliter
        unit_multipler_i  = 1000.0f;
    }else if (unit_i.compare("KL") == 0){
        // kiloliter 
        unit_multipler_i = 1.0f;
    }else if (unit_i.compare("HL") == 0){
        // hectoliter 
        unit_multipler_i = 0.1f;
    }else if (unit_i.compare("AL") == 0){
        // dekaliter 
        unit_multipler_i = 0.01f;
    }else if (unit_i.compare("DL") == 0){
        // deciliter 
        unit_multipler_i = 0.0001f;
    }else if (unit_i.compare("CL") == 0){
        // centiliter
        unit_multipler_i = 1.0E-5;
    }else if (unit_i.compare("UL") == 0){
        // microliter
        unit_multipler_i = 1.0E-9;
    }else if (unit_i.compare("NL") == 0){
        // nanoliter
        unit_multipler_i = 1.0E-12;
    }else if (unit_i.compare("PL") == 0){
        // picoliter 
        unit_multipler_i = 1.0E-15;
    }else if (unit_i.compare("BBL_US") == 0){
        // barrel (US)
        unit_multipler_i = 0.1192404712f;
    }else if (unit_i.compare("BBL") == 0){
        // barrel (UK) 
        unit_multipler_i = 0.16365924f;
    }else if (unit_i.compare("GAL_UK") == 0){
        // gallon (UK)
        unit_multipler_i = 0.00454609f;
    }else if (unit_i.compare("QT_UK") == 0){
        // quart (UK) 
        unit_multipler_i = 0.0011365225f;
    }else if (unit_i.compare("PT_UK") == 0){
        // pint (UK) 
        unit_multipler_i = 0.0005682613f;
    }else if (unit_i.compare("FL_OZ_US") == 0){
        // fluid ounce (US) 
        unit_multipler_i = 2.95735E-5;
    }else if (unit_i.compare("FL_OZ_UK") == 0){
        // fluid ounce (UK) 
        unit_multipler_i = 2.84131E-5;
    }else if (unit_i.compare("AC_FT") == 0){
        // acre-foot [ac*ft] 
        unit_multipler_i = 1233.4818375475f;
    }else if (unit_i.compare("AC_FT_US") == 0){
        // acre-foot (US survey)
        unit_multipler_i = 1233.4892384682f;
    }else if (unit_i.compare("AC_IN") == 0){
        // acre-inch [ac*in] 
        unit_multipler_i = 102.790153129f;
    }else{
        volume_unit_flag = false;
    }

    // Return true if unts were found
    return volume_unit_flag;

}

/* #################################################
// Check if Time Units
// Get the multiplers if YES
################################################# */
bool OpenWQ_units::Check_Time_Units(
    std::string unit_i,         // unit of numerator or denominator
    double& unit_multipler_i){  // multiplier of numerator or denominator

    // Local variables
    bool time_unit_flag = true;

    // Volume units supported

    if (unit_i.compare("SEC") == 0){
        unit_multipler_i = 1.0f;
    }else if (unit_i.compare("MIN") == 0){
        unit_multipler_i = 60;
    }else if (unit_i.compare("HOUR") == 0){
        unit_multipler_i = 3600.0f;
    }else if (unit_i.compare("DAY") == 0){
        unit_multipler_i = 3600.0f * 24;   
    }else if (unit_i.compare("WEEK") == 0){
        unit_multipler_i = 3600.0f * 24 * 7;
    }else if (unit_i.compare("MONTH") == 0){
        unit_multipler_i = 3600.0f * 24 * 30;
    }else if (unit_i.compare("YEAR") == 0){
        unit_multipler_i = 3600.0f * 24 * 365;
    } else{
        time_unit_flag = false;
    }

    // Return true if unts were found
    return time_unit_flag;

}

/* #################################################
// Convert discreted date info into time_t
################################################# */
time_t OpenWQ_units::convertTime_ints2time_t(
    int YYYY, 
    int MM, 
    int DD, 
    int HH, 
    int MIN,
    int SEC) {

    std::time_t sim_time;
    std::tm tm{};
    
    tm.tm_year = YYYY - 1900; // -1900 is needed to get the conversion to produce the correct output
    tm.tm_mon = MM - 1;
    tm.tm_hour = HH;
    tm.tm_mday = DD;
    tm.tm_min = MIN;
    tm.tm_sec = SEC;
    sim_time = timegm(&tm);

    return sim_time;
}

/* #################################################
// Convert date string info into time_t
################################################# */
time_t OpenWQ_units::convertTime_str2time_t(
    std::string datetime_str) {
    
    // Local variables
    std::time_t sim_time;
    struct tm tm;
    struct tm tm_month;

    // Get date
    tm.tm_year = stoi(datetime_str.substr(0,4));
    tm.tm_mday = stoi(datetime_str.substr(7,2));
    tm.tm_hour = stoi(datetime_str.substr(10,2));
    tm.tm_min = stoi(datetime_str.substr(13,2));
    tm.tm_sec = stoi(datetime_str.substr(16,2));

    // ######################
    // Processing month that is in char
    const char* month_char = datetime_str.substr(4,3).c_str();
    strptime(month_char, "%b", &tm_month);
    tm.tm_mon = tm_month.tm_mon;

    // Convert time to time_t  
    sim_time = mktime(&tm);

    return sim_time;
    
}