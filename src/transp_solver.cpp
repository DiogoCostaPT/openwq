
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
    
    int numcmp = JSONfiles.H2O["compartments"].size();
    double disp_x = JSONfiles.BGC["dispersion"]["x-dir"];
    double disp_y = JSONfiles.BGC["dispersion"]["y-dir"];
    double disp_z = JSONfiles.BGC["dispersion"]["z-dir"];
    

    for (int icmp=1;icmp<numcmp;icmp++){
        
        nx = JSONfiles.H2O[std::to_string(icmp+1)]["nx"];
        ny = JSONfiles.H2O[std::to_string(icmp+1)]["ny"];
        nz = JSONfiles.H2O[std::to_string(icmp+1)]["nz"];

        numspec = JSONfiles.BGC["compartments"][std::to_string(icmp)]["chem_species"].size();
        wfluxC_x = (*Prj_StateVar.wflux)(icmp)(0);
        wfluxC_y = (*Prj_StateVar.wflux)(icmp)(1);
        wfluxC_z = (*Prj_StateVar.wflux)(icmp)(2);
        wmassC = (*Prj_StateVar.wmass)(icmp);
        
        for (int ichem=1;ichem<numspec;ichem++){

                chemassC = (*Prj_StateVar.chemass)(icmp)(ichem);
                conc = chemassC / wmassC;

            for (int ix=0;ix<nx;ix++){
                for (int iy=0;iy<ny;iy++){
                    for (int iz=0;iz<nz;iz++){
                        
                        std::cout << chemassC(ix,iy,iz) << std::endl;
                        

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