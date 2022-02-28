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


#include "OpenWQ_hydrolink.h"


// Constructor
ClassWQ_OpenWQ::ClassWQ_OpenWQ(int _numHru): numHRU(_numHru) {
    std::cout << "OpenWQ Class Created C++" << std::endl;
}
// Deconstructor
ClassWQ_OpenWQ::~ClassWQ_OpenWQ() {
    std::cout << "C++ deconstructor" << std::endl;
}
// Methods
int ClassWQ_OpenWQ::getNum() const {
    return numHRU;
}

time_t ClassWQ_OpenWQ::convert_time(int year, int month, int day, int hour, int minute) {
    std::time_t sim_time;
    std::tm tm{};
    tm.tm_year = year - 1900; // -1900 is needed to get the conversion to produce the correct output
    tm.tm_mon = month - 1;
    tm.tm_hour = hour;
    tm.tm_mday = day;
    tm.tm_min = minute;
    sim_time = timegm(&tm);

    return sim_time;
}

int ClassWQ_OpenWQ::decl() {
    std::cout << "C++ decl, Initalizing structures" << std::endl;
    std::cout << numHRU << std::endl;
    OpenWQ_hostModelconfig_ref = new OpenWQ_hostModelconfig(); // Initalize hostModelconfig
    OpenWQ_couplercalls_ref = new OpenWQ_couplercalls();
    OpenWQ_json_ref = new OpenWQ_json();
    OpenWQ_wqconfig_ref = new OpenWQ_wqconfig(11); // This is 11 because the OpenWQ_global.h says it should be in the class definition
    OpenWQ_units_ref = new OpenWQ_units();
    OpenWQ_readjson_ref = new OpenWQ_readjson();
    OpenWQ_initiate_ref = new OpenWQ_initiate();
    OpenWQ_watertransp_ref = new OpenWQ_watertransp();
    OpenWQ_chem_ref = new OpenWQ_chem();
    OpenWQ_sinksource_ref = new OpenWQ_sinksource();
    OpenWQ_output_ref = new OpenWQ_output();

    if (OpenWQ_hostModelconfig_ref->HydroComp.size()==0) {
        std::cout << "Host Model size = 0" << std::endl;

        OpenWQ_hostModelconfig_ref->HydroComp.push_back(OpenWQ_hostModelconfig::hydroTuple(0,"SWE",numHRU,1,1));
        std::cout << "Host Model Size is now = " << 
            OpenWQ_hostModelconfig_ref->HydroComp.size() << std::endl;

        OpenWQ_vars_ref = new OpenWQ_vars(OpenWQ_hostModelconfig_ref->HydroComp.size());
        
        // Master Json
        OpenWQ_wqconfig_ref->OpenWQ_masterjson = "/u1/kck540/openWQ/openwq/bin/openWQ_master.json";


        OpenWQ_couplercalls_ref->InitialConfig(
            *OpenWQ_hostModelconfig_ref,
            *OpenWQ_json_ref,                    // create OpenWQ_json object
            *OpenWQ_wqconfig_ref,            // create OpenWQ_wqconfig object
            *OpenWQ_units_ref,                  // functions for unit conversion
            *OpenWQ_readjson_ref,            // read json files
            *OpenWQ_vars_ref,
            *OpenWQ_initiate_ref,            // initiate modules
            *OpenWQ_watertransp_ref,      // transport modules
            *OpenWQ_chem_ref,                   // biochemistry modules
            *OpenWQ_sinksource_ref,        // sink and source modules)
            *OpenWQ_output_ref);
    }

    // Testing output to know when the function exits
    std::cout << "Exiting decl Function" << std::endl;

    return 0;
}


