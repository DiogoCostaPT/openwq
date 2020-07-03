
// ADE solver

#include "ADE_solver.h"

void ADE_solver_1(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar,int &icmp,int &ichem){

    double mfluxL,wflux_intraL,frac;
    int nx = JSONfiles.H2O[std::to_string(icmp+1)]["nx"];
    int ny = JSONfiles.H2O[std::to_string(icmp+1)]["ny"];
    int nz = JSONfiles.H2O[std::to_string(icmp+1)]["nz"];

    //arma::cube wflux_intraC_x = (*Prj_StateVar.wflux_intra)(icmp)(0);
    //arma::cube wflux_intraC_y = (*Prj_StateVar.wflux_intra)(icmp)(1);
    //arma::cube wflux_intraC_z = (*Prj_StateVar.wflux_intra)(icmp)(2);
    //arma::cube wmassC = (*Prj_StateVar.wmass)(icmp);

    arma::mat elemt_plus = arma::zeros<arma::mat>(3,3);
    elemt_plus(0,0) = 1;
    elemt_plus(1,1) = 1;
    elemt_plus(2,2) = 1;
    int dir_plus, idir,n_idir;

    for (int ix=0;ix<nx;ix++){
        for (int iy=0;iy<ny;iy++){
            for (int iz=0;iz<nz;iz++){
                
                for (int dir=0;dir<3;dir++){ // x, y and z directions

                    wflux_intraL = (*Prj_StateVar.wflux_intra)(icmp)(dir)(ix,iy,iz);

                    if ((*Prj_StateVar.wmass)(icmp)(ix,iy,iz) <= 0.0f)
                        continue;

                    // Check water flux
                    if (wflux_intraL==0.0f)  // skip if no flow in dir direction
                        continue;                                 
                    else  // limit fo available material
                        frac = fmax(fmin(wflux_intraL/(*Prj_StateVar.wmass)(icmp)(ix,iy,iz),1.0f),-1.0f);

                    // To support the identification of the element from/t where the flux comes or goes
                    if (frac>0)
                        dir_plus = 1;
                    else
                        dir_plus = -1;
                         
                    
                    // limit to available material
                    wflux_intraL = frac * wflux_intraL;
                    mfluxL = frac * (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy,iz);

                    // Mass balance at x,y,z node
                    (*Prj_StateVar.wmass)(icmp)(ix,iy,iz) -= wflux_intraL;
                    (*Prj_StateVar.chemass)(icmp)(ichem)(ix,iy,iz) -= mfluxL;
                    
                    // Mass balance at respective adjacent (source) node
                    // but skip if the receiving node it outside the domain
                    if (dir==0){
                        idir = ix;
                        n_idir = nx;
                    }else if (dir==1){
                        idir = iy;
                        n_idir = ny;
                    }else{
                        idir = iz;
                        n_idir = nz;
                    }

                    if (idir+dir_plus*elemt_plus(dir,dir)>=n_idir)
                        continue;

                    (*Prj_StateVar.wmass)(icmp)(ix+dir_plus*elemt_plus(dir,0),iy+dir_plus*elemt_plus(dir,1),
                        iz+dir_plus*elemt_plus(dir,2)) += wflux_intraL;
                    (*Prj_StateVar.chemass)(icmp)(ichem)(ix+dir_plus*elemt_plus(dir,0),iy+dir_plus*elemt_plus(dir,1),
                        iz+dir_plus*elemt_plus(dir,2)) += mfluxL;
                }

            }
        }
    }

}

