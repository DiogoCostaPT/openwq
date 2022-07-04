
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
# graphml file name (define full path)

new_BGC_JSON_name = 'BGC_cycling_Database/OpenWQ_BGC_STC.json'

CYCLING_FRAMEWORKS_names = ['global',
                            'river'
                            ]

GRAPHML_filepaths_associated = ['BGC_cycling_Database/CRHM_soilNandP_cycle.graphml',
                                'BGC_cycling_Database/riverNandP_cycle.graphml']

kinetics_flag_graphml_arrow_labels = 'kinetics:'
mobile_flag_graphml_nodes_labels = 'mobile:'
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
if len(CYCLING_FRAMEWORKS_names) != len(GRAPHML_filepaths_associated):
    print('> ERROR: Number of entries in "CYCLING_FRAMEWORKS_names" and "GRAPHML_filepaths_associated" must match.')
    exit()

# number of BGC frameworks to include
num_BGC_frameworks = len(CYCLING_FRAMEWORKS_names)

# Data needed from all graphml
node_ids_all = []
node_labels_all = []
arrow_ids_all = []
arrow_labels_all = []
arrow_kinetics_all = []
arrow_sources_all = []
arrow_targets_all = []
vector_mobile_all = []

###########################################
# loop over BGC frameworks to extract data
###########################################
for i in range(0, num_BGC_frameworks):

    # Data needed from graphml
    node_ids = []
    node_labels = []
    arrow_ids = []
    arrow_labels = []
    arrow_kinetics = []
    arrow_sources = []
    arrow_targets = []

    # Dummy variables
    arrow_lablkinet = []
    vector_mobile = []
    node_index = 1

    # flags
    found_graph_flag = False
    found_node_flag = False
    found_arrow_flag = False

    # open and read graphml file
    my_file = open(GRAPHML_filepaths_associated[i], "r")
    content_list = my_file.readlines()

    # Extract info from graphml file
    for l in range(0, len(content_list)):
        content_l = content_list[l]

        # Continue if not inside graph section
        if content_l.find('<graph ') == -1 & found_graph_flag == False:
            continue
        else:
            found_graph_flag = True

        # Check if node
        if content_l.find('<node ') != -1:
            found_node_flag = True
            found_arrow_flag = False

        # Check if arrow
        if content_l.find('<edge ') != -1:
            found_arrow_flag = True
            found_node_flag = False

        #############################################
        # Search for node info
        #############################################
        if (found_node_flag == True):



            # Extract id
            id_key = '<node id="'
            if content_l.find(id_key) != -1:
                keyword_str = '<node id="'
                keyword_end = '">'
                loc_str = content_l.find(keyword_str) + len(keyword_str)
                loc_end = content_l.find(keyword_end)
                node_ids.append(content_l[loc_str:loc_end])

            # Extract label
            label_key = '<y:NodeLabel '
            if content_l.find(label_key) != -1:
                keyword_str = '">'
                keyword_end = '<y:LabelModel>'
                loc_str = content_l.find(keyword_str) + len(keyword_str)
                loc_end = content_l.find(keyword_end)

                new_entry = content_l[loc_str:loc_end]
                if new_entry.find(mobile_flag_graphml_nodes_labels) != -1:
                    if new_entry.find('true') != -1:
                        vector_mobile.append(node_index)
                    node_index += 1
                else:
                    node_labels.append(new_entry)

            else:
                continue


        #############################################
        # Search for arrow info
        #############################################
        if (found_arrow_flag == True):

            # Extract ids, sources and targets
            id_key = '<edge id="'
            if content_l.find(id_key) != -1:

                # id
                keyword_str = '<edge id="'
                keyword_end = '" source'
                loc_str = content_l.find(keyword_str) + len(keyword_str)
                loc_end = content_l.find(keyword_end)
                arrow_ids.append(content_l[loc_str:loc_end])

                # source
                keyword_str = 'source="'
                keyword_end = '" target'
                loc_str = content_l.find(keyword_str) + len(keyword_str)
                loc_end = content_l.find(keyword_end)
                arrow_sources.append(content_l[loc_str:loc_end])

                # target
                # ids
                keyword_str = 'target="'
                keyword_end = '">'
                loc_str = content_l.find(keyword_str) + len(keyword_str)
                loc_end = content_l.find(keyword_end)
                arrow_targets.append(content_l[loc_str:loc_end])

            # Extract label
            label_key = '<y:EdgeLabel '
            if content_l.find(label_key) != -1:
                keyword_str = '">'
                keyword_end = '<y:LabelModel>'
                loc_str = content_l.find(keyword_str) + len(keyword_str)
                loc_end = content_l.find(keyword_end)

                # if properties of labels are changed, if may not find loc_end
                # this is another attempt to find it
                if loc_end == -1:
                    keyword_end = '<y:PreferredPlacementDescriptor'
                    loc_end = content_l.find(keyword_end)

                arrow_lablkinet.append(content_l[loc_str:loc_end])

            else:
                continue

    # Abort if arrows don't have all the info needed: name of transformation + kinetics
    if len(node_labels) != len(node_ids):
        print('> ERROR: Chemical Constituents: each node needs two labels (name of chemical and mobile-bool)')
        exit()
    if len(arrow_lablkinet)/2 != len(arrow_ids):
        print('> ERROR: Chemical Transformations: each arrow needs two labels (transformation name and kinetics)')
        exit()

    # Separate labels and kinetics
    for i in range(0, len(arrow_lablkinet)):
        arrow_info_i = arrow_lablkinet[i]
        # if kinetics
        if arrow_info_i.find(kinetics_flag_graphml_arrow_labels) != -1:
            arrow_kinetics.append(arrow_info_i[len(kinetics_flag_graphml_arrow_labels):len(arrow_info_i)])
        else:
             arrow_labels.append(arrow_info_i)

    # Abort if number of arrow labels and kinetics don't match
    if len(arrow_labels) != len(arrow_kinetics):
        print('> ERROR: No match between transformation labels and transformation kinetics. \n'
              '  Make sure to add signatures in graphml file to all kinetics given by python variable "kinetics_flag_graphml_arrow_labels"\n'
              '  Signature (kinetics_flag_graphml_arrow_labels) = ' + kinetics_flag_graphml_arrow_labels
              )
        exit()

    # Append graphml data to global database
    node_ids_all.append(node_ids)
    node_labels_all.append(node_labels)
    arrow_ids_all.append(arrow_ids)
    arrow_labels_all.append(arrow_labels)
    arrow_kinetics_all.append(arrow_kinetics)
    arrow_sources_all.append(arrow_sources)
    arrow_targets_all.append(arrow_targets)
    vector_mobile_all.append(vector_mobile)

