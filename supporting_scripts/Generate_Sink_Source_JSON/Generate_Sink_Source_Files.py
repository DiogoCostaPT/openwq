
# Copyright 2020, Diogo Costa, diogo.pinhodacosta@canada.ca
# This file is part of OpenWQ model.

# This program, openWQ, is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import json
import numpy as np

###########################################
# sink-source (SS) file name (define relative or full path)
new_SS_JSONfile = 'examples/OpenWQ_source_example.json'

# keys and values for JSON file
METADATA_Comment = 'Test'
METADATA_Source = 'test'

Chemical_name = ["NO3", "NH4"]
Compartment_name = ["SWE", "SWE"]
Type = ["source", "source"]
Units = ["kg", "kg"]
Data_CSV_source = ["examples/NO3_fertilizer.csv", "examples/NH4_fertilizer.csv"]

# Don't change below this point
##
##
##
###########################################
###########################################

################################################
# Check consistency of input data
n_Chemical_name = len(Chemical_name)
n_Compartment_name = len(Compartment_name)
n_Type = len(Type)
n_nits = len(Units)
n_Data_CSV_source = len(Data_CSV_source)

if n_Chemical_name != n_Compartment_name or \
    n_Chemical_name != n_Type or \
    n_Chemical_name != n_nits or \
    n_Chemical_name != n_Data_CSV_source:

    print('> ERROR: Input data is not consistent (number of elements does not match)')
    exit()

#################################################
# Creating all dictionaries
#################################################

SS_JSON_dic = {}
SS_JSON_dic["METADATA"] = {}

# create dictionary for each sink-source file
for s in range(0, n_Data_CSV_source):
    SS_JSON_dic[str(s+1)] = {}

#################################################
# Add data to dictionaries
#################################################
SS_JSON_dic["METADATA"]["Comment"] = METADATA_Comment
SS_JSON_dic["METADATA"]["Source"] = METADATA_Source

# define dictionary for each sink-source file
for s in range(0, n_Data_CSV_source):
    # General info
    SS_JSON_dic[str(s + 1)]["Chemical_name"] = Chemical_name[s]
    SS_JSON_dic[str(s + 1)]["Compartment_name"] = Compartment_name[s]
    SS_JSON_dic[str(s + 1)]["Type"] = Type[s]
    SS_JSON_dic[str(s + 1)]["Units"] = Units[s]
    SS_JSON_dic[str(s + 1)]["Data"] = {}

    # actual forcing data
    try:
        SS_data = np.genfromtxt(Data_CSV_source[s], delimiter=',', skip_header=1)
        num_rows, num_cols = SS_data.shape # number of cols and rows of SS_data

        # write to dictionary
        for r in range(0, num_rows):
            row_data = SS_data[r, :].tolist() # needs to be serialized for json
            SS_JSON_dic[str(s + 1)]["Data"][str(r + 1)] = row_data

    except:
        print('> ERROR: file not found' + Data_CSV_source[s])
        exit()

##############################
# Save to JSON file
with open(new_SS_JSONfile, 'w') as outfile:
    json.dump(SS_JSON_dic, outfile, separators=(',', ': '), indent=4)