
import json
import numpy as np

###########################################
# graphml file name (define full path)
graphml_filepath = 'BGC_test.graphml'
CYCLING_FRAMEWORKS_name = 'global'
###########################################


# Don't change below this point
##
##
##
###########################################
###########################################

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

# flags
found_graph_flag = False
found_node_flag = False
found_arrow_flag = False


# open and read graphml file
my_file = open(graphml_filepath, "r")
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
            node_labels.append(content_l[loc_str:loc_end])

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
            arrow_lablkinet.append(content_l[loc_str:loc_end])

        else:
            continue

# Abort if arrows don't have all the info needed: name of transformation + kinetics
if len(arrow_lablkinet)/2 != len(arrow_ids):
    print('> ERROR: each arrow (transformation) needs two labels (transformation name and kinetics)')
    exit()

# Separate labels and kinetics
indexes_labels = np.linspace(0,len(arrow_lablkinet)-2,len(arrow_ids))
for i in range(0, len(indexes_labels)):
    arrow_labels.append(arrow_lablkinet[int(indexes_labels[i])])
for i in range(0, len(indexes_labels)):
    arrow_kinetics.append(arrow_lablkinet[int(indexes_labels[i]+1)])


#################################################
# Creating JSON BGC file
#################################################

#node_ids = []
#node_labels = []
#arrow_ids = []
#arrow_labels = []
#arrow_kinetics = []
#arrow_sources = []
#arrow_targets = []

BGC_JSON_dic = {}
BGC_JSON_dic["CHEMICAL_SPECIES"] = {}
BGC_JSON_dic["CHEMICAL_SPECIES"]["LIST"] = {}
BGC_JSON_dic["CHEMICAL_SPECIES"]["MOBILE_SPECIES"] = []
BGC_JSON_dic["CYCLING_FRAMEWORKS"] = {}

##############################
# Construct chemical list data
LIST_data = {}
for i in range(0, len(node_ids)):
    BGC_JSON_dic["CHEMICAL_SPECIES"]["LIST"][str(i + 1)] = node_labels[i]

##############################
# Construct cycling frameworks dictionary
BGC_JSON_dic["CYCLING_FRAMEWORKS"][CYCLING_FRAMEWORKS_name] = {}

# Add list of transformations and details for each of them
BGC_JSON_dic["CYCLING_FRAMEWORKS"][CYCLING_FRAMEWORKS_name]["LIST_TRANSFORMATIONS"] = {}

for i in range(0, len(arrow_ids)):

    # Add to list of transformations
    BGC_JSON_dic["CYCLING_FRAMEWORKS"][CYCLING_FRAMEWORKS_name]["LIST_TRANSFORMATIONS"][str(i + 1)] = arrow_labels[i]

    # Add details of transformation
    BGC_JSON_dic["CYCLING_FRAMEWORKS"][CYCLING_FRAMEWORKS_name][str(i + 1)] = {}
    BGC_JSON_dic["CYCLING_FRAMEWORKS"][CYCLING_FRAMEWORKS_name][str(i + 1)]["CONSUMED"] = \
        node_labels[node_ids.index(arrow_sources[i])]
    BGC_JSON_dic["CYCLING_FRAMEWORKS"][CYCLING_FRAMEWORKS_name][str(i + 1)]["PRODUCED"] = \
        node_labels[node_ids.index(arrow_targets[i])]
    BGC_JSON_dic["CYCLING_FRAMEWORKS"][CYCLING_FRAMEWORKS_name][str(i + 1)]["KINETICS"] = \
        arrow_kinetics[i]
    BGC_JSON_dic["CYCLING_FRAMEWORKS"][CYCLING_FRAMEWORKS_name][str(i + 1)]["PARAMETER_NAMES"] = []
    BGC_JSON_dic["CYCLING_FRAMEWORKS"][CYCLING_FRAMEWORKS_name][str(i + 1)]["PARAMETER_VALUES"] = {}


##############################
# Save to JSON file
JSON_filename = graphml_filepath[0:len(graphml_filepath)-8] + ".json"
with open(JSON_filename, 'w') as outfile:
    json.dump(BGC_JSON_dic, outfile, separators=(',', ': '), indent=4)
