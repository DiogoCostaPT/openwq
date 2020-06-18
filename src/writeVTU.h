
#ifndef WRITEVTUH_INCLUDED
#define WRITEVTUH_INCLUDED

#include <vtkSmartPointer.h>
#include <vtkCellArray.h>
//#include <vtkXMLUnstructuredGridReader.h>
//#include <vtkDataSetMapper.h>
//#include <vtkActor.h>
//#include <vtkRenderer.h>
//#include <vtkRenderWindow.h>
//#include <vtkRenderWindowInteractor.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPointData.h>
//#include <vtkVertexGlyphFilter.h>

#include <vtkHexagonalPrism.h>
#include <vtkHexahedron.h>

#include "global.h"

void writeVTU(JSONfiles& JSONfiles,int icmp);

#endif