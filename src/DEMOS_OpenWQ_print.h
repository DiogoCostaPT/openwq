
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



#ifndef WRITEVTUH_INCLUDED
#define WRITEVTUH_INCLUDED

#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>

/* // if want to load 
#include <vtkXMLUnstructuredGridReader.h>
#
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
*/

#include "DEMOS_OpenWQ_global.h"

class DEMOS_OpenWQ_print{

    public:
    
    int writeVTU(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json, 
        DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars, 
        int numcmp, 
        int tmpst_i);

};

#endif