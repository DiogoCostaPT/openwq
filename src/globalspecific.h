// Copyright 2020, Diogo Costa
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

        this-> numcmp = numcmp;
        this-> numspec = numspec;
        this-> nx = nx;
        this-> ny = ny;
        this-> nz = nz;

        try{
            char cmpt_names[numcmp]; // compartment names
            char spec_names[numspec]; // species names

            //arma::Cube<double> domain_xyz(nx,ny,nz);
            

            //arma::field<arma::Mat<double>> wflux(numcmp,numspec); // water fluxes
            //arma::field<arma::Mat<double>> wmass(numcmp,numspec); // water mass
            //arma::field<arma::Mat<double>> chemass(numcmp,numspec); // chemical mass


            domain_xyz = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx,ny,nz));

            wflux = std::unique_ptr<arma::field<arma::Mat<double>>>(new arma::field<arma::mat>(numcmp,numspec));
            wmass = std::unique_ptr<arma::field<arma::Mat<double>>>(new arma::field<arma::mat>(numcmp,numspec));
            chemass = std::unique_ptr<arma::field<arma::Mat<double>>>(new arma::field<arma::mat>(numcmp,numspec));

            /*
            cmp1_wmass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[0][0],ny*onoff[0][0],nz*onoff[0][0]));
            cmp2_wmass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[1][0],ny*onoff[1][0],nz*onoff[1][0]));
            cmp3_wmass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[2][0],ny*onoff[2][0],nz*onoff[2][0]));
            cmp4_wmass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[3][0],ny*onoff[3][0],nz*onoff[3][0]));
            cmp5_wmass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[4][0],ny*onoff[4][0],nz*onoff[4][0]));
            cmp6_wmass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[5][0],ny*onoff[5][0],nz*onoff[5][0]));

            cmp1_wflux = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[0][0],ny*onoff[0][0],nz*onoff[0][0]));
            cmp2_wflux = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[1][0],ny*onoff[1][0],nz*onoff[1][0]));
            cmp3_wflux = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[2][0],ny*onoff[2][0],nz*onoff[2][0]));
            cmp4_wflux = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[3][0],ny*onoff[3][0],nz*onoff[3][0]));
            cmp5_wflux = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[4][0],ny*onoff[4][0],nz*onoff[4][0]));
            cmp6_wflux = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[5][0],ny*onoff[5][0],nz*onoff[5][0]));
            
            cmp1_chm1_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[0][0],ny*onoff[0][0],nz*onoff[0][0]));
            cmp2_chm1_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[1][0],ny*onoff[1][0],nz*onoff[1][0]));
            cmp3_chm1_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[2][0],ny*onoff[2][0],nz*onoff[2][0]));
            cmp4_chm1_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[3][0],ny*onoff[3][0],nz*onoff[3][0]));
            cmp5_chm1_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[4][0],ny*onoff[4][0],nz*onoff[4][0]));
            cmp6_chm1_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[5][0],ny*onoff[5][0],nz*onoff[5][0]));

            cmp1_chm2_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[0][1],ny*onoff[0][1],nz*onoff[0][1]));
            cmp2_chm2_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[1][1],ny*onoff[1][1],nz*onoff[1][1]));
            cmp3_chm2_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[2][1],ny*onoff[2][1],nz*onoff[2][1]));
            cmp4_chm2_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[3][1],ny*onoff[3][1],nz*onoff[3][1]));
            cmp5_chm2_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[4][1],ny*onoff[4][1],nz*onoff[4][1]));
            cmp6_chm2_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[5][1],ny*onoff[5][1],nz*onoff[5][1]));

            cmp1_chm3_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[0][2],ny*onoff[0][2],nz*onoff[0][2]));
            cmp2_chm3_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[1][2],ny*onoff[1][2],nz*onoff[1][2]));
            cmp3_chm3_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[2][2],ny*onoff[2][2],nz*onoff[2][2]));
            cmp4_chm3_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[3][2],ny*onoff[3][2],nz*onoff[3][2]));
            cmp5_chm3_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[4][2],ny*onoff[4][2],nz*onoff[4][2]));
            cmp6_chm3_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[5][2],ny*onoff[5][2],nz*onoff[5][2]));

            cmp1_chm4_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[0][3],ny*onoff[0][3],nz*onoff[0][3]));
            cmp2_chm4_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[1][3],ny*onoff[1][3],nz*onoff[1][3]));
            cmp3_chm4_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[2][3],ny*onoff[2][3],nz*onoff[2][3]));
            cmp4_chm4_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[3][3],ny*onoff[3][3],nz*onoff[3][3]));
            cmp5_chm4_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[4][3],ny*onoff[4][3],nz*onoff[4][3]));
            cmp6_chm4_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[5][3],ny*onoff[5][3],nz*onoff[5][3]));

            cmp1_chm5_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[0][4],ny*onoff[0][4],nz*onoff[0][4]));
            cmp2_chm5_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[1][4],ny*onoff[1][4],nz*onoff[1][4]));
            cmp3_chm5_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[2][4],ny*onoff[2][4],nz*onoff[2][4]));
            cmp4_chm5_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[3][4],ny*onoff[3][4],nz*onoff[3][4]));
            cmp5_chm5_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[4][4],ny*onoff[4][4],nz*onoff[4][4]));
            cmp6_chm5_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[5][4],ny*onoff[5][4],nz*onoff[5][4]));

            cmp1_chm6_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[0][5],ny*onoff[0][5],nz*onoff[0][5]));
            cmp2_chm6_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[1][5],ny*onoff[1][5],nz*onoff[1][5]));
            cmp3_chm6_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[2][5],ny*onoff[2][5],nz*onoff[2][5]));
            cmp4_chm6_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[3][5],ny*onoff[3][5],nz*onoff[3][5]));
            cmp5_chm6_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[4][5],ny*onoff[4][5],nz*onoff[4][5]));
            cmp6_chm6_mass = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx*onoff[5][5],ny*onoff[5][5],nz*onoff[5][5]));
            */


            //v_air = std::unique_ptr<arma::Mat<double>>( new  arma::mat(nl,nh));
        }catch(int e){
            std::cout << "An exception occurred creating the domain: ERR " << std::to_string(e) << std::endl;
        }

    }
    size_t numcmp, numspec, nx, ny, nz;

    /*
    std::unique_ptr<arma::Cube<double>> cmp1_wmass,cmp2_wmass,cmp3_wmass,cmp4_wmass,cmp5_wmass,cmp6_wmass,
        cmp1_wflux,cmp2_wflux,cmp3_wflux,cmp4_wflux,cmp5_wflux,cmp6_wflux,
        cmp1_chm1_mass,cmp2_chm1_mass,cmp3_chm1_mass,cmp4_chm1_mass,cmp5_chm1_mass,cmp6_chm1_mass,
        cmp1_chm2_mass,cmp2_chm2_mass,cmp3_chm2_mass,cmp4_chm2_mass,cmp5_chm2_mass,cmp6_chm2_mass,
        cmp1_chm3_mass,cmp2_chm3_mass,cmp3_chm3_mass,cmp4_chm3_mass,cmp5_chm3_mass,cmp6_chm3_mass,
        cmp1_chm4_mass,cmp2_chm4_mass,cmp3_chm4_mass,cmp4_chm4_mass,cmp5_chm4_mass,cmp6_chm4_mass,
        cmp1_chm5_mass,cmp2_chm5_mass,cmp3_chm5_mass,cmp4_chm5_mass,cmp5_chm5_mass,cmp6_chm5_mass,
        cmp1_chm6_mass,cmp2_chm6_mass,cmp3_chm6_mass,cmp4_chm6_mass,cmp5_chm6_mass,cmp6_chm6_mass;
        */
    
    //arma::Cube<double> domain_xyz;
    //arma::field<arma::Mat<double>> wflux,wmass,chemass;

    std::unique_ptr<arma::Cube<double>> domain_xyz;
    std::unique_ptr<arma::field<arma::Mat<double>>> wflux;
    std::unique_ptr<arma::field<arma::Mat<double>>> wmass;
    std::unique_ptr<arma::field<arma::Mat<double>>> chemass;
   
    

};

#endif