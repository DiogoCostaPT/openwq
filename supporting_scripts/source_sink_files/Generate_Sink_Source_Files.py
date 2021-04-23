
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

###########################################
# sink-source file name (define full path)
new_sink_source_jsonfile = '/media/dcosta/data/megasync/ec_main/models/openwq/code/internally_coupled/supporting_scripts/source_sink_files/OpenWQ_source_example.json'

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


