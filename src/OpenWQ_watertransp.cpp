

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


#include "OpenWQ_watertransp.h"

/* #################################################
// Mass transport
// Only Advection
// General case (flux exchanges within the model domain)
################################################# */
void OpenWQ_watertransp::Adv(
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    const int source, const int ix_s, const int iy_s, const int iz_s,
    const int recipient, const int ix_r, const int iy_r, const int iz_r,
    double wflux_s2r, 
    double wmass_source){

    double chemass_flux_adv;
    unsigned int ichem_mob;

    // CHANGE THE LINE BELOW: see my notes -> there should be no icmp because all compartments should have the same number of mobile species
    unsigned int numspec = OpenWQ_wqconfig.BGC_general_mobile_species.size();

    // Loop for mobile chemical species
    for (unsigned int chemi=0;chemi<numspec;chemi++){

        // mobile chemical species index
        ichem_mob = OpenWQ_wqconfig.BGC_general_mobile_species[chemi];

        // Chemical mass flux between source and recipient (Advection)
        chemass_flux_adv = 
            wflux_s2r
            * (*OpenWQ_vars.chemass)(source)(ichem_mob)(ix_s,iy_s,iz_s) // concentration calculation
             / wmass_source;
        
        //##########################################
        // Set derivative for source and recipient 
        
        // Remove Chemical mass flux from SOURCE 
        (*OpenWQ_vars.d_chemass_dt_transp)(source)(ichem_mob)(ix_s,iy_s,iz_s) 
            -= chemass_flux_adv;

        // Add Chemical mass flux to RECIPIENT 
        // if recipient == -1, then  it's an OUT-flux (loss from system)
        if (recipient == -1) continue;
        (*OpenWQ_vars.d_chemass_dt_transp)(recipient)(ichem_mob)(ix_r,iy_r,iz_r) 
            += chemass_flux_adv;
    }
                
}

/* #################################################
// Mass transport
// Only Advection
// Special case (in-fluxes from external water flux sources)
################################################# */
void Adv_IN(
    OpenWQ_vars& OpenWQ_vars, 
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    const int recipient, const int ix_r, const int iy_r, const int iz_r,
    double wflux_s2r,   // water flux (m3/s)
    int chemid,    // chemical id
    double ewf_conc){    // concentration of chemical

    // Local variables
    double chemass_flux_adv;

    // Chemical mass flux between source and recipient (Advection)
    chemass_flux_adv = 
        wflux_s2r
        * ewf_conc;
        
    //##########################################
    // Set derivative for source and recipient 
    
    // Add Chemical mass flux to RECIPIENT 
    (*OpenWQ_vars.d_chemass_dt_transp)(recipient)(chemid)(ix_r,iy_r,iz_r) 
        += chemass_flux_adv;

}

