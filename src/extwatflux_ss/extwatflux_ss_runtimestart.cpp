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
    if (OpenWQ_wqconfig.is_tstep1()){

        // Remove requested loads that are prior to the simulation start datetime
        RemoveLoadBeforeSimStart_jsonAscii(
            OpenWQ_wqconfig,
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
            OpenWQ_wqconfig,
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
    simTime = OpenWQ_units.convertTime_ints2time_t(
        OpenWQ_wqconfig,
        YYYY, MM, DD, HH, MIN, SEC);

    // Get number of rows in SinkSource_FORC
    num_rowdata = (*array_FORC).n_rows; 

    // First reset all values of ewf_conc to ZERO for new time step
    #pragma omp parallel for collapse(2) num_threads(OpenWQ_wqconfig.get_num_threads_requested())
    for (unsigned int ewfi=0;ewfi<OpenWQ_hostModelconfig.get_num_HydroExtFlux();ewfi++){ // 
        for (unsigned int chemi=0;chemi<(OpenWQ_wqconfig.BGC_general_num_chem);chemi++){
            (*OpenWQ_vars.ewf_conc)(ewfi)(chemi).zeros();}}

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
        jsonTime = OpenWQ_units.convertTime_ints2time_t(
            OpenWQ_wqconfig,
            YYYY_json, MM_json, DD_json, HH_json, MIN_json, SEC_json);

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
            jsonTime = OpenWQ_units.convertTime_ints2time_t(
                OpenWQ_wqconfig,
                YYYY_json, MM_json, DD_json, HH_json, MIN_json, SEC_json);

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
    time_t h5EWF_time_before;               // iteractive time extraction from FORC_vec_time_t (before simtime)
    time_t h5EWF_time_after;                // iteractive time extraction from FORC_vec_time_t (after simtime)
    unsigned long long num_ewfh5_requests;  // number of json ewf-h5 requests
    unsigned long long num_chems;           // number of chems in h5 stucture
    unsigned long long num_timeStamps;      // number of timesteps in h5 stucture
    arma::cube h5Conc_chemi_before;         // iteractive h5-chemi concentration at previous timestep 
    arma::cube h5Conc_chemi_after;          // iteractive h5-chemi concentration at current or next timestep 
    arma::cube h5Conc_chemi_interp;         // iteractive h5-chemi concentration at interpolated timestep 
    std::string msg_string;                 // error message

    // Get number of json ewf-h5 requests
    num_ewfh5_requests = (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_time).size();

    // Convert sim time to time_t
    simTime = OpenWQ_units.convertTime_ints2time_t(
        OpenWQ_wqconfig,
        YYYY, MM, DD, HH, MIN, SEC);

    /* ########################################
    // Data update/clean-up at 1st timestep
    ######################################## */
    if (OpenWQ_wqconfig.is_tstep1()){

        // Loop over all requests
        for (unsigned int reqi=0;reqi<num_ewfh5_requests;reqi++){
            // Loop over all of them to removee load before simtime
            RemoveLoadBeforeSimStart_h5(
                OpenWQ_wqconfig,
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
    for (unsigned int reqi=0;reqi<num_ewfh5_requests;reqi++){

        // Number of timestamps
        num_chems = (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_data)[reqi].size();
        num_timeStamps = (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_time)[reqi].size();

        /* ########################################
        // Loop over timeStamps
        // to find the row at (or above) the current simTime timeStamp
        ######################################## */

        for (unsigned long long tStamp=0;tStamp<num_timeStamps;tStamp++){

            // Get timestamp tStamp
            h5EWF_time_after =  (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_time)[reqi][tStamp];
            
            // if at next timestep
            // this will then get upper and lower timesteps
            if (h5EWF_time_after > simTime){
                
                // Loop over all chemical species
                for (unsigned long long chemi=0;chemi<num_chems;chemi++){
                    
                    // Get h5-ewf for chemical chemi at tStamp-1 timestep
                    // if at tStamp==0, then need to get the next step
                    if(tStamp!=0){
                        h5Conc_chemi_before = (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_data)[reqi][chemi][tStamp-1];
                        h5EWF_time_before = (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_time)[reqi][tStamp-1];
                    }else{
                        h5Conc_chemi_before = (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_data)[reqi][chemi][tStamp];
                        h5EWF_time_before = (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_time)[reqi][tStamp];
                    }

                    // ########################################
                    // Interpolation options

                    // Option: "STEP"
                    if(OpenWQ_wqconfig.is_h5EWF_interpMethod("STEP")){

                        h5Conc_chemi_interp = h5Conc_chemi_before;

                    }else{

                        // Get h5-ewf for chemical chemi at tStamp+1 timestep
                            h5Conc_chemi_after = (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_data)[reqi][chemi][tStamp];
                        // Option: "LINEAR"
                        if(OpenWQ_wqconfig.is_h5EWF_interpMethod("LINEAR")){
                            h5Conc_chemi_interp = h5Conc_chemi_before 
                                                + (
                                                    (h5Conc_chemi_after - h5Conc_chemi_before) 
                                                    * (simTime - h5EWF_time_before) 
                                                    / (h5EWF_time_after - h5EWF_time_before)
                                                );
                        }
                        // Option: "NEAREST"
                        else if(OpenWQ_wqconfig.is_h5EWF_interpMethod("NEAREST")){
                            if( (simTime - h5EWF_time_before) 
                                > (h5EWF_time_after - h5EWF_time_before)/2){
                                h5Conc_chemi_interp = h5Conc_chemi_after;
                            }else{
                                h5Conc_chemi_interp = h5Conc_chemi_before;}
                        // Option: unkown 
                        // (defaulting to STEP and throw warning message)
                        }else {
                            OpenWQ_wqconfig.set_h5EWF_interpMethod("STEP");
                            h5Conc_chemi_interp = h5Conc_chemi_before;

                            //Create message
                            msg_string = OpenWQ_wqconfig.h5EWF_interp_warning_msg();
                               
                            // Print it (Console and/or Log file)
                            OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);
                        }
                    }

                // Update h5Conc_chemi_interp for all elements
                    Update_EWFconc_h5(
                        OpenWQ_vars,
                        OpenWQ_wqconfig,
                        OpenWQ_hostModelconfig,
                        OpenWQ_output,
                        reqi,
                        chemi,
                        h5Conc_chemi_interp); 

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
    unsigned int nx = OpenWQ_hostModelconfig.get_HydroComp_num_cells_x_at(cmpi);
    unsigned int ny = OpenWQ_hostModelconfig.get_HydroComp_num_cells_y_at(cmpi);
    unsigned int nz = OpenWQ_hostModelconfig.get_HydroComp_num_cells_z_at(cmpi);

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
            "Compartment=" + OpenWQ_hostModelconfig.get_HydroComp_name_at(cmpi)
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
    unsigned int nx = OpenWQ_hostModelconfig.get_HydroComp_num_cells_x_at(cmpi);
    unsigned int ny = OpenWQ_hostModelconfig.get_HydroComp_num_cells_y_at(cmpi);
    unsigned int nz = OpenWQ_hostModelconfig.get_HydroComp_num_cells_z_at(cmpi);

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
            "Compartment=" + OpenWQ_hostModelconfig.get_HydroComp_name_at(cmpi)
            + ", Chemical=" + OpenWQ_wqconfig.BGC_general_chem_species_list[chemi]
            + ", ix=" + std::to_string(ix)
            + ", iy=" + std::to_string(iy)
            + ", iz=" + std::to_string(iz);

        // Print it (Console and/or Log file)
        OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);

    }
}


/* #################################################
 // Update concentration if EWF: 
 // input type: JSON and ASCII input
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
    unsigned int nx = OpenWQ_hostModelconfig.get_HydroExtFlux_num_cells_x_at(ewfi);
    unsigned int ny = OpenWQ_hostModelconfig.get_HydroExtFlux_num_cells_y_at(ewfi);
    unsigned int nz = OpenWQ_hostModelconfig.get_HydroExtFlux_num_cells_z_at(ewfi);

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
        
        // Now update the elements with information in the EWF file
        // It has to be "+="" and not ""="" because of missmatch between
        // left and right variables. 
        // Since (*OpenWQ_vars.ewf_conc)(ewfi)(chemi) is reset in solver,
        // this works just fine
        (*OpenWQ_vars.ewf_conc)(ewfi)(chemi)(
            arma::span(spX_min, spX_max), 
            arma::span(spY_min, spY_max),
            arma::span(spZ_min, spZ_max)) += new_concVal;

        // Replace all negative values by zero
        // Needed because can have negative values
        (*OpenWQ_vars.ewf_conc)(ewfi)(chemi).transform( [](double val) { return (val < 0.0) ? 0.0 : val; });

    }catch (...){

        // Through a warning if request out of boundaries
        // Create Message
        msg_string = 
            "<OpenWQ> WARNING: EWF conc out of boundaries."
            "Requested EWF concentration update using JSON/ASCII input ignored and set to zero: "
            "EWF=" + OpenWQ_hostModelconfig.get_HydroComp_name_at(ewfi)
            + ", Chemical=" + OpenWQ_wqconfig.BGC_general_chem_species_list[chemi]
            + ", ix=" + std::to_string(ix)
            + ", iy=" + std::to_string(iy)
            + ", iz=" + std::to_string(iz);

        // Print it (Console and/or Log file)
        OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);

    }
}

/* #################################################
 // Update concentration if EWF: 
 // input type: H5
 ################################################# */
void OpenWQ_extwatflux_ss::Update_EWFconc_h5(
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_output& OpenWQ_output,
    const unsigned int reqi,            // request index
    const unsigned int chemi,           // chemical model index    
    arma::cube& h5Conc_chemi_interp){    // arma_mat with concentrations

    // Local Variables
    std::string msg_string;             // error/warning message string
    unsigned ewfi;                    // EWF input
    unsigned int spX_min, spX_max, spY_min, spY_max, spZ_min, spZ_max;
    unsigned int nx_interf, ny_interf, nz_interf;
    
    // Get external water flux id
    ewfi = (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec_ewfCompID)[reqi];

    // Get num of interface elements
    nx_interf = OpenWQ_hostModelconfig.get_HydroExtFlux_num_cells_x_at(ewfi) - 1;
    ny_interf = OpenWQ_hostModelconfig.get_HydroExtFlux_num_cells_y_at(ewfi) - 1;
    nz_interf = OpenWQ_hostModelconfig.get_HydroExtFlux_num_cells_z_at(ewfi) - 1;

    // Set range of cells to replace
    spX_min = 0; spX_max = nx_interf;
    spY_min = 0; spY_max = ny_interf;
    spZ_min = 0; spZ_max = nz_interf;

    try{
        
        // Now update the elements with information in the EWF file
        (*OpenWQ_vars.ewf_conc)(ewfi)(chemi)(
            arma::span(spX_min, spX_max), 
            arma::span(spY_min, spY_max),
            arma::span(spZ_min, spZ_max)) = h5Conc_chemi_interp;

        // Replace all negative values by zero
        // Needed because can have negative values
        (*OpenWQ_vars.ewf_conc)(ewfi)(chemi).transform( [](double val) { return (val < 0.0) ? 0.0 : val; });

    }catch (...){

        // Through a warning if request out of boundaries
        // Create Message
        msg_string = 
            "<OpenWQ> WARNING: EWF conc out of boundaries."
            "Requested EWF concentration update using HDF5 input ignored and set to zero: "
            "EWF=" + OpenWQ_hostModelconfig.get_HydroComp_name_at(ewfi)
            + ", Chemical=" + OpenWQ_wqconfig.BGC_general_chem_species_list[chemi];

        // Print it (Console and/or Log file)
        OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);

    }

}


/* #################################################
// At timestep 1, 
// remove requested loads that are prior to the simulation start datetime
// only do this for rows that don't have any "all" elements
#################################################*/
// JSON and ASCII entries
void OpenWQ_extwatflux_ss::RemoveLoadBeforeSimStart_jsonAscii(
    OpenWQ_wqconfig& OpenWQ_wqconfig,
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
    simTime = OpenWQ_units.convertTime_ints2time_t(
        OpenWQ_wqconfig,
        YYYY, MM, DD, HH, MIN, SEC);

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
            OpenWQ_wqconfig,
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
    const int SEC){         // current model step: sec

    // Local variables
    time_t simTime;                         // current simulation time in time_t
    time_t h5EWF_time;                      // iteractive time extraction from FORC_vec_time_t
    unsigned long long num_timeStamps;               // number of timesteps in h5 stucture
    unsigned long long num_chems;                    // number of timesteps in h5 stucture
    unsigned long long n_rows2remove;       // number of rows to remove
    std::vector<int> timStampsIndex2Remove; // List of rows indexes to remove  
    long long ri2remove;                    // index of row to remove   

    // Convert sim time to time_t
    simTime = OpenWQ_units.convertTime_ints2time_t(
        OpenWQ_wqconfig,
        YYYY, MM, DD, HH, MIN, SEC);

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

    // Sort timStampsIndex2Remove in descending order
    // This is needed because, as the vector rows are removed,
    // the indexes of the rows still to remove will change
    // So, the removal needs to start from the bottom of the vector to avoid this
    std::sort(timStampsIndex2Remove.rbegin(), timStampsIndex2Remove.rend());

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
        (*FORC_vec_time_t)[reqi].erase((*FORC_vec_time_t)[reqi].begin()+(ri2remove));

        // Loop over all chemical species
        for (unsigned long long chemi=0;chemi<num_chems;chemi++){

            (*FORC_vec_data)[reqi][chemi].erase((*FORC_vec_data)[reqi][chemi].begin()+(ri2remove));

        }
    }
}


/* #################################################
// At timestep 1, 
// adjust time increments for YYYY, MM, DD, HH, MIN
#################################################*/
void OpenWQ_extwatflux_ss::UpdateAllElemTimeIncremts(
    std::unique_ptr<arma::Mat<double>>& array_FORC,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
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
    simTime = OpenWQ_units.convertTime_ints2time_t(
        OpenWQ_wqconfig,
        YYYY, MM, DD, HH, MIN, SEC);

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
            OpenWQ_wqconfig,
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
                        OpenWQ_wqconfig,
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
                        OpenWQ_wqconfig,
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
                        OpenWQ_wqconfig,
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
                        OpenWQ_wqconfig,
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
                        OpenWQ_wqconfig,
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
                        OpenWQ_wqconfig,
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