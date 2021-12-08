

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
        int numHRU;
        OpenWQ_couplercalls *OpenWQ_couplercalls_ref;
        OpenWQ_hostModelconfig *OpenWQ_hostModelconfig_ref;
        OpenWQ_json *OpenWQ_json_ref;
        OpenWQ_wqconfig *OpenWQ_wqconfig_ref;
        OpenWQ_units *OpenWQ_units_ref;
        OpenWQ_readjson *OpenWQ_readjson_ref;
        OpenWQ_vars *OpenWQ_vars_ref;
        OpenWQ_initiate *OpenWQ_initiate_ref;
        OpenWQ_watertransp *OpenWQ_watertransp_ref;
        OpenWQ_chem *OpenWQ_chem_ref;            
        OpenWQ_sinksource *OpenWQ_sinksource_ref;
        OpenWQ_output *OpenWQ_output_ref;        

    
    // Constructor
    public:
        ClassWQ_OpenWQ(int numHRU);
        ~ClassWQ_OpenWQ();
    
    // Methods
    void printNum() {
        std::cout << "num = " << this->numHRU << std::endl;
    }
    int getNum() const;

    int decl();

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

// Create OpenWQ Object
CLASSWQ_OPENWQ* create_openwq(int num);
// Delete OpenWQ Object
void delete_openwq(CLASSWQ_OPENWQ* openWQ);
// Test Method
int openwq_getNum(const CLASSWQ_OPENWQ *openWQ);
// OpenWQ initalization method
int openwq_decl(CLASSWQ_OPENWQ *openWQ);
#ifdef __cplusplus
}
#endif
#endif