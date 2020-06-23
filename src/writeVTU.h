
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


#include "global.h"

int writeVTU(JSONfiles& JSONfiles,int icmp, Prj_StateVar& Prj_StateVar, int tmpst_i);

#endif