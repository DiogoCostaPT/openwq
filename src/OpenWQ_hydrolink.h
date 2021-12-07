

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

#ifndef OPENWQ_HYDROLINK_INCLUDED
#define OPENWQ_HYDROLINK_INCLUDED


#include "OpenWQ_couplercalls.h"
#include "OpenWQ_global.h"
#include "OpenWQ_readjson.h"
#include "OpenWQ_initiate.h"
#include "OpenWQ_chem.h"
#include "OpenWQ_watertransp.h"
#include "OpenWQ_sinksource.h"
#include "OpenWQ_units.h"
#include "OpenWQ_solver.h"
#include "OpenWQ_output.h"

class ClassWQ_OpenWQ
{
    // Instance Variables
    private:
        int num;
    
    // Constructor
    public:
        ClassWQ_OpenWQ(int num);
        ~ClassWQ_OpenWQ();
    
    // Methods
    void printNum() {
        std::cout << "num = " << this->num << std::endl;
    }
    int getNum() const;

    void decl(int num_hru);

    void run_time_start();

    void run_space();

    void run_time_end();

};


#ifdef __cplusplus
extern "C" { 
    class ClassWQ_OpenWQ;
    typedef ClassWQ_OpenWQ CLASSWQ_OPENWQ;
#else
    typedef struct CLASSWQ_OPENWQ CLASSWQ_OPENWQ;
#endif

CLASSWQ_OPENWQ* create_openwq(int num);

void delete_openwq(CLASSWQ_OPENWQ* openWQ);

int openwq_getNum(const CLASSWQ_OPENWQ *openWQ);
#ifdef __cplusplus
}
#endif
#endif