/* #################################################
// Mass transport
// Advection & Dispersion
################################################# */
void OpenWQ_watertransp::AdvDisp(
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    const int source, const int ix_s, const int iy_s, const int iz_s,
    const int recipient, const int ix_r, const int iy_r, const int iz_r,
    double wflux_s2r, 
    double wmass_source){

    double chemass_flux_adv;
    unsigned int ichem_mob;

    // CHANGE THE LINE BELOW: see my notes -> there should be no icmp because all compartments should have the same number of mobile species
    unsigned int numspec = OpenWQ_wqconfig.BGC_general_mobile_species.size();

    // Loop for mobile chemical species
    for (unsigned int chemi=0;chemi<numspec;chemi++){

        // mobile chemical species index
        ichem_mob = OpenWQ_wqconfig.BGC_general_mobile_species[chemi];

        // Chemical mass flux between source and recipient (Advection)
        chemass_flux_adv = 
            wflux_s2r
            * (*OpenWQ_vars.chemass)(source)(ichem_mob)(ix_s,iy_s,iz_s) // concentration calculation
             / wmass_source;
        
        // dcdy(xi,yi,t)=(c(xi,yi,t-1)-c(xi,yi-1,t-1))/delty;
        // dc2dy2(xi,yi,t)=((c(xi,yi+1,t-1)-c(xi,yi,t-1))/delty-(c(xi,yi,t-1)-c(xi,yi-1,t-1))/delty)/(delty);         
        // dc2dx2(xi,yi,t)=((c(xi+1,yi,t-1)-c(xi,yi,t-1))/deltx-(c(xi,yi,t-1)-c(xi-1,yi,t-1))/deltx)/(deltx);
        // dcdt(xi,yi,t)=-U*dcdy(xi,yi,t)+Di(xi,yi,t)*(dc2dx2(xi,yi,t)+dc2dy2(xi,yi,t));

        //##########################################
        // Set derivative for source and recipient 
        
        // Remove Chemical mass flux from SOURCE 
        (*OpenWQ_vars.d_chemass_dt_transp)(source)(ichem_mob)(ix_s,iy_s,iz_s) 
            -= chemass_flux_adv;

        // Add Chemical mass flux to RECIPIENT
        // if recipient == -1, then  it's an OUT-flux (loss from system)
        if (recipient == -1) continue;
        (*OpenWQ_vars.d_chemass_dt_transp)(recipient)(ichem_mob)(ix_r,iy_r,iz_r) 
            += chemass_flux_adv;
    }
                
}


/* #################################################
// Internal mobilization of immobile pools
// Erosion and weathering
################################################# */
void OpenWQ_watertransp::IntMob(
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    const int source, const int ix_s, const int iy_s, const int iz_s,
    const int recipient, const int ix_r, const int iy_r, const int iz_r,
    double wflux_s2r, 
    double wmass_source){

    // Internal Variables
    double chemass_flux;

    // Loop for immobile chemical species
    for (unsigned int chemi=0;chemi<OpenWQ_wqconfig.BGC_general_num_chem;chemi++){

        //##########################################
        // Check if the chem species is mobile
        // Skip if mobile (mobile species are moved by OpenWQ_watertransp::AdvDisp)
        std::vector<unsigned int>::iterator itr 
            = std::find(
                OpenWQ_wqconfig.BGC_general_mobile_species.begin(), 
                OpenWQ_wqconfig.BGC_general_mobile_species.end(), 
                chemi);

        if (itr != end(OpenWQ_wqconfig.BGC_general_mobile_species))
            continue;

        //##########################################
        // Chemical mass flux between source and recipient 
        chemass_flux = 
            OpenWQ_wqconfig.OpenWQ_TE_native_IntMob_Erodib_K[source]
            * wflux_s2r
            * (*OpenWQ_vars.chemass)(source)(chemi)(ix_s,iy_s,iz_s);
        
        
        //##########################################
        // Set derivative for source and recipient 
        
        // Remove Chemical mass flux from SOURCE 
        (*OpenWQ_vars.d_chemass_dt_transp)(source)(chemi)(ix_s,iy_s,iz_s) 
            -= chemass_flux;

        // Add Chemical mass flux to RECIPIENT
        // if recipient == -1, then  it's an OUT-flux (loss from system)
        if (recipient == -1) continue;
        (*OpenWQ_vars.d_chemass_dt_transp)(recipient)(chemi)(ix_r,iy_r,iz_r) 
            += chemass_flux;
    }

}


