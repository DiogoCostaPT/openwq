
#ifndef WRITEVTUH_INCLUDED
#define WRITEVTUH_INCLUDED

#include <vtkSmartPointer.h>
//#include <vtkCellArray.h>
//#include <vtkXMLUnstructuredGridReader.h>
//#include <vtkDataSetMapper.h>
//#include <vtkActor.h>
//#include <vtkRenderer.h>
//#include <vtkRenderWindow.h>
//#include <vtkRenderWindowInteractor.h>
#include <vtkXMLUnstructuredGridWriter.h>
//#include <vtkUnstructuredGrid.h>
//#include <vtkPointData.h>
//#include <vtkVertexGlyphFilter.h>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellType.h>
#include <vtkDataSetMapper.h>
#include <vtkNamedColors.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDoubleArray.h>

#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>

#include "global.h"

void writeVTU(JSONfiles& JSONfiles,int icmp);

#endif