
#include "transp_solver.h"

// main solver
void transp_solve(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar){

/*
    int nx ,ny, nz;
    int dx, dy, dz;
    double disp_x, disp_y, disp_z;
    arma::cube conc;
    arma::cube dcdx,dcdy,dcdz;
    arma::cube dc2dx2,dc2dy2,dc2dz2;
    arma::cube dmdt;
    arma::cube wmassC;
    arma::cube wfluxC_x, wfluxC_y, wfluxC_z;
    
    int numcmp = JSONfiles.H2O["compartments"].size();
    numspec = JSONfiles.BGC["compartments"][std::to_string(i+1)]["chem_species"].size();

    for (int icmp=1;icmp<numcmp;icmp++){
        
        (*Prj_StateVar.wflux)(icmp);
        

        for (int ichem=1;ichem<numspec;ichem++){

                wmassC = (*Prj_StateVar.chemass)(icmp)(ichem);

            for (int ix=1;ix<nx;ix++){
                for (int iy=1;iy<ny;iy++){
                    for (int iz=1;iz<nz;iz++){
                    
                        dcdx(ix,iy,iz)=(conc_0(ix,iy,iz)-conc_0(ix-1,iy,iz))/dx;
                        dcdy(ix,iy,iz)=(conc_0(ix,iy,iz)-conc_0(ix,iy-1,iz))/dy;
                        dcdz(ix,iy,iz)=(conc_0(ix,iy,iz)-conc_0(ix,iy,iz-1))/dz;

                        dc2dx2(ix,iy,iz)=((conc_0(ix+1,iy,iz)-conc_0(ix,iy,iz))/dx
                            -(conc_0(ix,iy,iz)-conc_0(ix-1,iy,iz))/dx)/(dx);
                        dc2dy2(ix,iy,iz)=((conc_0(ix,iy+1,iz)-conc_0(ix,iy,iz))/dy
                            -(conc_0(ix,iy,iz)-conc_0(ix,iy-1,iz))/dy)/(dy);
                        dc2dz2(ix,iy,iz)=((conc_0(ix,iy,iz+1)-conc_0(ix,iy,iz))/dz
                            -(conc_0(ix,iy,iz)-conc_0(ix,iy,iz-1))/dz)/(dz);

                        dmdt(ix,iy,iz)= -(wflux_x)(il,ih)*dcdx(il,ih)
                            -(wfluxC_x)(il,ih)*dcdy(il,ih)
                            -(wfluxC_y)(il,ih)*dcdz(il,ih)
                            +disp_x*dc2dx2(il,ih)
                            +disp_y*dc2dy2(il,ih)
                            +disp_z*dc2dz2(il,ih);

                        dmdt(ix,iy,iz) = std::fmin(dmdt(ix,iy,iz), chemass(ix,iy,iz))

                        conc(ix,iy,iz) = conc(ix,iy,iz) + dmdt(ix,iy,iz)/wmassC(ix,iy,iz);

                    }
                }
            }
        }
    }
    */
    
}