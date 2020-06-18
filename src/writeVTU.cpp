
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

/*
  static double x[27][3]={{0,0,0}, {1,0,0}, {2,0,0}, {0,1,0}, {1,1,0}, {2,1,0},
                          {0,0,1}, {1,0,1}, {2,0,1}, {0,1,1}, {1,1,1}, {2,1,1},
                          {0,1,2}, {1,1,2}, {2,1,2}, {0,1,3}, {1,1,3}, {2,1,3},
                          {0,1,4}, {1,1,4}, {2,1,4}, {0,1,5}, {1,1,5}, {2,1,5},
                          {0,1,6}, {1,1,6}, {2,1,6}};

  static vtkIdType pts[12][8]={{0, 1, 4, 3, 6, 7, 10, 9},
                               {1, 2, 5, 4, 7, 8, 11, 10},
                               {6, 10, 9, 12, 0, 0, 0, 0},
                               {8, 11, 10, 14, 0, 0, 0, 0},
                               {16, 17, 14, 13, 12, 15, 0, 0},
                               {18, 15, 19, 16, 20, 17, 0, 0},
                               {22, 23, 20, 19, 0, 0, 0, 0},
                               {21, 22, 18, 0, 0, 0, 0, 0},
                               {22, 19, 18, 0, 0, 0, 0, 0},
                               {23, 26, 0, 0, 0, 0, 0, 0},
                               {21, 24, 0, 0, 0, 0, 0, 0},
                               {25, 0, 0, 0, 0, 0, 0, 0}};

*/
    int numvert = (nx+1)*(ny+1)*(nz+1);
    int nnumel = nx * ny * nz;
    
  
    vtkIdType pts[nnumel][8];
    int i = 0;
     for (int ix=0;ix<=nx;ix++){
        for (int iy=0;iy<=ny;iy++){
            for (int iz=0;iz<=nz;iz++){
                pts[i][0] = i;
                pts[i][1] = i+1;
                pts[i][2] = (ny+1) + i;
                pts[i][3] = (ny+1) + i+1;
                pts[i][4] = (ny+1)*(nx+1) + i;
                pts[i][5] = (ny+1)*(nx+1) + i+1;
                pts[i][6] = (ny+1)*(nx+1) + (ny+1) + i;
                pts[i][7] = (ny+1)*(nx+1) + (ny+1) + i+1;
                i++;
            }
        }
     }

    double x[numvert][3];

    i = 0;

    for (int iz=0;iz<=nz;iz++){
        for (int iy=0;iy<=ny;iy++){
            for (int ix=0;ix<=nx;ix++){
                x[i][0] = ix-0.5;
                x[i][1] = iy-0.5;
                x[i][2] = iz-0.5;
                i++;
            }
        }
    }


  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  for (i=0; i<numvert; i++) points->InsertPoint(i,x[i]);

  vtkSmartPointer<vtkUnstructuredGrid> ugrid =
    vtkSmartPointer<vtkUnstructuredGrid>::New();
  ugrid->Allocate(100);
  for (i=0; i<nnumel; i++){
      ugrid->InsertNextCell(VTK_HEXAHEDRON, 8, pts[i]);
  }

  ugrid->SetPoints(points);


  //return EXIT_SUCCESS;

      // Write file
    vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer =
        vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();  
    writer->SetFileName(filename.c_str());
    
    writer->SetInputData(ugrid);
    writer->Write();

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