// Copyright 2020, Diogo Costa
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


#ifndef UTILITYH_INCLUDED
#define UTILITYH_INCLUDED

#include <armadillo>
#include "string.h"
#include <dirent.h>
#include <algorithm>

#include "global.h"
#include "ChemTransf.h"

using json = nlohmann::json;

void initmemory(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar);

void read_JSON_2class(json & jsondata,const std::string& jsonfile);

void readSetIC(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar);

void readSetFluxes(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar);

void read_file_3Dcoldata(json & filejson,arma::Cube<double> & to_cubedata, 
    int var_col,std::string filename);

void read_file_CMPIcoldata(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar, int iteraction, int source, 
    int recipient, std::string filename, std::string exchange_type);

void GetFluxesFiles(JSONfiles& JSONfiles,std::vector<std::vector<std::string>> &fluxes_filenames);

void GetComptInteractFluxesFiles(JSONfiles& JSONfiles,std::vector<std::vector<std::string>> &compInt_filenames);

void GetFilesInFolder(std::string &path,std::vector<std::string> &filenames_i);

void ConvertSortFilenames2Double(int numtotal,std::vector<std::vector<std::string>>& filenames,
    std::vector<std::vector<double>>& filenames_num);

void IdentifyMobileCompt(std::vector<std::vector<double>> &filenames_num,
    std::vector<int> &mobileCompt);

bool CheckIfCompTimeStepsMatch(std::vector<std::vector<double>> &fluxes_filenames_num);

void GetFileExtension(std::string & str, std::string & str_extention);

#endif