/* #################################################
// Boundary Mixing due to velocity gradients
// due to turbulence and cross-boarder eddies
################################################# */
void OpenWQ_watertransp::BoundMix(
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    const int source, const int ix_s, const int iy_s, const int iz_s,
    const int recipient, const int ix_r, const int iy_r, const int iz_r,
    double wflux_s2r, 
    double wmass_source){
    
    
    // Local Variables
    double chemass_exchange_upper_comprt;
    double chemass_exchange_lower_comprt;
    unsigned int chemi_mob;             // interactive index of mobile species
    std::vector<unsigned int> xyz_source;
    xyz_source.push_back(ix_s);
    xyz_source.push_back(iy_s);
    xyz_source.push_back(iz_s);         // get x,y,z from source comparment in vector

    // Dummy/interactive variables
    unsigned int input_direction_index;
    int input_upper_compartment_index;
    int input_lower_compartment_index;
    double input_k_val;
    unsigned int index_lower_cell;     // index of lower cell for mass exchange
    std::vector<int> xyz_upper_compartment;

    // Return if flux across compartments
    // This lateral mixing only occurs when fluxes occur when fluxes along the 
    // interface of compartments
    if (source != recipient)
        return;


    for (unsigned int entry_i = 0; entry_i < OpenWQ_wqconfig.OpenWQ_TE_native_BoundMix_info.size(); entry_i++){

        xyz_upper_compartment.clear();

        // Get inputs of entry
        input_direction_index = std::get<0>(OpenWQ_wqconfig.OpenWQ_TE_native_BoundMix_info[entry_i]);
        input_upper_compartment_index = std::get<1>(OpenWQ_wqconfig.OpenWQ_TE_native_BoundMix_info[entry_i]);
        input_lower_compartment_index = std::get<2>(OpenWQ_wqconfig.OpenWQ_TE_native_BoundMix_info[entry_i]);
        input_k_val = std::get<3>(OpenWQ_wqconfig.OpenWQ_TE_native_BoundMix_info[entry_i]);

        // Num of cells in upper_compartment
        xyz_upper_compartment.push_back(std::get<2>(OpenWQ_hostModelconfig.HydroComp.at(input_upper_compartment_index)));
        xyz_upper_compartment.push_back(std::get<3>(OpenWQ_hostModelconfig.HydroComp.at(input_upper_compartment_index)));
        xyz_upper_compartment.push_back(std::get<4>(OpenWQ_hostModelconfig.HydroComp.at(input_upper_compartment_index)));

        // Ignore if entry is not applicable to the current source compartment
        if (input_upper_compartment_index != source)
            continue;

        // Get number of cells in input_direction_index
        index_lower_cell = xyz_upper_compartment[input_direction_index] - 1;

        // Ignore if current source cell is not the lower adjacent cell where mixing may occur
        if (xyz_source[input_direction_index] != index_lower_cell)
            continue;


        // Loop over mobile species
        for (unsigned int chemi=0;chemi<OpenWQ_wqconfig.BGC_general_mobile_species.size();chemi++){
   
            // Get index of mobile species
            chemi_mob = OpenWQ_wqconfig.BGC_general_mobile_species[chemi];
            
            //##########################################
            // Chemical exxhange between upper and lower compartments
            chemass_exchange_upper_comprt = 
                fmin(
                    input_k_val
                    * wflux_s2r
                    * (*OpenWQ_vars.chemass)(input_upper_compartment_index)(chemi_mob)(ix_s,iy_s,iz_s),
                    (*OpenWQ_vars.chemass)(input_upper_compartment_index)(chemi_mob)(ix_s,iy_s,iz_s));
            
            chemass_exchange_lower_comprt = 
                fmin(
                    input_k_val
                    * wflux_s2r
                    * (*OpenWQ_vars.chemass)(input_lower_compartment_index)(chemi_mob)(ix_s,iy_s,iz_s),
                    (*OpenWQ_vars.chemass)(input_lower_compartment_index)(chemi_mob)(ix_s,iy_s,iz_s));
                
            //##########################################
            // Set derivative for source and recipient 
            
            // Remove Chemical mass flux from SOURCE 
            (*OpenWQ_vars.d_chemass_dt_transp)(input_upper_compartment_index)(chemi_mob)(ix_s,iy_s,iz_s) 
                += (chemass_exchange_lower_comprt - chemass_exchange_upper_comprt);

            // Add Chemical mass flux to RECIPIENT
            // if recipient == -1, then  it's an OUT-flux (loss from system)
            if (recipient == -1) continue;
            (*OpenWQ_vars.d_chemass_dt_transp)(input_lower_compartment_index)(chemi_mob)(ix_r,iy_r,iz_r) 
                += (chemass_exchange_upper_comprt - chemass_exchange_lower_comprt);

        }

    }



}

