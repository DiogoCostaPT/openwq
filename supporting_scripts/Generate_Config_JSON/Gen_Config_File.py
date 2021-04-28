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
# JBGC JSON file name (define full path)
bgc_jsonfile = '/media/dcosta/data/megasync/ec_main/models/openwq/code/internally_coupled/supporting_scripts/graphml_to_json/BGC_cycling_Database/CRHM_soilNandP_cycle.json'
new_configfile_name = 'examples/OpenWQ_config.json'

###########################################
# Configuration setup

# Transport configuration
disperson_x_m2 = 0.3
disperson_y_m2 = 0.3
disperson_z_m2 = 0.3

# Biogeochemistry configuration
Compartment_names = [
    'SWE',
    'RUNOFF',
    'SD',
    'SOIL_RECHR',
    'SOIL_LOWER',
    'SURFSOIL',
    'GW',
]
BGC_frameworks_associated = [
    ['global', 'diogo'],
    ['global'],
    ['global'],
    ['global'],
    ['global'],
    ['global'],
    ['global']
]

# default IC configuration for all compartments and species
# this can be changed directly in the JSON config file generated
default_IC_value = 2                # float
default_IC_type = 'concentration'   # string
default_IC_unit = 'mg/l'            # string


###########################################


# Don't change below this point
##
##
##
###########################################
###########################################

###########################################
# Configuration setup checks
# check if Compartment_names and BGC_frameworks_associated have the same size
if len(Compartment_names) != len(BGC_frameworks_associated):
    print('> ERROR: Number of entries in "Compartment_names" and "BGC_frameworks_associated" must match.')
    exit()

###########################################
# load BGC json file (needed to get names of chemical species)
with open(bgc_jsonfile) as f:
    BGCjson = json.load(f)

#################################################
# Creating JSON Config file
#################################################

Config_JSON_dict = {}
Config_JSON_dict["TRANSPORT_CONFIGURATION"] = {}
Config_JSON_dict["BIOGEOCHEMISTRY_CONFIGURATION"] = {}

#################################################
# Add Transport Configuration
Config_JSON_dict["TRANSPORT_CONFIGURATION"]['disperson_x_m2'] = disperson_x_m2
Config_JSON_dict["TRANSPORT_CONFIGURATION"]['disperson_y_m2'] = disperson_y_m2
Config_JSON_dict["TRANSPORT_CONFIGURATION"]['disperson_z_m2'] = disperson_z_m2

#################################################
# Add biogeochemistry configuration
num_compt = len(Compartment_names)  # number of compartments
chemspec_names = BGCjson["CHEMICAL_SPECIES"]["LIST"]
num_spec = len(chemspec_names)

default_IC = [default_IC_value,
              default_IC_type,
              default_IC_unit]   # default IC array

for i in range(0, num_compt):
    # initiate dictionary
    Config_JSON_dict["BIOGEOCHEMISTRY_CONFIGURATION"][Compartment_names[i]] = {}

    # add cycling frameworks
    Config_JSON_dict["BIOGEOCHEMISTRY_CONFIGURATION"][Compartment_names[i]]["CYCLING_FRAMEWORK"] = \
        BGC_frameworks_associated[i]

    # initiate IC dict
    Config_JSON_dict["BIOGEOCHEMISTRY_CONFIGURATION"][Compartment_names[i]]["INITIAL_CONDITIONS"] = {}

    # add initial conditions (for each chemical species)
    for s in range(0, num_spec):
        Config_JSON_dict["BIOGEOCHEMISTRY_CONFIGURATION"][Compartment_names[i]]["INITIAL_CONDITIONS"] \
            [chemspec_names[str(s+1)]] = default_IC
            


##############################
# Save to JSON file
JSON_filename = new_configfile_name
with open(JSON_filename, 'w') as outfile:
    json.dump(Config_JSON_dict, outfile, separators=(',', ': '), indent=4)