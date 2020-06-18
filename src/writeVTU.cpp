
// Write VTU 

#include "writeVTU.h"

void writeVTU(JSONfiles& JSONfiles,int icmp)
{

    std::string filename = JSONfiles.H2O["compartments"][std::to_string(icmp+1)];
    filename.append(".vtu");

    int nx = JSONfiles.H2O[std::to_string(icmp+1)]["nx"];
    int ny = JSONfiles.H2O[std::to_string(icmp+1)]["ny"];
    int nz = JSONfiles.H2O[std::to_string(icmp+1)]["nz"];

    int a = 0;

/*
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for (int ix=0;ix<nx;ix++){
        for (int iy=0;iy<ny;iy++){
            for (int iz=0;iz<nz;iz++){
                points->InsertNextPoint(ix, iy, iz);
            }
        }
    }

    vtkSmartPointer<vtkTetra> tetra = vtkSmartPointer<vtkTetra>::New();
    for (int ix=0;ix<nx;ix++){
        for (int iy=0;iy<ny;iy++){
            for (int iz=0;iz<nz;iz++){
                tetra->GetPointIds()->SetId(a,a);
                a++;
            }
        }
    }


    vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();
    cellArray->InsertNextCell(tetra);

    vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    unstructuredGrid->SetPoints(points);
    unstructuredGrid->SetCells(VTK_TETRA, cellArray);

*/


   // A regular hexagon (cube) with all faces square and three squares around
  // each vertex is created below.

  // Setup the coordinates of eight points
  // (the two faces must be in counter clockwise
  // order as viewed from the outside).

  // As an exercise you can modify the coordinates of the points to create
  // seven topologically distinct convex hexahedras.

  int numberOfVertices = 8;

// Write file
    vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer =
        vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();  
    writer->SetFileName(filename.c_str());
    

for (int i = 0; i < 2; ++i)
    {
  // Create the points
  vtkSmartPointer<vtkPoints> points =
    vtkSmartPointer<vtkPoints>::New();
  points->InsertNextPoint(0.0+i, 0.0+i, 0.0+i);
  points->InsertNextPoint(1.0+i, 0.0+i, 0.0+i);
  points->InsertNextPoint(1.0+i, 1.0+i, 0.0+i);
  points->InsertNextPoint(0.0+i, 1.0+i, 0.0+i);
  points->InsertNextPoint(0.0+i, 0.0+i, 1.0+i);
  points->InsertNextPoint(1.0+i, 0.0+i, 1.0+i);
  points->InsertNextPoint(1.0+i, 1.0+i, 1.0+i);
  points->InsertNextPoint(0.0+i, 1.0+i, 1.0+i);
  

  // Create a hexahedron from the points
  vtkSmartPointer<vtkHexahedron> hex =
    vtkSmartPointer<vtkHexahedron>::New();
  for (int i = 0; i < numberOfVertices; ++i)
    {
    hex->GetPointIds()->SetId(i, i);
    }

  // Add the points and hexahedron to an unstructured grid
  vtkSmartPointer<vtkUnstructuredGrid> uGrid =
    vtkSmartPointer<vtkUnstructuredGrid>::New();
  uGrid->SetPoints(points);
  uGrid->InsertNextCell(hex->GetCellType(), hex->GetPointIds());
    
    writer->SetInputData(uGrid);
    writer->Write();
    }

// Display file without the need of Parview
/*
    
    vtkSmartPointer<vtkXMLUnstructuredGridReader> reader = vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
    reader->SetFileName(filename.c_str());
    reader->Update();

    vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper->SetInputConnection(reader->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper); // actor contain`s: (1) object properties (color, shading type, etc), (2) geometry, and (3) transformations

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(.3, .6, .3); // Background color green

    renderWindow->Render();
    renderWindowInteractor->Start();
    */

}