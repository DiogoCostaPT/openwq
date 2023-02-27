Master
==================================

The master configuration is a JSON file that provides OpenWQ with information and instructions to run a simulation. It contains key-values pairs defined below: 

**Principal Key 1**: General Project Information

+-----------------------------+--------------------------------------+
| ``PROJECT_NAME``            | Project name string                  |
+-----------------------------+--------------------------------------+
| ``GEOGRAPHICAL_LOCATION``   | Project location string              |
+-----------------------------+--------------------------------------+
| ``AUTHORS``                 | Model run authors                    |
+-----------------------------+--------------------------------------+
| ``DATE``                    | Model run date                       |
+-----------------------------+--------------------------------------+
| ``COMMENT``                 | Additional documentation information |
+-----------------------------+--------------------------------------+

**Principal Key 2**: ``COMPUTATIONAL_SETTINGS``

+----------------------+---------------------------------------------------------+
| ``RUN_MODE_DEBUG``   | Run model debugging flag (true/false)                   |
+----------------------+---------------------------------------------------------+
| ``USE_NUM_THREADS``  | "all" or integer of number of threads to use (e.g., 10) |
+----------------------+---------------------------------------------------------+

**Principal Keys 3**: ``OPENWQ_INPUT``

+-------------------------------------------------------+-------------------------------------------------------------------------------------------------------+
| ``CONFIG_FILEPATH``                                   | `Main configuration file path <https://openwq.readthedocs.io/en/latest/4_1_2Config.html>`_            |
+-------------------------------------------------------+-------------------------------------------------------------------------------------------------------+
| - ``EXTERNAL_WATER_FLUXES``                           | Numbered pairs of LABEL and FILEPATH for external-fluxes input files                                  |
| - *Optional*                                          |                                                                                                       |
+-------------------------------------------------------+-------------------------------------------------------------------------------------------------------+
| - ``EXTERNAL_WATER_FLUXES`` -> ``(#)`` -> ``LABEL``   | Label id for input file                                                                               |
| - *Optional*                                          |                                                                                                       |
+-------------------------------------------------------+-------------------------------------------------------------------------------------------------------+
| - ``EXTERNAL_WATER_FLUXES`` -> ``(#)`` -> ``FILEPATH``| File path to `External Water Fluxes file <https://openwq.readthedocs.io/en/latest/4_1_4Source.html>`_ |
| - *Optional*                                          |                                                                                                       |
+-------------------------------------------------------+-------------------------------------------------------------------------------------------------------+
| - ``SINK_SOURCE``                                     | Numbered pairs of LABEL and FILEPATH for chemical-sources input files                                 |
| - *Optional*                                          |                                                                                                       |
+-------------------------------------------------------+-------------------------------------------------------------------------------------------------------+
| - ``SINK_SOURCE`` -> ``(#)`` -> ``LABEL``             | Label id for input file                                                                               |
| - *Optional*                                          |                                                                                                       |
+-------------------------------------------------------+-------------------------------------------------------------------------------------------------------+
| - ``SINK_SOURCE`` -> ``(#)`` -> ``FILEPATH``          | Path to `Sink and Source file <https://openwq.readthedocs.io/en/latest/4_1_4Source.html>`_            |
+-------------------------------------------------------+-------------------------------------------------------------------------------------------------------+

**Principal Keys 4**: ``MODULES``

+-----------------------------------------------------+---------------------------------------------------------------------------------------------------------------+
| ``BIOGEOCHEMISTRY`` -> ``MODULE_NAME``              | Biogeochemical cycling module choice                                                                          |
+-----------------------------------------------------+---------------------------------------------------------------------------------------------------------------+
| ``BIOGEOCHEMISTRY`` -> ``MODULE_CONFIG_FILEPATH``   | Path to `Biogeochemical cycling configuration file <https://openwq.readthedocs.io/en/latest/4_1_3BGC.html#>`_ |
+-----------------------------------------------------+---------------------------------------------------------------------------------------------------------------+
| ``TRANSPORT_EROSION`` -> ``MODULE_NAME``            | Transport-Erosion module choice                                                                               |
+-----------------------------------------------------+---------------------------------------------------------------------------------------------------------------+
| ``TRANSPORT_EROSION`` -> ``MODULE_CONFIG_FILEPATH`` | Transport-Erosion configuration file path                                                                     |
+-----------------------------------------------------+---------------------------------------------------------------------------------------------------------------+

**Principal Keys 5**: ``OPENWQ_OUTPUT``

+----------------------------+-----------------------------------------------------------------------------------------------------+
| ``RESULTS_FOLDERPATH``     | Output folder path                                                                                  |
+----------------------------+-----------------------------------------------------------------------------------------------------+
| ``FORMAT``                 | Output file format ( `VTU, CSV, or HDF5 <https://openwq.readthedocs.io/en/latest/4_2_1file.html>`_) |
+----------------------------+-----------------------------------------------------------------------------------------------------+
| ``CHEMICAL_SPECIES``       | Chemical species to include in output files                                                         |
+----------------------------+-----------------------------------------------------------------------------------------------------+
| ``UNITS``                  | Units of outputs of chemical species                                                                |
+----------------------------+-----------------------------------------------------------------------------------------------------+
| ``NO_WATER_CONC_FLAG``     | Default value for concentration when there is not water                                             |
+----------------------------+-----------------------------------------------------------------------------------------------------+
| ``COMPARTMENTS_AND_CELLS`` | Names of compartments and indices to output                                                         |
+----------------------------+-----------------------------------------------------------------------------------------------------+
| ``TIMESTEP``               | Temporal resolution of output file, e.g., ``[1,"hour"]``                                            |
+----------------------------+-----------------------------------------------------------------------------------------------------+

**Principal Keys 5**: ``COMPARTMENTS_AND_CELLS``

+------------------------------------------------------------+---------------------------------------------------------------------+
| ``COMPARTMENTS_AND_CELLS`` -> ``<Compt_Name>`` -> ``"all"``| This will print all cells of the compartment                        |
+------------------------------------------------------------+---------------------------------------------------------------------+
| ``COMPARTMENTS_AND_CELLS`` -> ``<Compt_Name>`` -> ``(i#)`` | ix, iy and iz cells locations, e.g., ``[1,"all","all"]``            |
+------------------------------------------------------------+---------------------------------------------------------------------+

The JSON file supports C/C++ syntax for comments: single-line comment (``//``) or comment blocks (``/*`` and ``*/``).
| Names of compartments and indices to output                                                         |

The symbol ``(i#)`` refers to a integer number sequence.

Example:

.. code-block:: json

    {
        "PROJECT_NAME": "My_Project",
        "GEOGRAPHICAL_LOCATION": "Saskatoon",
        "AUTHORS": "Diogo Costa",
        "DATE": "Aug_2022",
        "COMMENT": "This is my first project.",
        "COMPUTATIONAL_SETTINGS": {
            "RUN_MODE_DEBUG": true,
            "USE_NUM_THREADS": "all"
        },
        "OPENWQ_INPUT": {
            "CONFIG_FILEPATH": "openWQ_config.json",
            "EXTERNAL_WATER_FLUXES": {
                "1": {
                    "LABEL": "Rainfall_example",
                    "FILEPATH": "openWQ_rainfall.json"
                }
            },
            "SINK_SOURCE": {
                "1": {
                    "LABEL": "External_Load_example",
                    "FILEPATH": "openWQ_source_batchSimulations.json"
                }
            }
        },
        "MODULES": {
            "BIOGEOCHEMISTRY": {
                "MODULE_NAME": "OPENWQ_NATIVE_BGC",
                "MODULE_CONFIG_FILEPATH": "openWQ_MODULE_BGC_cycling.json"
            },
            "TRANSPORT_EROSION": {
                "MODULE_NAME": "OPENWQ_NATIVE_TE_NO_ADVDISP"
                "MODULE_CONFIG_FILEPATH": "openWQ_MODULE_TE_batchSimulations.json"
            }
        },
        "OPENWQ_OUTPUT": {
            "RESULTS_FOLDERPATH": "Output_OpenWQ",
            "FORMAT": "HDF5",
            "CHEMICAL_SPECIES": ["species_A", "species_B"],
            "UNITS": "kg",
            "NO_WATER_CONC_FLAG": -9999,
            "COMPARTMENTS_AND_CELLS": {
                "COMPART_1": "all",
                "COMPART_2": {
                    "1": [4, "all", 2],
                },
                "COMPART_3": {
                    "1": [1, 20, 5],
                }
            },
            "TIMESTEP": [1, "day"]
        }
    }