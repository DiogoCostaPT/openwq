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

using namespace CRHM;


void ClassWQ_OpenWQ::decl(
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_json& OpenWQ_json,                    // create OpenWQ_json object
    OpenWQ_wqconfig& OpenWQ_wqconfig,            // create OpenWQ_wqconfig object
    OpenWQ_units& OpenWQ_units,                  // functions for unit conversion
    OpenWQ_readjson& OpenWQ_readjson,            // read json files
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_initiate& OpenWQ_initiate,            // initiate modules
    OpenWQ_watertransp& OpenWQ_watertransp,      // transport modules
    OpenWQ_chem& OpenWQ_chem,                   // biochemistry modules
    OpenWQ_sinksource& OpenWQ_sinksource,        // sink and source modules)
    OpenWQ_output& OpenWQ_output,
    unsigned long nhru)
{
    
    Description = "Multi-chemistry OpenWQ framework";

    // Initiate output VTU file name string 
    //STASKED// std::string vtufilename;

    // Create Object: OpenWQ_hostModelconfig (link to host hydrological model)
    //OpenWQ_hostModelconfig OpenWQ_hostModelconfig;

    // ################################
    // Link openWQ data strucuture indexes to hydrological model compartments
    // ################################
    typedef std::tuple<int,std::string,int, int, int> hydroTuple;
    /* hydroTuple: 
    (1) index in openWQ variable, 
    (2) name identifier in openWQ_config.json, 
    (3) number of cell in x-direction
    (4) number of cell in y-direction
    (5) number of cell in z-direction
    */

    // Check if the call is from ::decl (allow to set OpenWQ_hostModelconfig)
    // or ::initbase (do not allow because OpenWQ_hostModelconfig has already been
    // defined)
    if (OpenWQ_hostModelconfig.HydroComp.size()==0){

        OpenWQ_hostModelconfig.HydroComp.push_back(hydroTuple(0,"SWE",nhru,1,1));
        OpenWQ_hostModelconfig.HydroComp.push_back(hydroTuple(1,"RUNOFF",nhru,1,1));
        OpenWQ_hostModelconfig.HydroComp.push_back(hydroTuple(2,"SSR",nhru,1,1));
        OpenWQ_hostModelconfig.HydroComp.push_back(hydroTuple(3,"SD",nhru,1,1));
        OpenWQ_hostModelconfig.HydroComp.push_back(hydroTuple(4,"SOIL_RECHR",nhru,1,1));
        OpenWQ_hostModelconfig.HydroComp.push_back(hydroTuple(5,"SOIL_LOWER",nhru,1,1));
        OpenWQ_hostModelconfig.HydroComp.push_back(hydroTuple(6,"SURFSOIL",nhru,1,1));
        OpenWQ_hostModelconfig.HydroComp.push_back(hydroTuple(7,"GW",nhru,1,1));
        // (add other compartments as needed)...
    
        // Number of hydrological compartments in host model
        OpenWQ_hostModelconfig.num_HydroComp = OpenWQ_hostModelconfig.HydroComp.size(); 

        // Create Object: OpenWQ_json (Input JSON files) and wqconfig
        //OpenWQ_json OpenWQ_json;            // create OpenWQ_json object
        //OpenWQ_wqconfig OpenWQ_wqconfig;    // create OpenWQ_wqconfig object
        //OpenWQ_units OpenWQ_units;          // functions for unit conversion
        

        // ################################
        // Read JSON file
        // ################################
        // Get master file location
        OpenWQ_wqconfig.OpenWQ_masterjson = "build/openWQ_master.json";
        //OpenWQ_wqconfig.OpenWQ_masterjson = OpenWQ_masterjson_path->Strings[0];

        //OpenWQ_readjson OpenWQ_readjson; // create object: json files load modules
        OpenWQ_readjson.read_all(
            OpenWQ_json,
            OpenWQ_hostModelconfig,
            OpenWQ_wqconfig,
            OpenWQ_units,
            OpenWQ_output);

        // Fix number of species for use in other module
        // Particularly WQ_CRHM that computes transport
        Global::numsubstances = OpenWQ_wqconfig.num_chem;
        
        // Create Object: OpenWQ_vars (openWQ variables)
        //OpenWQ_vars OpenWQ_vars(
        //    num_HydroComp);

        // OpenWQ_initiate
        //OpenWQ_initiate OpenWQ_initiate; // create object: start modules e.g., initiate
        
        // ################################
        // Allocate memory: set variable sizes
        // ################################
        
        OpenWQ_initiate.initmemory(
            OpenWQ_json,
            OpenWQ_vars,
            OpenWQ_hostModelconfig,
            OpenWQ_wqconfig,
            OpenWQ_output);
        
        // Create objects 
        //OpenWQ_watertransp OpenWQ_watertransp;      // transport modules
        //OpenWQ_chem OpenWQ_chem;                    // biochemistry modules
        //OpenWQ_sinksource OpenWQ_sinksource;        // sink and source modules
        //STASKED// OpenWQ_output OpenWQ_output;                // print modules
        
        //STASKED-not needed// unsigned int ts_hosthydromod = 5000; // (timesteps) TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL

        /* #################################################
        // Parse biogeochemical expressions (and save in global)
        ################################################# */
        OpenWQ_chem.setBGCexpressions(
            OpenWQ_json,
            OpenWQ_hostModelconfig,
            OpenWQ_wqconfig,
            OpenWQ_vars,
            OpenWQ_output);

        /* #################################################
        // Parse sink and source inputs and store them in tuple and arma::mat for rapid access
        ################################################# */
        OpenWQ_sinksource.SetSinkSource(
            OpenWQ_json,
            OpenWQ_vars,
            OpenWQ_hostModelconfig,
            OpenWQ_wqconfig,
            OpenWQ_units,
            OpenWQ_output);  

    }

    // ################################
    // Get Variables from other modules or meteorology
    // ################################
    // SWE
    declgetvar("*", "SWE", "(mm)", &SWE);
    declputvar("*", "SWE_conc", "(mg/l)", &SWE_conc, &SWE_conc_lay);
    // runoff
    declgetvar("*", "soil_runoff", "(mm)", &soil_runoff);
    declputvar("*", "soil_runoff_cWQ", "(mm)", &soil_runoff_cWQ,&soil_runoff_cWQ_lay);
    // ssr
    declgetvar("*", "soil_ssr", "(mm)", &soil_ssr);
    declputvar("*", "soil_ssr_conc", "(mm)", &soil_ssr_conc,&soil_ssr_conc_lay);
    // Sd
    declstatvar("Sd", NHRU, "Depression storage.", "(mm)", &Sd);
    declstatvar("Sd_conc", NDEFN, "Concentration: Depression storage.", "(mg/l)", &Sd_conc, &Sd_conc_lay, Global::numsubstances);
    // soil_rechr
    declgetvar("*", "soil_rechr", "(mm)", &soil_rechr);
    declputvar("*", "conc_soil_rechr", "(mg/l)", &conc_soil_rechr, &conc_soil_rechr_lay);
    // soil moist (vol) & soil_lower (conc)
    declgetvar("*", "soil_moist", "(mm)", &soil_moist);
    declputvar("*", "conc_soil_lower", "(mg/l)", &conc_soil_lower, &conc_soil_lower_lay);
    // surficial soil (only mass)
    declputvar("*", "surfsoil_solub_mWQ", "(mg/l)", &surfsoil_solub_mWQ, &surfsoil_solub_mWQ_lay);
    // gw
    declstatvar("gw", NHRU, "ground water storage.", "(mm)", &gw);
    declstatvar("gw_conc", NDEFN, "Concentration: ground water storage.", "(mg/l)", &gw_conc, &gw_conc_lay, Global::numsubstances);

    // ################################
    // Get Parameters from other modules or meteorology
    // ################################
    // air temperature
    declgetvar("*", "hru_t", "(°C)", &hru_t);
    declparam("hru_area", NHRU, "[1]", "1e-6", "1e+09", "hru area", "(km^2)", &hru_area);
    declparam("soil_rechr_max", NHRU, "[60.0]", "0.0", "350.0",
    "Maximum value for soil recharge zone (upper portion of soil_moist where losses occur as both evaporation "//
    "and transpiration).  Must be less than or equal to soil_moist.","( )", &soil_rechr_max);


}

