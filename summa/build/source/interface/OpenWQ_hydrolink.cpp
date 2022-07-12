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
// initalize numHRUs value
ClassWQ_OpenWQ::ClassWQ_OpenWQ(int _numHru): numHRU(_numHru) {}

// Deconstructor
ClassWQ_OpenWQ::~ClassWQ_OpenWQ() {}

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
    OpenWQ_hostModelconfig_ref = new OpenWQ_hostModelconfig(); // Initalize hostModelconfig
    OpenWQ_couplercalls_ref = new OpenWQ_couplercalls();
    OpenWQ_json_ref = new OpenWQ_json();
    OpenWQ_wqconfig_ref = new OpenWQ_wqconfig(13); // This is 11 because the OpenWQ_global.h says it should be in the class definition
    OpenWQ_units_ref = new OpenWQ_units();
    OpenWQ_readjson_ref = new OpenWQ_readjson();
    OpenWQ_initiate_ref = new OpenWQ_initiate();
    OpenWQ_watertransp_ref = new OpenWQ_watertransp();
    OpenWQ_chem_ref = new OpenWQ_chem();
    OpenWQ_sinksource_ref = new OpenWQ_sinksource();
    OpenWQ_output_ref = new OpenWQ_output();

    if (OpenWQ_hostModelconfig_ref->HydroComp.size()==0) {
        // Make sure to use capital letters for compartment names
        // OpenWQ_hostModelconfig_ref->HydroComp.push_back(OpenWQ_hostModelconfig::hydroTuple(0,"SOIL_RECHR",numHRU,1,1));
        OpenWQ_hostModelconfig_ref->HydroComp.push_back(OpenWQ_hostModelconfig::hydroTuple(1,"SCALARCANOPYWAT",numHRU,1,1));
        OpenWQ_hostModelconfig_ref->HydroComp.push_back(OpenWQ_hostModelconfig::hydroTuple(2,"MLAYERMATRICHEAD",numHRU,1,1));
        OpenWQ_hostModelconfig_ref->HydroComp.push_back(OpenWQ_hostModelconfig::hydroTuple(2,"SCALARAQUIFER",numHRU,1,1));

        OpenWQ_vars_ref = new OpenWQ_vars(OpenWQ_hostModelconfig_ref->HydroComp.size());
        
        // Master Json
        OpenWQ_wqconfig_ref->OpenWQ_masterjson = "/code/openwq-summa/synthetic_tests/9_batch_singleSp_1storder/summa/openWQ_master.json";


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
    return 0;
}

// SoilMoisture does not have a value - it is passed as 0
int ClassWQ_OpenWQ::run_time_start(int numHRU, int simtime_summa[],
        double soilMoisture[], double soilTemp[], double airTemp[],
        double SWE_vol[], double canopyWat[], double matricHead_vol[], double aquiferStorage[]) {

    time_t simtime = convert_time(simtime_summa[0], simtime_summa[1], simtime_summa[2], simtime_summa[3], simtime_summa[4]);

    for (int i = 0; i < numHRU; i++) {
        (*OpenWQ_hostModelconfig_ref->SM)   (i,0,0) = soilMoisture[i]; 
        (*OpenWQ_hostModelconfig_ref->Tair) (i,0,0) = airTemp[i];
        (*OpenWQ_hostModelconfig_ref->Tsoil)(i,0,0) = soilTemp[i];
        (*OpenWQ_hostModelconfig_ref->waterVol_hydromodel)[0](i,0,0) = SWE_vol[i];
        (*OpenWQ_hostModelconfig_ref->waterVol_hydromodel)[1](i,0,0) = canopyWat[i];
        // (*OpenWQ_hostModelconfig_ref->waterVol_hydromodel)[2](i,0,0) = matricHead_vol[i];
        (*OpenWQ_hostModelconfig_ref->waterVol_hydromodel)[2](i,0,0) = aquiferStorage[i];
    }

    // *OpenWQ_hostModelconfig_ref.time_step = 5;

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

int ClassWQ_OpenWQ::run_space(int simtime_summa[], int source, int ix_s, int iy_s, int iz_s,
        int recipient, int ix_r, int iy_r, int iz_r, double wflux_s2r, double wmass_source) {
    std::cout << "C++ run_space" << std::endl;
    std::cout << source << ", " << ix_s << ", " << iy_s << ", " << iz_s << ", " << recipient << ", " << ix_r 
        << ", " << iy_r << ", " << iz_r << ", " << wflux_s2r << ", " << wmass_source << std::endl;

    time_t simtime = convert_time(simtime_summa[0], simtime_summa[1], simtime_summa[2], simtime_summa[3], simtime_summa[4]);
    

    OpenWQ_couplercalls_ref->RunSpaceStep(
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
        simtime,
        source,
        ix_s,
        iy_s,
        iz_s,
        recipient,
        ix_r,
        iy_r,
        iz_r,
        wflux_s2r,
        wmass_source);

    return 0;
}

int ClassWQ_OpenWQ::run_time_end(int simtime_summa[]) {
    
    time_t simtime = convert_time(simtime_summa[0], simtime_summa[1], simtime_summa[2], simtime_summa[3], simtime_summa[4]);


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




/**
 * Below is the implementation of the C interface for SUMMA. When Summa calls a function 
 * the functions below are the ones that are invoked first. 
 * The openWQ object is then passed from Fortran to these functions so that the OpenWQ object
 * can be called. The openWQ object methods are defined above.
 */
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


int openwq_run_time_start(ClassWQ_OpenWQ *openWQ, int numHRU, int simtime_summa[], double soilMoisture[], double soilTemp[], double airTemp[],
    double SWE_vol[], double canopyWat_vol[], double matricHead_vol[], double aquiferStorage_vol[]) {
    
    return openWQ->run_time_start(numHRU, simtime_summa,
        soilMoisture, soilTemp, airTemp, SWE_vol, canopyWat_vol, matricHead_vol, aquiferStorage_vol);
}


int openwq_run_space(ClassWQ_OpenWQ *openWQ, int simtime_summa[], int source, int ix_s, int iy_s, int iz_s,
        int recipient, int ix_r, int iy_r, int iz_r, double wflux_s2r, double wmass_source) {

    return openWQ->run_space(simtime_summa, source, ix_s, iy_s, iz_s,
        recipient, ix_r, iy_r, iz_r, wflux_s2r, wmass_source);
}

    int openwq_run_time_end(ClassWQ_OpenWQ *openWQ, int simtime_summa[]) {

    return openWQ->run_time_end(simtime_summa);
}