/*
// ADE solver
void ADE_solver_2(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar)
{

    double pfw,pfe,qfs,qfn,ntp, pfce, he,fp,fe,hne, pfde,area,areae,arean,hn,qxl,qyl,fw,
       fee,fs,fn,fnn,hnue,fem,hnn,qfcn,qfdn,fnm, cvolrate,cf,cbilan,dc,cvolpot,cvolrat,con, hnew;
    long unsigned int ix,iy,a;
    double dx,dy,hp,ie,iee,in, inn, is,iw;
    double nt =1 ; // eddy viscosity (m2/s) = 1,
    double sigc = 0.5;

    arma::mat cmaxr(ds.MROWS,ds.MCOLS); //double  cmaxr(0:mx,0:my)
    arma::mat cminr(ds.MROWS,ds.MCOLS); //cminr(0:mx,0:my);
    arma::mat qfcds(ds.MROWS*ds.MCOLS,1);  //double qfcds(0:mx);
    arma::mat con_step(ds.MROWS,ds.MCOLS);  //double qfcds(0:mx);


    if(it>1) {
    // ADJUST CONCENTRATION TO NEW DEPTH
        for (a=1;a<=ds.NCOLS*ds.NROWS;a++) {
            iy= ((a-1)/ds.NROWS)+1;
            ix=a-ds.NROWS*(iy-1);

            cmaxr(ix,iy)=std::max((*ds.conc_SW)(ix-1,iy),std::max((*ds.conc_SW)(ix+1,iy),std::max((*ds.conc_SW)(ix,iy-1),(*ds.conc_SW)(ix,iy+1))));
            cminr(ix,iy)=std::min((*ds.conc_SW)(ix-1,iy),std::min((*ds.conc_SW)(ix+1,iy),std::min((*ds.conc_SW)(ix,iy-1),(*ds.conc_SW)(ix,iy+1))));
            hnew=(*ds.h)(ix,iy);
            
            if((*ds.ldry)(ix,iy)==0 && (*ds.ldry_prev)(ix,iy)==0) 
            {
                (*ds.conc_SW)(ix,iy)=(*ds.conc_SW)(ix,iy)*(*ds.h0)(ix,iy)/hnew;
            } else if ((*ds.ldry)(ix,iy)==1) 
            {
                (*ds.conc_SW)(ix,iy) = 0.0f;
            }
        }
    }else
    {
        return;
    }
            
    //...    POLLUTION SOURCES
    //$OMP PARALLEL
    //if (isqes2_on==1) call isqes2            // instantenous
    //if (csqes2_on==1) call csqes2          // continuous
    //if (gusqes2_on==1) call gusqes2        // gullies
    //if (gusqes2_on==1) call gusqes2_buildup
    //if (load_u_src>0) call usqes2                      // uniform
    //$OMP END PARALLEL

    dx=ds.dxy;
    dy = ds.dxy;
    //dyn=ds.dxy; 

    // SPACE LOOP
    for (a=1;a<ds.NCOLS*ds.NROWS;a++) {

        iy= ((a-1)/ds.NROWS)+1;
        ix=a-ds.NROWS*(iy-1);

        is=iy-1; 
        in=iy+1; 
        inn=std::min(iy+2,ds.NCOLS+1);
        iw=ix-1;
        ie=ix+1;
        iee=std::min(ix+2,ds.NROWS+1);
                     
        //  BC 
        if (ix==1) {
            pfce=(*ds.conc_SW)(0,iy)*(*ds.fe_1)(0,iy)*dy;     // convective flux
            hp=std::max((*ds.h)(1,iy),ds.hdry);                  
            he=std::max((*ds.h)(2,iy),ds.hdry);
            fp=(*ds.conc_SW)(0,iy);
            fe=(*ds.conc_SW)(1,iy);
           
            hne=std::sqrt(hp*nt*he*nt)/sigc/std::abs(dx)*dy*ds.D_coef;
            pfde=0.;            // no diffusive flux over boundary
            pfe=pfce;  
        }  

        // CHECK IF THE DOMAIN IS DRY
        if((*ds.ldry)(ix,iy)==1){
            pfe=0.;
            qfcds(ix)=0.;
            (*ds.conc_SW)(ix,iy)=0.;            
            continue; 
        };

        // INITIALIZATION 
        area=ds.arbase;
        areae=ds.arbase;
        arean=ds.arbase;
        ntp = nt; 
        hp=(*ds.h)(ix,iy); 
        he=(*ds.h)(ie,iy);
        hn=(*ds.h)(ix,in);
        qxl=(*ds.fe_1)(ix,iy);
        qyl=(*ds.fn_1)(ix,iy);
        fw=(*ds.conc_SW)(iw,iy);
        fp=(*ds.conc_SW)(ix,iy);
        fe=(*ds.conc_SW)(ie,iy);
        fee=(*ds.conc_SW)(iee,iy);
        fs=(*ds.conc_SW)(ix,is);
        fn=(*ds.conc_SW)(ix,in);
        fnn=(*ds.conc_SW)(ix,inn);
            
        // FLUXES OVER WEST AND SOUTH FACES (from previous interaction)
        pfw=pfe; 
        qfs=qfcds(ix);


        // X-DIRECTION
        // diffusive flux and mean concentration at east face
        if((*ds.ldry)(ie,iy)==0) {
            hnue=std::max(hp*nt*he*nt,.0001); 
            hne=std::sqrt(hnue)/sigc/dx*dy*ds.D_coef; 
            pfde=-hne*(fe-fp);                     // diffusive flux

            if(qxl>0.0f){
                if ((*ds.ldry)(iw,iy)==0) {
                   fem=-.125*fw+.75*fp+.375*fe;
                }else {
                   fem=0.5*fp+0.5*fe;
                }             
            } else{
                if ((*ds.ldry)(iee,iy)==0) {
                  fem=.375*fp+.75*fe-.125*fee;
                }else {
                  fem=0.5*fp+0.5*fe;   
                }
            }
        }else {
            fem=0.;
            pfde=0.;
        }

        fem=std::max(0.,fem);

        if(ix==ds.NROWS){  // if Boundary (overwrite the BC)
            fem=(*ds.conc_SW)(ds.NROWS+1,iy);
        }

        // advective flux - X-direction  - [m3/s]   
        pfce=qxl*fem*dy;  
        
        // total flux = advective flux + diffusive
        pfe=pfce+pfde;      
        
        // check available material if coming from the east cell
        if(pfe<0){ 
            if((*ds.ldry)(ie,iy)==0)    {
                cvolrate=-(fe*he)*areae/ds.dtfl; 
                pfe=std::max(pfe,cvolrate); //limit to available material
            }else {
                pfe=0.;
            }
        }             

        // Y-DIRECTION
        // diffusion at the present time step Y-direction (pfde, where "d" refers to diffusion)
        if((*ds.ldry)(ix,in)==0)           {
            hnue=std::max(.0001,hp*ntp*hn*nt);
            hnn=std::sqrt(hnue)/sigc/dy*dx*ds.D_coef;              // [m3/s]
            qfdn=-hnn*(fn-fp);                    // diffusive flux
            if(qyl>0.0f)       {
                    if((*ds.ldry)(ix,is)==0) {
                         fnm=-.125*fs+.75*fp+.375*fn; 
                    }else {
                        fnm=0.5*fp+.5*fn;    
                   }
            }else{
                   if ((*ds.ldry)(ix,inn)==0) {
                        fnm=.375*fp+.75*fn-.125*fnn;
                   }else {
                       fnm=.5*fp+.5*fn;
                  }
            }
        } else {
            fnm=0.;
            qfdn=0.;
        }

        fnm=std::max(0.,fnm);

        // if Boundary (overwrite BC)
        if(iy==ds.NCOLS)    {
            fnm=(*ds.conc_SW)(ix,ds.NCOLS+1);
        }

        // advective flux - X-direction  
        qfcn=qyl*fnm*dx; // [g/s]

        // total flux
        qfn=qfcn+qfdn;
        
        // check available material if coming from the north cell
        if(qfn<0)    {
            if((*ds.ldry)(ix,in)==0)    {     
                cvolrate=-(fn*hn)*arean/ds.dtfl; 
                qfn=std::max(qfn,cvolrate);   //limit to available material
            }else {
                qfn=0.;
            }
        }

        // CHECK AVAILABLE MATERIAL - VOLUME RATE [m3/s] in actual cell
        cvolpot=(fp*hp)*area; // [m3]
        cvolrat=cvolpot/ds.dtfl + pfw+qfs; // inflow during actual time-step
      
        if (cvolrat>0.0f)  {                    // outflow is possible
            if(pfe>0.  &&  qfn>0.) {            // both outflow
                if (pfe+qfn > cvolrat) {        // limit outflow to volrat
                    cf=qfn/(pfe+qfn);
                    pfe= (1.-cf)*cvolrat;       // [m3/s]
                    qfn=cf*cvolrat;
                }
            } else if(pfe>0.){
                pfe=std::min(pfe,(cvolrat-qfn));
            } else if(qfn>0.){
                qfn=std::min(qfn,(cvolrat-pfe));         
            }
        }else { // bilance outflow with inflow
            if(pfe>=0.  &&  qfn<0.)  { //restrict pfe to bilan
                cbilan=cvolrat-qfn;                
                if(cbilan>0.) {
                    pfe=std::min(pfe,cbilan);
                }else{
                    pfe=0.;
                }
            } else if(pfe<0.  &&  qfn>=0.)  {
                cbilan=cvolrat-pfe;
                if(cbilan>0.) {
                    qfn=std::min(qfn,cbilan);
                }else{
                    qfn=0.;
                }
            } else if(pfe>=0.  &&  qfn>=0.)  {
                pfe=0.0f;
                qfn=0.0f;
            }        
        }                             

        // CALCULATE NEW CONCENTRATION
        dc=(pfw-pfe + qfs-qfn)*ds.dtfl/area;  // [m]  
        con= (*ds.conc_SW)(ix,iy) +  dc/hp;

        con=std::fmin(cmaxr(ix,iy),con);
        con=std::fmax(cminr(ix,iy),con);
        (*ds.conc_SW)(ix,iy) = con;

        qfcds(ix)=qfn;  // convective+diffusive flux
  
    }
}

*/