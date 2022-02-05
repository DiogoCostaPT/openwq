Master
==================================

The master configuration is a JSON file that provides OpenWQ with information and instructions to run a simulation. It contains key-values pairs defined below: 

+-----------------------------+-----------------------------------------------------------------------+
| Key                         | Description                                                           |
+=============================+=======================================================================+
| PROJECT_NAME                | Project name string                                                   |
+-----------------------------+-----------------------------------------------------------------------+
| GEOGRAPHICAL_LOCATION       | Project location string                                               |
+-----------------------------+-----------------------------------------------------------------------+
| AUTHORS                     | Model run authors                                                     |
+-----------------------------+-----------------------------------------------------------------------+
| DATE                        | Model run data                                                        |
+-----------------------------+-----------------------------------------------------------------------+
| COMMENT                     | Additional documentation information                                  |
+-----------------------------+-----------------------------------------------------------------------+
| COMPUTATIONAL_SETTINGS      | Other run settings                                                    |
+-----------------------------+-----------------------------------------------------------------------+
| RUN_MODE_DEBUG              | Run model debugging flag (true/false)                                 |
+-----------------------------+-----------------------------------------------------------------------+
| USE_NUM_THREADS             | "all" or integer of number of threads to use (e.g., 10)               |
+-----------------------------+-----------------------------------------------------------------------+
| OPENWQ_INPUT                | Input file locations                                                  |
+-----------------------------+-----------------------------------------------------------------------+
| CONFIG_FILEPATH             | Main configuration file path                                          |
+-----------------------------+-----------------------------------------------------------------------+
| BGC_CYCLES_FILEPATH         | Biogeochemical cycling configuration file path                        |
+-----------------------------+-----------------------------------------------------------------------+
| SINKSOURCE_FILES            | Numbered pairs of LABEL and FILEPATH for chemical source files        |
|                             | e.g. ``fertilizer_N``                                                 |
+-----------------------------+-----------------------------------------------------------------------+
| OPENWQ_OUTPUT               | Output file locations and configuration                               |
+-----------------------------+-----------------------------------------------------------------------+
| RESULTS_FOLDERPATH          | Output folder path                                                    |
+-----------------------------+-----------------------------------------------------------------------+
| FORMAT                      | Output file format (one of VTU, CSV, or HDF5)                         | 
+-----------------------------+-----------------------------------------------------------------------+
| CHEMICAL_SPECIES            | Chemical species to include in output files                           |
+-----------------------------+-----------------------------------------------------------------------+
| UNITS                       | Units of outputs of chemical species                                  |
+-----------------------------+-----------------------------------------------------------------------+
| COMPARTMENTS_AND_CELLS      | Names of compartments and indices to output                           |
+-----------------------------+-----------------------------------------------------------------------+
| TIMESTEP                    | Temporal resolution of output file e.g., ``[1,"hour"]``               |
+-----------------------------+-----------------------------------------------------------------------+

Example:

.. code-block:: json

  {
      "PROJECT_NAME": "My project",
      "GEOGRAPHICAL_LOCATION": "Neverland",
      "AUTHORS": "Barack Obama",
      "DATE": "Jan 2009",
      "COMMENT": "This is my first project as the 44th president of the United States",
      "COMPUTATIONAL_SETTINGS":{
        "RUN_MODE_DEBUG": true,
        "USE_NUM_THREADS": "all"
      },
      "OPENWQ_INPUT": {
        "CONFIG_FILEPATH": "openWQ_config.json",
         "BGC_CYCLES_FILEPATH": "openWQ_BGC_cycling.json",
        "SINKSOURCE_FILES": {
          "1": {
            "LABEL": "fertilizer_N",
            "FILEPATH": "openWQ_source_fertN.json"
          },
          "2":
          {
            "LABEL": "fertilizer_P",
            "FILEPATH": "openWQ_source_fertP.json"
          }
        }
      },
    "OPENWQ_OUTPUT": {
      "RESULTS_FOLDERPATH": "Output_OpenWQ",
      "FORMAT": "HDF5",
      "CHEMICAL_SPECIES":["NO3","NH4","N2","DON","labile_orgN","refractory_orgN","SRP","DOP","partP","labile_orgP","refractory_orgP"],
      "UNITS":"mg/l",
      "COMPARTMENTS_AND_CELLS": {
        "RUNOFF": {
          "1": [304,1,1],
          "2": [281,1,1]
        },
        "SOIL_RECHR": {
          "1": [304,1,1],
          "2": [281,1,1]
        },
        "GW": {
        	"1": [304,1,1],
            "2": [281,1,1]
        }
      },
      "TIMESTEP": [1,"hour"]
    }
  }