
#ifndef GLOBALSPECIFICH_INCLUDED
#define GLOBALSPECIFICH_INCLUDED

//#include "globalgeneral.h"

#include<armadillo>
#include<memory> 

// General information about the project
class ClassDetProj
{
    public:
    ClassDetProj(){

    }
    ClassDetProj(size_t numcmp,size_t numspec, size_t nx, size_t ny, size_t nz){

        //ClassGenProj ClassGP; 

        this-> numcmp = numcmp;
        this-> numspec = numspec;
        this-> nx = nx;
        this-> ny = ny;
        this-> nz = nz;

        char cmpt_names[numcmp]; // compartment names
        char spec_names[numspec]; // species names
        
        cmp1_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx,ny,nz));
        cmp1_conc = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx,ny,nz));

        //v_air = std::unique_ptr<arma::Mat<double>>( new  arma::mat(nl,nh));

    }
    size_t numcmp, numspec, nx, ny, nz;

    std::unique_ptr<arma::Cube<double>> cmp1_mass,cmp1_conc;
    

};

#endif