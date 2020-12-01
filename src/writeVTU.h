
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

int writeVTU(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json,int icmp, DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars, int tmpst_i);

#endif