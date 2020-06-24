
#include "transp_solver.h"

// main solver
void transp_solve(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar){


    int nx ,ny, nz;
    int dx, dy, dz;
    int numspec;
    int icmpMob, tmpst_i;
    arma::cube conc;
    arma::cube dcdx,dcdy,dcdz;
    arma::cube dc2dx2,dc2dy2,dc2dz2;
    arma::cube dmdt;
    arma::cube wmassC,chemassC;
    arma::cube wfluxC_x, wfluxC_y, wfluxC_z;
    double mfluxL,frac;
    bool mobile;
    std::vector<std::vector<std::string>> fluxes_filenames;
    std::vector<std::vector<double>> fluxes_filenames_num;
    std::string fluxes_filenamesExtention;
    std::vector<int> mobileCompt;
    std::string res_folder = JSONfiles.Master["export_results_folder"];
    
    int numcmp = JSONfiles.H2O["compartments"].size();
    double disp_x = JSONfiles.BGC["dispersion"]["x-dir"];
    double disp_y = JSONfiles.BGC["dispersion"]["y-dir"];
    double disp_z = JSONfiles.BGC["dispersion"]["z-dir"];

    // Get fluxes files for each compartment
    GetFluxesFiles(JSONfiles,fluxes_filenames);

    // Convert filename strings to numbers
    ConvertSortFluxesFilenames2Double(JSONfiles,fluxes_filenames,fluxes_filenames_num);

    // Check if time steps match between mobile compartments
    bool timeMatch_flag = CheckIfCompTimeStepsMatch(fluxes_filenames_num,mobileCompt);

    // Save initial conditions
    for (int j=0;j<numcmp;j++){
        writeVTU(JSONfiles,j,Prj_StateVar,0); // https://lorensen.github.io/VTKExamples/site/Cxx/IO/WriteVTU/
    }
    
    // Get Extention of Fluxes files: use 1st mobile compartment as reference and [3]-> because the first 2 are allways ".' and "..""
    if (!mobileCompt.empty()){ // if at least one compartment is mobile

        GetFileExtension(fluxes_filenames[mobileCompt[0]][2],fluxes_filenamesExtention);

        // if all mobile_compartment timesteps match, run ADE_solver
        if (timeMatch_flag){

            int tmpst_num = fluxes_filenames_num[mobileCompt[0]].size(); // num of elements of the 1st mobile compartment
                        
            for (int tmpst=0;tmpst<tmpst_num;tmpst++){ // time loop
                
                tmpst_i = fluxes_filenames_num[mobileCompt[0]][tmpst]; // timestep in file

                readSetFluxes(JSONfiles,Prj_StateVar,mobileCompt,fluxes_filenamesExtention,tmpst_i); // Get all fluxes at timestep tmpst

                for (int icmp=0;icmp<mobileCompt.size();icmp++){ // comparment loop
                    
                    icmpMob = mobileCompt[icmp]; // get mobile compartments
                    
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

                        for (int ix=0;ix<nx;ix++){
                            for (int iy=0;iy<ny;iy++){
                                for (int iz=0;iz<nz;iz++){
                                    
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
                                    if (wfluxC_x(ix,iy,iz) > 0.0f && ix<(nx-1)){
                                        (*Prj_StateVar.chemass)(icmp)(ichem)(ix+1,iy,iz) += (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy,iz) * wfluxC_x(ix,iy,iz)/wmassC(ix,iy,iz);
                                    }else if(ix>0)
                                    {
                                        (*Prj_StateVar.chemass)(icmp)(ichem)(ix-1,iy,iz) -= (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy,iz) * wfluxC_x(ix,iy,iz)/wmassC(ix,iy,iz);
                                    }

                                    // y-dir
                                    if (wfluxC_y(ix,iy,iz) > 0.0f && iy<(ny-1)){
                                        (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy+1,iz) += (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy,iz) * wfluxC_y(ix,iy,iz)/wmassC(ix,iy,iz);
                                    }else if(iy>0)
                                    {
                                        (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy-1,iz) -= (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy,iz) * wfluxC_y(ix,iy,iz)/wmassC(ix,iy,iz);
                                    }

                                    // z-dir
                                    if (wfluxC_z(ix,iy,iz) > 0.0f && iz<(nz-1)){
                                        (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy,iz+1) += (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy,iz) * wfluxC_z(ix,iy,iz)/wmassC(ix,iy,iz);
                                    }else if(iz>0)
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

                // Print Results
            for (int j=0;j<numcmp;j++){
                writeVTU(JSONfiles,j,Prj_StateVar,tmpst_i); // https://lorensen.github.io/VTKExamples/site/Cxx/IO/WriteVTU/
            }

            }
        }
    }

    
}