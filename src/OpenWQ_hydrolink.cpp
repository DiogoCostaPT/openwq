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


#include "OpenWQ_hydrolink.h"

// using namespace CRHM;
#include <iostream>


ClassWQ_OpenWQ::ClassWQ_OpenWQ(int _num): num(_num) {
    std::cout << "OpenWQ Class Created C++" << std::endl;
}
ClassWQ_OpenWQ::~ClassWQ_OpenWQ() {
    std::cout << "C++ deconstructor" << std::endl;
}
int ClassWQ_OpenWQ::getNum() const {
    return num;
}

CLASSWQ_OPENWQ* create_openwq(int num) {
    std::cout << "C API, create_openwq" << std::endl;
    return new ClassWQ_OpenWQ(num);
}

void delete_openwq(CLASSWQ_OPENWQ* openWQ) {
    std::cout << "C API, delete openwq" << std::endl;
    delete openWQ;
}

int openwq_getNum(const CLASSWQ_OPENWQ *openWQ) {
    return openWQ->getNum();
}




extern "C" { 



    // int *dummy_(int *num) {
    //     std::cout << *num << std::endl;
    //     int newNum = 1;

    //     return newNum;
    // }

    void *GetObject(int *num);

    void checkVal(void * obj);

    // void *openWQ_decl_(void*, int);

    // void *openWQ_run_time_start_(void*);

    // void *openWQ_run_space_(void*);

    // void *openWQ_run_time_end_(void*);
}

void *GetObject(int* num) {
    std::cout << "HERE IN Get OBject" << std::endl;
    std::cout << *num << std::endl;
    ClassWQ_OpenWQ *openWQ = new ClassWQ_OpenWQ(*num);
    std::cout << &openWQ << std::endl;
    openWQ->printNum();
    return (void *)openWQ;
}

void checkVal(void *obj) {
    ClassWQ_OpenWQ *openWQ = (ClassWQ_OpenWQ*)obj;
    std::cout << &openWQ << std::endl;
    std::cout << "HERE" << std::endl;

    openWQ->printNum();

}


// void *openWQ_decl(void * openWQ, int num_hrus) {


// }

// void *openWQ_run_time_start(void *openWQ) {

//     return;
// }

// void *openWQ_run_space(void *openWQ) {

//     return;
// }

// void *openWQ_run_time_end(void *openWQ) {

//     return;
// }