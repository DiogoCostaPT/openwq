
// ADE solver

#include "ADE_solver.h"

void ADE_solver(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar,int &icmpMob,int &ichem,
    arma::cube& wfluxC_x,arma::cube& wfluxC_y,arma::cube& wfluxC_z,arma::cube& wmassC){

    double mfluxL,frac;
    int nx = JSONfiles.H2O[std::to_string(icmpMob+1)]["nx"];
    int ny = JSONfiles.H2O[std::to_string(icmpMob+1)]["ny"];
    int nz = JSONfiles.H2O[std::to_string(icmpMob+1)]["nz"];

    for (int ix=0;ix<nx;ix++){
        for (int iy=0;iy<ny;iy++){
            for (int iz=0;iz<nz;iz++){
                
                mfluxL = (*Prj_StateVar.chemass)(icmpMob)(ichem)(ix,iy,iz) * (wfluxC_x(ix,iy,iz) + wfluxC_y(ix,iy,iz) 
                    + wfluxC_z(ix,iy,iz))/wmassC(ix,iy,iz);
                if (mfluxL < 0.0f){ // limit flux to available material
                    mfluxL = (*Prj_StateVar.chemass)(icmpMob)(ichem)(ix,iy,iz);
                    frac = wmassC(ix,iy,iz) / (wfluxC_x(ix,iy,iz) + wfluxC_y(ix,iy,iz) + wfluxC_z(ix,iy,iz));
                }else {
                    frac = 1.0f;
                }
                (*Prj_StateVar.chemass)(icmpMob)(ichem)(ix,iy,iz) -= frac * mfluxL;
                
                // x-dir
                if (wfluxC_x(ix,iy,iz) > 0.0f && ix<(nx-1)){
                    (*Prj_StateVar.chemass)(icmpMob)(ichem)(ix+1,iy,iz) += (*Prj_StateVar.chemass)(icmpMob)(ichem)(ix,iy,iz) * frac * wfluxC_x(ix,iy,iz)/wmassC(ix,iy,iz);
                }else if(wfluxC_x(ix,iy,iz) < 0.0f && ix>0)
                {
                    (*Prj_StateVar.chemass)(icmpMob)(ichem)(ix-1,iy,iz) -= (*Prj_StateVar.chemass)(icmpMob)(ichem)(ix,iy,iz) * frac *  wfluxC_x(ix,iy,iz)/wmassC(ix,iy,iz);
                }

                // y-dir
                if (wfluxC_y(ix,iy,iz) > 0.0f && iy<(ny-1)){
                    (*Prj_StateVar.chemass)(icmpMob)(ichem)(ix,iy+1,iz) += (*Prj_StateVar.chemass)(icmpMob)(ichem)(ix,iy,iz) * frac * wfluxC_y(ix,iy,iz)/wmassC(ix,iy,iz);
                }else if(wfluxC_y(ix,iy,iz) < 0.0f && iy>0)
                {
                    (*Prj_StateVar.chemass)(icmpMob)(ichem)(ix,iy-1,iz) -= (*Prj_StateVar.chemass)(icmpMob)(ichem)(ix,iy,iz) * frac * wfluxC_y(ix,iy,iz)/wmassC(ix,iy,iz);
                }

                // z-dir
                if (wfluxC_z(ix,iy,iz) > 0.0f && iz<(nz-1)){
                    (*Prj_StateVar.chemass)(icmpMob)(ichem)(ix,iy,iz+1) += (*Prj_StateVar.chemass)(icmpMob)(ichem)(ix,iy,iz) * frac * wfluxC_z(ix,iy,iz)/wmassC(ix,iy,iz);
                }else if(wfluxC_z(ix,iy,iz) < 0.0f && iz>0)
                {
                    (*Prj_StateVar.chemass)(icmpMob)(ichem)(ix,iy,iz-1) -= (*Prj_StateVar.chemass)(icmpMob)(ichem)(ix,iy,iz) * frac * wfluxC_z(ix,iy,iz)/wmassC(ix,iy,iz);
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