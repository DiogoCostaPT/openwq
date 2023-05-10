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

// For VTK support:
// This was built for hexahedrons (see line 66) for now (cubes, Rectangular cuboid, Trigonal trapezohedron, etc)
// But line 33:33 makes determining cubes of side lenght = 1 (but this can all be changed)
// based on https://lorensen.github.io/VTKExamples/site/Cxx/IO/WriteVTU/


#include "OpenWQ_utils.hpp"


// Function to return total number of days in a given year and month
int  OpenWQ_utils::getNumberOfDaysInMonthYear(
        const unsigned int YYYY_check,          // json: Year 
        const unsigned int MM_check)            // json: Month
{
	//leap year condition, if month is 2
	if(MM_check == 2)
	{
		if((YYYY_check%400==0) || (YYYY_check%4==0 && YYYY_check%100!=0))	
			return 29;
		else	
			return 28;
	}
	//months which has 31 days
	else if(MM_check == 1 || MM_check == 3 || MM_check == 5 || MM_check == 7 || MM_check == 8
	||MM_check == 10 || MM_check==12)	
		return 31;
	else 		
		return 30;

} 