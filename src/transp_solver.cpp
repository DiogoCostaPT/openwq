
#include "transp_solver.h"

// main solver
void transp_solve(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar){


    int nx ,ny, nz;
    int dx, dy, dz;
    int numspec;
    arma::cube conc;
    arma::cube dcdx,dcdy,dcdz;
    arma::cube dc2dx2,dc2dy2,dc2dz2;
    arma::cube dmdt;
    arma::cube wmassC,chemassC;
    arma::cube wfluxC_x, wfluxC_y, wfluxC_z;
    double mfluxL,frac;
    bool mobile;
    std::string folder_path; 
    std::vector<std::vector<std::string>> filenames;
    
    int numcmp = JSONfiles.H2O["compartments"].size();
    double disp_x = JSONfiles.BGC["dispersion"]["x-dir"];
    double disp_y = JSONfiles.BGC["dispersion"]["y-dir"];
    double disp_z = JSONfiles.BGC["dispersion"]["z-dir"];


    // Get fluxes
    //readSetFluxes(JSONfiles,Prj_StateVar);

    // Get fluxes files
    for (int icmp=1;icmp<=numcmp;icmp++){
        
        mobile = JSONfiles.H2O[std::to_string(icmp)]["mobile"];
        std::vector<std::string> filenames_i;

        if (mobile){
            folder_path = JSONfiles.H2O[std::to_string(icmp)]["water_fluxes_files"]["folder_path"];
            GetFluxesFiles(folder_path,filenames_i,icmp); // list the results files to get the last time step
            
        }else{
            filenames_i.push_back("NOT_MOBILE");
        }
        filenames.push_back(filenames_i);
    }
  
    
    // ADE solver
    for (int icmp=0;icmp<numcmp;icmp++){
        
        mobile = JSONfiles.H2O[std::to_string(icmp)]["mobile"];
        if(!mobile) continue; // skip if compartment is not mobile

        nx = JSONfiles.H2O[std::to_string(icmp+1)]["nx"];
        ny = JSONfiles.H2O[std::to_string(icmp+1)]["ny"];
        nz = JSONfiles.H2O[std::to_string(icmp+1)]["nz"];

        numspec = JSONfiles.BGC["compartments"][std::to_string(icmp+1)]["chem_species"].size();
        wfluxC_x = (*Prj_StateVar.wflux)(icmp)(0);
        wfluxC_y = (*Prj_StateVar.wflux)(icmp)(1);
        wfluxC_z = (*Prj_StateVar.wflux)(icmp)(2);
        wmassC = (*Prj_StateVar.wmass)(icmp);
       
        for (int ichem=0;ichem<numspec;ichem++){

            chemassC = (*Prj_StateVar.chemass)(icmp)(ichem);

            for (int ix=1;ix<nx-1;ix++){
                for (int iy=1;iy<ny-1;iy++){
                    for (int iz=1;iz<nz-1;iz++){
                        
                        mfluxL = (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy,iz) * (wfluxC_x(ix,iy,iz) + wfluxC_y(ix,iy,iz) 
                        + wfluxC_z(ix,iy,iz))/wmassC(ix,iy,iz);
                        if (mfluxL < 0.0f){ // limit flux to available material
                            mfluxL = (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy,iz);
                            frac = wmassC(ix,iy,iz) / (wfluxC_x(ix,iy,iz) + wfluxC_y(ix,iy,iz) + wfluxC_z(ix,iy,iz));
                        }else {
                            frac = 1.0f;
                        }
                        (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy,iz) -= frac * mfluxL;
                        
                        // x-dir
                        if (wfluxC_x(ix,iy,iz) > 0.0f){
                            (*Prj_StateVar.chemass)(icmp)(ichem)(ix+1,iy,iz) += (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy,iz) * wfluxC_x(ix,iy,iz)/wmassC(ix,iy,iz);
                        }else
                        {
                            (*Prj_StateVar.chemass)(icmp)(ichem)(ix-1,iy,iz) -= (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy,iz) * wfluxC_x(ix,iy,iz)/wmassC(ix,iy,iz);
                        }

                         // y-dir
                        if (wfluxC_y(ix,iy,iz) > 0.0f){
                            (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy+1,iz) += (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy,iz) * wfluxC_y(ix,iy,iz)/wmassC(ix,iy,iz);
                        }else
                        {
                            (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy-1,iz) -= (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy,iz) * wfluxC_y(ix,iy,iz)/wmassC(ix,iy,iz);
                        }

                         // z-dir
                        if (wfluxC_z(ix,iy,iz) > 0.0f){
                            (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy,iz+1) += (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy,iz) * wfluxC_z(ix,iy,iz)/wmassC(ix,iy,iz);
                        }else
                        {
                            (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy,iz-1) -= (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy,iz) * wfluxC_z(ix,iy,iz)/wmassC(ix,iy,iz);
                        }
                        
                        // MISSING DISPERSION (NOT VALIDATED YET)
                        
                        /*
                        dcdx(ix,iy,iz)=(conc(ix,iy,iz)-conc(ix-1,iy,iz))/dx;
                        dcdy(ix,iy,iz)=(conc(ix,iy,iz)-conc(ix,iy-1,iz))/dy;
                        dcdz(ix,iy,iz)=(conc(ix,iy,iz)-conc(ix,iy,iz-1))/dz;

                        dc2dx2(ix,iy,iz)=((conc(ix+1,iy,iz)-conc(ix,iy,iz))/dx
                            -(conc(ix,iy,iz)-conc(ix-1,iy,iz))/dx)/(dx);
                        dc2dy2(ix,iy,iz)=((conc(ix,iy+1,iz)-conc(ix,iy,iz))/dy
                            -(conc(ix,iy,iz)-conc(ix,iy-1,iz))/dy)/(dy);
                        dc2dz2(ix,iy,iz)=((conc(ix,iy,iz+1)-conc(ix,iy,iz))/dz
                            -(conc(ix,iy,iz)-conc(ix,iy,iz-1))/dz)/(dz);

                        dmdt(ix,iy,iz)= -(wfluxC_x)(ix,iy,iz)*dcdx(ix,iy,iz)
                            -(wfluxC_y)(ix,iy,iz)*dcdy(ix,iy,iz)
                            -(wfluxC_z)(ix,iy,iz)*dcdz(ix,iy,iz)
                            +disp_x*dc2dx2(ix,iy,iz)
                            +disp_y*dc2dy2(ix,iy,iz)
                            +disp_z*dc2dz2(ix,iy,iz);

                        dmdt(ix,iy,iz) = std::fmin(dmdt(ix,iy,iz), chemassC(ix,iy,iz));

                        chemassC(ix,iy,iz) = chemassC(ix,iy,iz) + dmdt(ix,iy,iz);
                        */

                    }
                }
            }
        }
    }

    
}