
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

#ifndef OPENWQ_EXTWATFLUX_SSH_INCLUDED
#define OPENWQ_EXTWATFLUX_SSH_INCLUDED


#include <tuple>
#include <string>
#include <algorithm>
#include <iterator>
#include <time.h>
#include <iostream>
#include <fstream>
#include "jnlohmann/json.hpp"
using json = nlohmann::json;

#include "OpenWQ_global.h"
#include "OpenWQ_units.h"
#include "OpenWQ_output.h"
#include "OpenWQ_utils.h"

class OpenWQ_extwatflux_ss{

    public:

    // Save Sink and Source data to tuple (more efficient than allways calling jnlohmann)
    // Driver function
    void Set_EWFandSS_drive(
        json &EWF_SS_json,
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_units& OpenWQ_units,
        OpenWQ_utils& OpenWQ_utils,
        OpenWQ_output& OpenWQ_output,
        std::string inputType);

    // if DATA_FORMAT = JSON or ASCII
    void Set_EWFandSS_jsonAscii(
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_utils& OpenWQ_utils,
        OpenWQ_units& OpenWQ_units,
        OpenWQ_output& OpenWQ_output,
        unsigned int ssf, unsigned int ssi,   // file-structure and substructure indexes
        std::string DataFormat,               // (JSON or ASCII)
        json EWF_SS_json_sub,                 // relevant sub-json
        std::string inputType,
        bool foundflag);

    // if DATA_FORMAT = HDF5
    void Set_EWF_h5(
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_utils& OpenWQ_utils,
        OpenWQ_units& OpenWQ_units,
        OpenWQ_output& OpenWQ_output,
        json EWF_SS_json_sub,  // relevant sub-json
        std::string inputType,
        bool foundflag);

    // Check if sink or sources needs to be applied
    // if JSON or ASCII
    void CheckApply_EWFandSS_jsonAscii(
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
        std::unique_ptr<arma::Mat<double>>& array_FORC);    // array FORC arma (SS or EWF)

    // if JSON
    void CheckApply_EWF_h5(
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
        const unsigned int SEC);                            // current model step: sec

    // Apply SS Source
    void Apply_Source(
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_output& OpenWQ_output,
        const unsigned int cmpi,                        // compartment model index
        const unsigned int chemi,                       // chemical model index    
        int ix_json,                                    // compartment model ix
        int iy_json,                                    // compartment model iy
        int iz_json,                                    // compartment model iz
        const double ss_data_json);                     // source load

    // Apply SS Sink
    void Apply_Sink(
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_output& OpenWQ_output,
        const unsigned int cmpi,                        // compartment model index
        const unsigned int chemi,                       // chemical model index    
        int ix_json,                                    // compartment model ix
        int iy_json,                                    // compartment model iy
        int iz_json,                                    // compartment model iz
        const double ss_data_json);                     // source load

    // Update EWF concentrations
    void Update_EWFconc_jsonAscii(
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_output& OpenWQ_output,
        const unsigned int ewfi,            // compartment model index
        const unsigned int chemi,           // chemical model index    
        int ix,                             // compartment model ix
        int iy,                             // compartment model iy
        int iz,                             // compartment model iz
        const double new_concVal);          // new EWF conc value

    void Update_EWFconc_h5(
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_output& OpenWQ_output,
        const unsigned int reqi,            // request index
        const unsigned int chemi,           // chemical model index    
        arma::cube& h5Conc_chemi_interp);

    void Convert_Mass_Units(
        double &ss_value,     // SS value
        std::string ss_unit);  // SS unit

    bool getModIndex(
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_output& OpenWQ_output,
        std::vector<std::string> &vec_list,
        std::string &obj_name,
        std::string &obj_text,
        unsigned long &vec_obj_index);

    bool getArrayElem( // elemEntry as string "all"
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_output& OpenWQ_output,
        std::string elemName,
        std::__cxx11::basic_string<char> elemEntry,
        int& elemVal,
        unsigned int& file_i,
        unsigned int& struc_i,
        unsigned int& row_i);

    void RemoveLoadBeforeSimStart_jsonAscii(
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_units& OpenWQ_units,
        std::unique_ptr<arma::Mat<double>>& array_FORC,
        const int YYYY,             // current model step: Year
        const int MM,               // current model step: month
        const int DD,               // current model step: day
        const int HH,               // current model step: hour
        const int MIN,              // current model step: min
        const int SEC);             // current model step: sec

    void RemoveLoadBeforeSimStart_h5(
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_units& OpenWQ_units,
        std::unique_ptr<std::vector<std::vector<std::vector<arma::Cube<double>>>>>& FORC_vec_data, // H5 interface data
        std::unique_ptr<std::vector<std::vector<time_t>>>& FORC_vec_time_t,          // H5 interface timestamps
        const int reqi,
        const int YYYY,         // current model step: Year
        const int MM,           // current model step: month
        const int DD,           // current model step: day
        const int HH,           // current model step: hour
        const int MIN,          // current model step: min
        const int SEC);         // current model step: sec

    void UpdateAllElemTimeIncremts(
        std::unique_ptr<arma::Mat<double>>& array_FORC,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_utils& OpenWQ_utils,
        OpenWQ_units& OpenWQ_units,
        const int YYYY,             // current model step: Year
        const int MM,               // current model step: month
        const int DD,               // current model step: day
        const int HH,               // current model step: hour
        const int MIN,              // current model step: min
        const int SEC);             // current model step: sec

    void AppendRow_SS_EWF_FORC_jsonAscii(
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        std::string inputType,
        arma::vec row_data_col);

    void AppendCube_SS_EWF_FORC_h5(
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        int h5EWF_request_index,        // get request index
        int chemi,                      // chem index
        bool flag_newChem,              // flag for new timestep, push back new vector row [i]
        bool flag_newJSON_h5Request,     // new json-h5-ewf request
        time_t timestamp_time_t);   // timestamp in time_t

};

#endif