int ClassWQ_OpenWQ::run_time_start(int year, int month, int day, int hour, int minute) {
    std::cout << "C++ run_time_start"     << std::endl;
    std::cout << "Year = "      << year   << std::endl;
    std::cout << "Month = "     << month  << std::endl;
    std::cout << "Day = "       << day    << std::endl;
    std::cout << "Hour = "      << hour   << std::endl;
    std::cout << "Minute = "    << minute << std::endl;
    //TODO: Needs to be passed simtime from SUMMA


    // Convert sim time to OpenWQ simtime which is seconds since 00:00 Jan 1, 1970 UTC
    // Call the method below
    
    time_t simtime = convert_time(year, month, day, hour, minute); // needs to be passed in

    // for (unsigned int hru=0;hru<this->getNum();hru++) {
    //     (OpenWQ_hostModelconfig_ref.SM)(hru,0,0) = 
    //     (OpenWQ_hostModelconfig_ref.Tair)(hru,0,0) =
    //     (OpenWQ_hostModelconfig_ref.Tsoil)(hru,0,0) =

    // }


    OpenWQ_couplercalls_ref->RunTimeLoopStart(
        *OpenWQ_hostModelconfig_ref,
        *OpenWQ_json_ref,
        *OpenWQ_wqconfig_ref,            // create OpenWQ_wqconfig object
        *OpenWQ_units_ref,                  // functions for unit conversion
        *OpenWQ_readjson_ref,            // read json files
        *OpenWQ_vars_ref,
        *OpenWQ_initiate_ref,            // initiate modules
        *OpenWQ_watertransp_ref,      // transport modules
        *OpenWQ_chem_ref,                   // biochemistry modules
        *OpenWQ_sinksource_ref,        // sink and source modules)
        *OpenWQ_solver_ref,
        *OpenWQ_output_ref,
        simtime);
    return 0;
}

int ClassWQ_OpenWQ::run_space() {
    std::cout << "C++ run_space" << std::endl;
    return 0;
}


// TODO: We need SimTime for this function as well
int ClassWQ_OpenWQ::run_time_end() {
    std::cout << "C++ run_time_end" << std::endl;
    

    time_t simtime = 0; // needs to be passed in

    OpenWQ_couplercalls_ref->RunTimeLoopEnd(
        *OpenWQ_hostModelconfig_ref,
        *OpenWQ_json_ref,
        *OpenWQ_wqconfig_ref,            // create OpenWQ_wqconfig object
        *OpenWQ_units_ref,                  // functions for unit conversion
        *OpenWQ_readjson_ref,            // read json files
        *OpenWQ_vars_ref,
        *OpenWQ_initiate_ref,            // initiate modules
        *OpenWQ_watertransp_ref,      // transport modules
        *OpenWQ_chem_ref,                   // biochemistry modules
        *OpenWQ_sinksource_ref,        // sink and source modules)
        *OpenWQ_solver_ref,
        *OpenWQ_output_ref,
        simtime);

    return 0;
}





// Interface functions to create Object
CLASSWQ_OPENWQ* create_openwq(int num) {
    std::cout << "C API, create_openwq" << std::endl;
    return new ClassWQ_OpenWQ(num);
}

void delete_openwq(CLASSWQ_OPENWQ* openWQ) {
    std::cout << "C API, delete openwq" << std::endl;
    delete openWQ;
}

int openwq_decl(ClassWQ_OpenWQ *openWQ) {
    std::cout << "C API, Decl" << std::endl;
    return openWQ->decl();
}

int openwq_getNum(const CLASSWQ_OPENWQ *openWQ) {
    return openWQ->getNum();
}

int openwq_run_time_start(ClassWQ_OpenWQ *openWQ, int year, int month, 
                    int day, int hour, int minute) {
    return openWQ->run_time_start(year, month, day, hour, minute);
}

int openwq_run(ClassWQ_OpenWQ *openWQ, int func) {
    if (func == 1) { // run_time_start()
        std::cout << "C API func = 1" << std::endl;

    
    } else if (func == 2) { // run_space()
        std::cout << "C API func = 2" << std::endl;
        return openWQ->run_space();

    } else if (func == 3) { // run_time_end()
        std::cout << "C API func = 3" << std::endl;
        return openWQ->run_time_end();
    
    } 
    
    std::cout << "C API func = ERROR" << std::endl;
    return -1;
    
}