#################################################
# Creating JSON BGC file
#################################################

# Initiating JSON
BGC_JSON_dict = {}
BGC_JSON_dict["CHEMICAL_SPECIES"] = {}
BGC_JSON_dict["CHEMICAL_SPECIES"]["LIST"] = {}
BGC_JSON_dict["CHEMICAL_SPECIES"]["MOBILE_SPECIES"] = []
BGC_JSON_dict["CYCLING_FRAMEWORKS"] = {}

##############################
# Construct chemical list data

# look for unique node names (chemical pools)
node_labels_all_vec = []
for i in range(0, len(node_ids_all)):

    # get i graphml node names
    node_labels = node_labels_all[i]
    num_node_labels = len(node_labels)

    # add to node_labels_all_vec unique node_labels entries
    for c in range(0, num_node_labels):
        node_name_entry = node_labels[c]

        # if not in list, then add
        if node_name_entry not in node_labels_all_vec:
            node_labels_all_vec.append(node_name_entry)


# Construct CHEMICAL_SPECIES LIST
for i in range(0, len(node_labels_all_vec)):
    BGC_JSON_dict["CHEMICAL_SPECIES"]["LIST"][str(i + 1)] = node_labels_all_vec[i]


# Get mobility info and check if consistent between BGC frameworks
vector_mobile_all_vec = []
for i in range(0, len(node_labels_all_vec)):

    # get node i name
    node_labels_all_vec_i = node_labels_all_vec[i]

    # get mobile info for all existing node_labels_all_vec_i entries
    mobile_info_i_all = []

    # check if node exists in each BGC framework
    for c in range(0, len(node_labels_all)):

        node_labels = node_labels_all[c]
        vector_mobile = vector_mobile_all[c]
        
        # check if node_labels_all_vec_i exists in node_labels
        if node_labels_all_vec_i in node_labels:
            # if yes, get index that should be contained in mobile vector if mobile
            iloc = node_labels.index(node_labels_all_vec_i) + 1 # + 1 because numbering in the json file starts in 1
            # check if mobile
            if iloc in vector_mobile:
                mobile_info_i_all.append(1)
            else:
                mobile_info_i_all.append(0)

    # Comparing each element with first item
    elem = mobile_info_i_all[0];
    for item in mobile_info_i_all:
        if elem != item:
            print('> ERROR: Mobility of ' + node_labels_all_vec_i + 'is not consistent across BGC frameworks')
            exit()

    # Add to vector_mobile_all_vec is elem = 1 (i.e. mobile)
    if elem == 1:
        vector_mobile_all_vec.append(i+1) # + 1 because numbering in the json file starts in 1

