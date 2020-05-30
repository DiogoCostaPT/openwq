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

#ifndef GLOBALGENERALH_INCLUDED
#define GLOBALGENERALH_INCLUDED

#include <string>

// General information about the project
class ClassGenProj
{
    public:

    struct geninfo {
        std::string project_name;
        std::string geographical_location;
        std::string authors;
        std::string date;
        std::string additional_info;
    } geninfo;

    struct size {
        int number_of_compartments;
        int number_of_chemical_species;
        int dim_nx;
        int dim_ny;
        int dim_nz;
    } size;

};

// Binary array to activate/deactivate compartments and chemcial species
class OnOFF {

    public:

    OnOFF(){

    }
    OnOFF(size_t numcmp,size_t numspec){

        this-> numcmp = numcmp;
        this-> numspec = numspec;

        unsigned int bin[numcmp][numspec];
    }

    size_t numcmp, numspec;
    unsigned int bin;

};

#endif