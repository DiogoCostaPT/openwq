MacOS
==================================

1. Armadillo
	First make sure that you have the latest version of Xcode and Command Line Tools installed. You could install Xcode from the AppStore application.
	
	You could install lapack and openblas with the following command:
	
	``brew install openblas``
	``brew install lapack``
	
	Usually , Homebrew has the latest version of Armadillo available. So with
	
	``brew tap homebrew/science``
	
	``brew install cmake pkg-config``
	
	``brew install armadillo``
	
	You could install Armadillo with new enough version.
	
2. OpenMP

	Install OpenMP with 
	
	``brew install libomp``
	
3. VTK
	
	https://formulae.brew.sh/formula/vtk on MacOS, we are able to install the vtk directly with the command
	
	``brew install vtk``
	
4. Cmake and compile
	You could download the cmake package from the website
	
	https://cmake.org/download/
	
	There is a CmakeLists.txt in the github repo. You could directly compile the application with the command
	
	``cmake .``
	
	``make``
	
	The executable file will be generated to the Results folder.