void ClassWQ_OpenWQ::run(
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_json& OpenWQ_json,                    // create OpenWQ_json object
    OpenWQ_wqconfig& OpenWQ_wqconfig,            // create OpenWQ_wqconfig object
    OpenWQ_units& OpenWQ_units,                  // functions for unit conversion
    OpenWQ_readjson& OpenWQ_readjson,            // read json files
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_initiate& OpenWQ_initiate,            // initiate modules
    OpenWQ_watertransp& OpenWQ_watertransp,      // transport modules
    OpenWQ_chem& OpenWQ_chem,                    // biochemistry modules
    OpenWQ_sinksource& OpenWQ_sinksource,        // sink and source modules)
    OpenWQ_solver& OpenWQ_solver,                // solver module
    OpenWQ_output& OpenWQ_output)                // output modules
{

    Description = "Multi-chemistry OpenWQ framework";


    // Get mobile species 
    // Only update mass witmobile_speciesh CRHM's wate volume for the mobile species
    std::vector<double> mobile_species = 
        OpenWQ_json.BGCcycling["CHEMICAL_SPECIES"]["MOBILE_SPECIES"];

    // Change indexing to start in zero
    for (auto& item : mobile_species){item += 1;}


    unsigned int Sub_mob;   // interactive species index (for mobile species)

    // Reset Derivatives 
    // Needed before start of new time iteraction
    OpenWQ_solver.Reset_Deriv(
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_vars,
        true,       // reset inst derivatives
        false);     // reset cumulative derivatives


    // ################################
    // Read and Set Initial Conditions
    // ################################
    OpenWQ_hostModelconfig.interaction_step = getstep();
    
    // Retrieve simulation timestamp
    // convert to OpenWQ time convention: seconds since 00:00 hours, Jan 1, 1970 UTC
    // this allows the use of a number of funtions of C++
    time_t simtime = (Global::DTnow - 70 * 365 - 21) * 24 * 3600;

    
    // Get Fluxes from Hydrological model
    // Save them to (*OpenWQ_hostModelconfig.fluxes_hydromodel)
    // Convert units to m3

    // CRHM water units are in mm
    // CRHM area units are in km2
    // So conversion of mm (water) to m3 is calculated as follows:
    // water_m3 = (water_mm / 1000) * (area_km2 * 1000000) <=>
    // water_m3 = water_mm * area_km2 * 1000
    for (unsigned int hru=0;hru<nhru;hru++){

        // SWE
        (*OpenWQ_hostModelconfig.fluxes_hydromodel)[0](hru,0,0) 
            = std::fmax(SWE[hru] * 1000 * hru_area[hru] 
                , 0.0f);
        // soil_runoff
        (*OpenWQ_hostModelconfig.fluxes_hydromodel)[1](hru,0,0) 
            = std::fmax(soil_runoff[hru] * 1000 * hru_area[hru] 
                , 0.0f);
        (*OpenWQ_hostModelconfig.fluxes_hydromodel)[2](hru,0,0) 
            = std::fmax(soil_ssr[hru] * 1000 * hru_area[hru] 
                , 0.0f);
        // Sd
        (*OpenWQ_hostModelconfig.fluxes_hydromodel)[3](hru,0,0) 
            = std::fmax(Sd[hru] * 1000 * hru_area[hru] 
                , 0.0f);
        // Soi_rechr
        (*OpenWQ_hostModelconfig.fluxes_hydromodel)[4](hru,0,0) 
            = std::fmax(soil_rechr[hru] * 1000 * hru_area[hru] 
                , 0.0f);
        // Soil_moist
        (*OpenWQ_hostModelconfig.fluxes_hydromodel)[5](hru,0,0) 
            = std::fmax((soil_moist[hh] - soil_rechr[hh]) * 1000 * hru_area[hru] 
                , 0.0f);
        // Surfsoil (not water in it)
        (*OpenWQ_hostModelconfig.fluxes_hydromodel)[6](hru,0,0) 
            = -9999;
        // gw
        (*OpenWQ_hostModelconfig.fluxes_hydromodel)[7](hru,0,0) 
            = std::fmax(gw[hru] * 1000 * hru_area[hru] 
                , 0.0f);
        
    }

    // Initiation steps that only need to be done once (at the start of the simulation)
    if (OpenWQ_hostModelconfig.interaction_step == 1){

        // Initiate OpenWQ_wqconfig.nexttime_out with simtime
        // this is only applicable at the start of the simulation
        OpenWQ_wqconfig.nexttime_out = simtime;

        // Initiate time_step
        OpenWQ_hostModelconfig.time_step = 0.0f;

        // Set IC 
        // (icmp, chemi, ix, ix and iz loop is inside the function)
        OpenWQ_initiate.readSet(
            OpenWQ_json,
            OpenWQ_vars,
            OpenWQ_hostModelconfig,
            OpenWQ_wqconfig,
            OpenWQ_units,
            OpenWQ_output);

    }else{

        // Update time step and difference as fraction of 1 day (because reaction kinetics are
        // given is rate per day. Then update time_previous for next time step
        OpenWQ_hostModelconfig.time_step = (simtime - OpenWQ_wqconfig.time_previous);

        // ################################
        // Retried current state of state variables form CRHM
        // Convert from concentration (default unit in CRHM) to mass (default unit of OpenWQ)
        // ################################ 

    
        #pragma omp parallel for private(Sub_mob) collapse(2) num_threads(OpenWQ_wqconfig.num_threads_requested)
        for(unsigned int hru = 0; hru < nhru; ++hru) {

            for(unsigned int Sub_i = 0; Sub_i < mobile_species.size(); ++Sub_i) {

                // Get mobile species indexes
                // Only update mass/concentrations using CRHM's water for mobile species
                Sub_mob = mobile_species[Sub_i];
    
                // SWE
                (*OpenWQ_vars.d_chemass_dt_transp)(0)(Sub_mob)(hru,0,0) = 
                    (std::fmax(SWE_conc_lay[Sub_mob][hru],0.0f)
                        * (*OpenWQ_hostModelconfig.fluxes_hydromodel)[0](hru,0,0) 
                    ) - (*OpenWQ_vars.chemass)(0)(Sub_mob)(hru,0,0);

                // RUNOFF
                (*OpenWQ_vars.d_chemass_dt_transp)(1)(Sub_mob)(hru,0,0) = 
                    (std::fmax(soil_runoff_cWQ_lay[Sub_mob][hru],0.0f)
                    * (*OpenWQ_hostModelconfig.fluxes_hydromodel)[1](hru,0,0) 
                    ) - (*OpenWQ_vars.chemass)(1)(Sub_mob)(hru,0,0);

                // SSR
                (*OpenWQ_vars.d_chemass_dt_transp)(2)(Sub_mob)(hru,0,0) = 
                    (std::fmax(soil_ssr_conc_lay[Sub_mob][hru],0.0f)
                    * (*OpenWQ_hostModelconfig.fluxes_hydromodel)[2](hru,0,0) 
                    ) - (*OpenWQ_vars.chemass)(2)(Sub_mob)(hru,0,0);

                // SD
                (*OpenWQ_vars.d_chemass_dt_transp)(3)(Sub_mob)(hru,0,0) = 
                    (std::fmax(Sd_conc_lay[Sub_mob][hru],0.0f)
                    * (*OpenWQ_hostModelconfig.fluxes_hydromodel)[3](hru,0,0) 
                    ) - (*OpenWQ_vars.chemass)(3)(Sub_mob)(hru,0,0);

                
                // SOIL_RECHR
                (*OpenWQ_vars.d_chemass_dt_transp)(4)(Sub_mob)(hru,0,0) = 
                    (std::fmax(conc_soil_rechr_lay[Sub_mob][hru],0.0f)
                    * (*OpenWQ_hostModelconfig.fluxes_hydromodel)[4](hru,0,0) 
                    ) - (*OpenWQ_vars.chemass)(4)(Sub_mob)(hru,0,0);

                // SOIL LOWER
                (*OpenWQ_vars.d_chemass_dt_transp)(5)(Sub_mob)(hru,0,0) = 
                    (std::fmax(conc_soil_lower_lay[Sub_mob][hru],0.0f)
                    * (*OpenWQ_hostModelconfig.fluxes_hydromodel)[5](hru,0,0) 
                    ) - (*OpenWQ_vars.chemass)(5)(Sub_mob)(hru,0,0);

                // SURFSOIL 
                // (does not multiply by volume of water because CRHM uses mass 
                //for this specific compartment)
                (*OpenWQ_vars.d_chemass_dt_transp)(6)(Sub_mob)(hru,0,0) = 
                    (std::fmax(surfsoil_solub_mWQ_lay[Sub_mob][hru],0.0f)
                    ) - (*OpenWQ_vars.chemass)(6)(Sub_mob)(hru,0,0);
                                            
                
                // GW
                (*OpenWQ_vars.d_chemass_dt_transp)(7)(Sub_mob)(hru,0,0) = 
                    (std::fmax(gw_conc_lay[Sub_mob][hru],0.0f)
                    * (*OpenWQ_hostModelconfig.fluxes_hydromodel)[7](hru,0,0) 
                    ) - (*OpenWQ_vars.chemass)(7)(Sub_mob)(hru,0,0);
                
            }
        }

    }

    // Save time_previous (in seconds) for calculation of kinetics rates in the next timestep
    // Needed because kinetics rates are provided as rate/day
    OpenWQ_wqconfig.time_previous = simtime;

    /* #################################################
    // Loop: Time 
    // Space loop is inside OpenWQ_chem.Run function
    // No space loop for OpenWQ_watertransp.Adv: it needs to be called throughout the host model code
    ################################################# */
    //STASKED//  for (unsigned int ts=0;ts<ts_hosthydromod;ts++){ // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL

    /* ########################################
        Sources and Sinks (doesn't need space loop => it's inside the function)
    ######################################## */            
    
    OpenWQ_sinksource.CheckApply(
        OpenWQ_vars,
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_output,
        Global::year_now,
        Global::month_now,
        Global::day_now,
        Global::hour_now,
        30); // minutes are not available in CRHM (it seems)

    
    /* ########################################
    Transport with water fluxes (No space loop: needs to be called for every water flux)
    ######################################## */ 

    /* (No transport needed - already embeded in CRHM)

    unsigned int source = 1;                 // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
    unsigned int ix_s = 1;                   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
    unsigned int iy_s = 1;                   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
    unsigned int iz_s = 0;                   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
    unsigned int recipient = 2;              // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
    unsigned int ix_r = 1;                   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
    unsigned int iy_r = 1;                   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
    unsigned int iz_r = 0;                   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
    double wflux_s2r = 0.01;                 // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
    double wmass_recipient = 0.1;            // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL

    
    // FUNCTION 1: Just Advection (Land Surface model)
    OpenWQ_watertransp.Adv(
        OpenWQ_json,
        OpenWQ_vars,
        source,
        ix_s, 
        iy_s,
        iz_s,
        recipient,
        ix_r,
        iy_r,
        iz_r,
        wflux_s2r,
        wmass_recipient
        );

    // FUNCTION 2: Advection and Dispersion (Aquatic Systems)
    //OpenWQ_watertransp.AdvDisp(
    //    OpenWQ_json,
    //    OpenWQ_vars);

    //
    //void OpenWQ_watertransp::ChemCompExchange(
    //    OpenWQ_json& OpenWQ_json, 
    //    OpenWQ_vars& OpenWQ_vars, 
    //    int source, std::string kinetics, 
    //    std::vector<std::string> parameter_names, 
    //    std::vector<double> parameter_values,
    //    std::array<double,7> & linedata, 
    //    int & index_chem){

    */

    /* ########################################
    Biogeochemistry (doesn't need space loop => it's inside the function)
    ######################################## */ 

    // Loop needed -> Dependencies to kinetic formulas (needs loop to get hydro model variables
        // that are dependencies to OpenWQ)
    for (unsigned int hru=0;hru<nhru;hru++){
        (*OpenWQ_hostModelconfig.SM_space_hydromodel)(hru,0,0) = soil_rechr[hru]/soil_rechr_max[hru];  // loop needed - Save all SM data from hostmodel at time t
        (*OpenWQ_hostModelconfig.Tair_space_hydromodel)(hru,0,0) = hru_t[hru];  // loop needed - Save all Taair data from hostmodel at time t      
    }

    // Run chemistry
    
    OpenWQ_chem.Run(
        OpenWQ_json,
        OpenWQ_vars,
        OpenWQ_wqconfig,
        OpenWQ_hostModelconfig,
        OpenWQ_output);


    /* ########################################
    Solver 
    (needs to be inside the time loop, but outside the space loop
    Only place where the state-variables are changed
    ######################################## */ 

    OpenWQ_solver.Numerical_Solver(
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_vars);

    /* ########################################
        Output Results
    ######################################## */

    // Only print if time to print -> Needs to be adapted to host model time conventions
    // Note that OpenWQ_wqconfig.timetep_out converted to seconds in OpenWQ_readjson    

    // Print/Save results
    // Return if not time to print
    if (simtime >= OpenWQ_wqconfig.nexttime_out){

        // Get compartment volumes for calculation of concentration (if requested)
        // neet to convert from mm (CRHM native units) to m3 (OpenWQ native units)

        // Call main printing function
        OpenWQ_output.writeResults(
            OpenWQ_json,
            OpenWQ_vars,
            OpenWQ_hostModelconfig,
            OpenWQ_wqconfig,
            OpenWQ_solver,
            simtime);  // needs to be in seconds since 00:00 hours, Jan 1, 1970 UTC

    }
    
    //}

    // ################################
    // Save the arma::variable results back to CRHM's variables (for each compartment)
    // Convert from mass (default unit in OpenWQ) to concentrations (default unit of CRHM-WQ)
    // ################################ 
    
    // Water volume conversions
        // CRHM water units are in mm
        // CRHM area units are in km2
        // So conversion of mm (water) to m3 is calculated as follows:
        // water_m3 = (water_mm / 1000) * (area_km2 * 1000000) <=>
        // water_m3 = water_mm * area_km2 * 1000


    #pragma omp parallel for private(Sub_mob) collapse(2) num_threads(OpenWQ_wqconfig.num_threads_requested)
    for(unsigned int hru = 0; hru < nhru; ++hru) {

        for(unsigned int Sub_i = 0; Sub_i < mobile_species.size(); ++Sub_i) {

                // Get mobile species indexes
                // Only update mass/concentrations using CRHM's water for mobile species
                Sub_mob = mobile_species[Sub_i];

            // SWE
            if ((*OpenWQ_hostModelconfig.fluxes_hydromodel)[0](hru,0,0) 
                    > OpenWQ_hostModelconfig.watervol_minlim){

                SWE_conc_lay[Sub_mob][hru] = 
                    (*OpenWQ_vars.chemass)(0)(Sub_mob)(hru,0,0)
                    / (*OpenWQ_hostModelconfig.fluxes_hydromodel)[0](hru,0,0);
            
            }else{
                SWE_conc_lay[Sub_mob][hru] = 0.0f;
            }

            // RUNOFF
            if ((*OpenWQ_hostModelconfig.fluxes_hydromodel)[1](hru,0,0) 
                    > OpenWQ_hostModelconfig.watervol_minlim){

                soil_runoff_cWQ_lay[Sub_mob][hru] = 
                    (*OpenWQ_vars.chemass)(1)(Sub_mob)(hru,0,0)
                    / (*OpenWQ_hostModelconfig.fluxes_hydromodel)[1](hru,0,0);
            
            }else{
                soil_runoff_cWQ_lay[Sub_mob][hru] = 0.0f;
            }

            // SSR
            if ((*OpenWQ_hostModelconfig.fluxes_hydromodel)[2](hru,0,0) 
                    > OpenWQ_hostModelconfig.watervol_minlim){

                soil_ssr_conc_lay[Sub_mob][hru]= 
                    (*OpenWQ_vars.chemass)(2)(Sub_mob)(hru,0,0)
                    / (*OpenWQ_hostModelconfig.fluxes_hydromodel)[2](hru,0,0);
            
            }else{
                soil_ssr_conc_lay[Sub_mob][hru] = 0.0f;
            }

            // SD
            if ((*OpenWQ_hostModelconfig.fluxes_hydromodel)[3](hru,0,0) 
                    > OpenWQ_hostModelconfig.watervol_minlim){

                Sd_conc_lay[Sub_mob][hru] = 
                    (*OpenWQ_vars.chemass)(3)(Sub_mob)(hru,0,0)
                    / (*OpenWQ_hostModelconfig.fluxes_hydromodel)[3](hru,0,0);
            
            }else{
                Sd_conc_lay[Sub_mob][hru] = 0.0f;
            }

            // SOIL_RECHR
            if ((*OpenWQ_hostModelconfig.fluxes_hydromodel)[4](hru,0,0) 
                    > OpenWQ_hostModelconfig.watervol_minlim){

                conc_soil_rechr_lay[Sub_mob][hru] = 
                    (*OpenWQ_vars.chemass)(4)(Sub_mob)(hru,0,0)
                    / (*OpenWQ_hostModelconfig.fluxes_hydromodel)[4](hru,0,0);
            
            }else{
                conc_soil_rechr_lay[Sub_mob][hru] = 0.0f;
            }

            // SOIL LOWER
            if ((*OpenWQ_hostModelconfig.fluxes_hydromodel)[5](hru,0,0) 
                    > OpenWQ_hostModelconfig.watervol_minlim){

                conc_soil_lower_lay[Sub_mob][hru] = 
                    (*OpenWQ_vars.chemass)(5)(Sub_mob)(hru,0,0)
                    / (*OpenWQ_hostModelconfig.fluxes_hydromodel)[5](hru,0,0);;
            
            }else{
                conc_soil_lower_lay[Sub_mob][hru] = 0.0f;
            }
            
            // SURFSOIL 
            // (does not divide by volume of water because CRHM uses mass 
            //for this specific compartment)
            surfsoil_solub_mWQ_lay[Sub_mob][hru] = (*OpenWQ_vars.chemass)(6)(Sub_mob)(hru,0,0);
            
            // GW
            if ((*OpenWQ_hostModelconfig.fluxes_hydromodel)[7](hru,0,0) 
                    > OpenWQ_hostModelconfig.watervol_minlim){

                gw_conc_lay[Sub_mob][hru] = 
                    (*OpenWQ_vars.chemass)(7)(Sub_mob)(hru,0,0)
                    / (*OpenWQ_hostModelconfig.fluxes_hydromodel)[7](hru,0,0);
            
            }else{
                gw_conc_lay[Sub_mob][hru] = 0.0f;
            }

        }
    }

}