# Add mobility info to JSON
BGC_JSON_dict["CHEMICAL_SPECIES"]["MOBILE_SPECIES"] = vector_mobile_all_vec

##############################
# Construct CYCLING_FRAMEWORKS dictionary with all BGC frameworks

# Add data for each BGC framework
for b in range(0, len(node_ids_all)):

    # Create a dictionary for each BGC framework
    BGC_JSON_dict["CYCLING_FRAMEWORKS"][CYCLING_FRAMEWORKS_names[b]] = {}

    # Add list of transformations and details for each of them
    BGC_JSON_dict["CYCLING_FRAMEWORKS"][CYCLING_FRAMEWORKS_names[b]]["LIST_TRANSFORMATIONS"] = {}
    
    # Get all info for each BGC cycle
    node_ids = node_ids_all[b]
    node_labels = node_labels_all[b]
    arrow_ids = arrow_ids_all[b]
    arrow_labels = arrow_labels_all[b]
    arrow_kinetics = arrow_kinetics_all[b]
    arrow_sources = arrow_sources_all[b]
    arrow_targets = arrow_targets_all[b]
    vector_mobile = vector_mobile_all[b]
    
    # Add info for each BGC framework
    for i in range(0, len(arrow_ids)):

        # Add to list of transformations
        BGC_JSON_dict["CYCLING_FRAMEWORKS"][CYCLING_FRAMEWORKS_names[b]]["LIST_TRANSFORMATIONS"][str(i + 1)] = \
            arrow_labels[i]

        # Add details of transformation
        BGC_JSON_dict["CYCLING_FRAMEWORKS"][CYCLING_FRAMEWORKS_names[b]][str(i + 1)] = {}
        BGC_JSON_dict["CYCLING_FRAMEWORKS"][CYCLING_FRAMEWORKS_names[b]][str(i + 1)]["CONSUMED"] = \
            node_labels[node_ids.index(arrow_sources[i])]
        BGC_JSON_dict["CYCLING_FRAMEWORKS"][CYCLING_FRAMEWORKS_names[b]][str(i + 1)]["PRODUCED"] = \
            node_labels[node_ids.index(arrow_targets[i])]
        BGC_JSON_dict["CYCLING_FRAMEWORKS"][CYCLING_FRAMEWORKS_names[b]][str(i + 1)]["KINETICS"] = \
            arrow_kinetics[i]
        BGC_JSON_dict["CYCLING_FRAMEWORKS"][CYCLING_FRAMEWORKS_names[b]][str(i + 1)]["PARAMETER_NAMES"] = []
        BGC_JSON_dict["CYCLING_FRAMEWORKS"][CYCLING_FRAMEWORKS_names[b]][str(i + 1)]["PARAMETER_VALUES"] = {}


##############################
# Save to JSON file
with open(new_BGC_JSON_name, 'w') as outfile:
    json.dump(BGC_JSON_dict, outfile, separators=(',', ': '), indent=4)
