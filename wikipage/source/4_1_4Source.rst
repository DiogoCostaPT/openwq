Source/Sink & External Fluxes (*Optional*)
==========================================

``Source/Sink`` loads and ``External Fluxes`` are optional entries. They use a similar data input structure. The differences are highlighted below.

The chemical sink and source files used by OpenWQ are defined in the master configuration file.
Each sink/source is defined in an individual ``json-block``, and you can add as many ``json`` files as desired.

**Principal Key 1**: ``METADATA``

+---------------+--------------------------------------------------+
| ``COMMNENT``  | Comments or relevant information about the data  |
+---------------+--------------------------------------------------+
| ``SOURCE``    | Additional information or additional comments    |
+---------------+--------------------------------------------------+

**Principal Key 2**: ``(i#)`` (input number in sequential order)

+--------------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| ``DATA_FORMAT``                      | Data format: ``"JSON"`` or ``"ASCII"``                                                                                  |
+--------------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| ``UNITS``                            | Units of input, e.g, ``"mg/l"``                                                                                         |
+--------------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| - ``COMPARTMENT_NAME``               | - Compartment name                                                                                                      |
| - *if Sink/Source*                   | - As defined in the interface-hydrolink.                                                                                |
+--------------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| - ``EXTERNAL_INPUTFLUX_NAME``        | - External Water Source name                                                                                            |
| - *else-if External Water Source*    | - As defined in the interface-hydrolink.                                                                                |
+--------------------------------------+-------------------------------------------------------------------------------------------------------------------------+

**OPTION SPECIFIC KEYS**

**-> If** ``DATA_FORMAT``: ``"JSON"`` or ``"ASCII"``

+--------------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| ``CHEMICAL_NAME``                    | - Chemical species names                                                                                                |
|                                      | - As defined in `Biogeochemical cycling configuration file <https://openwq.readthedocs.io/en/latest/4_1_3BGC.html#>`_   |
+--------------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| - ``TYPE``                           | Type of input,                                                                                                          |
| - *if Sink/Source only*              | Options: ``"source"`` or ``"sink"``                                                                                     |
+--------------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| ``DATA``                             | - Input data.                                                                                                           |
|                                      | - Format will depend on the ``DATA_FORMAT``                                                                             |
|                                      | - See details below.                                                                                                    |
+--------------------------------------+-------------------------------------------------------------------------------------------------------------------------+


**-> If** ``DATA_FORMAT``: ``"JSON"``

Then ``DATA`` with the time series of load has the following format:
``"(i#)"``: ``[YYYY,MM,DD,HH,MIN,SEC, ix,iy,iz,load, load type, time units (if continuous)]``

+---------------------+-------------------------------------------------------------------------+
| ``<YYYY>``          | Input year: ``"(i#)"`` or ``"all"``                                     |
+---------------------+-------------------------------------------------------------------------+
| ``<MM>``            | Input month: ``"(i#)"`` or ``"all"``                                    |
+---------------------+-------------------------------------------------------------------------+
| ``<DD>``            | Input day: ``"(i#)"`` or ``"all"``                                      |
+---------------------+-------------------------------------------------------------------------+
| ``<HH>``            | Input hour: ``"(i#)"`` or ``"all"``                                     |
+---------------------+-------------------------------------------------------------------------+
| ``<MIN>``           | Input minute: ``"(i#)"`` or ``"all"``                                   |
+---------------------+-------------------------------------------------------------------------+
| ``<SEC>``           | Input second:  ``"(i#)"`` or ``"all"``                                  |
+---------------------+-------------------------------------------------------------------------+
| ``<ix>``            | Input ix index: ``"(i#)"`` or ``"all"``                                 |
+---------------------+-------------------------------------------------------------------------+
| ``<iy>``            | Input iy index: ``"(i#)"`` or ``"all"``                                 |
+---------------------+-------------------------------------------------------------------------+
| ``<iz>``            | Input iz index: ``"(i#)"`` or ``"all"``                                 |
+---------------------+-------------------------------------------------------------------------+
| ``<load>``          | Input load: ``"(f#)"``                                                  |
+---------------------+-------------------------------------------------------------------------+
| *if Sink/Source*  (two additional entries)                                                    |
+---------------------+-------------------------------------------------------------------------+
| ``<load type>``     | Input load type: ``"discrete"``or ``"continuous"``                      |
+---------------------+-------------------------------------------------------------------------+
| ``<time units>``    | Time units of load if load type is ``"continuous"``, e.g., ``"1/day"``  |
+---------------------+-------------------------------------------------------------------------+

**-> If** ``DATA_FORMAT``: ``"ASCII"``

Then ``DATA`` with the time series of load has the same format above but it's provided through an ASCII file. The following additional json-keys need be included.

+-------------------------------+-------------------------------------------------------------------------+
| ``"FILEPATH"``                | Path to ASCII file with the input data                                  |
+-------------------------------+-------------------------------------------------------------------------+
| ``"DELIMITER"``               | - Delimiter used in the ASCII file                                      |
|                               | - e.g., ``","``                                                         |
+-------------------------------+-------------------------------------------------------------------------+
|``"NUMBER_OF_HEADER_ROWS"``    | - Number of header rows in the ASCII file to skip                       |
|                               | - e.g., ``3``                                                           |
+-------------------------------+-------------------------------------------------------------------------+
|``"HEADER_KEY_ROW"``           | - Number of header row with input keys.                                 |
|                               | - This row needs to contain all the keys listed above                   |
|                               | - YYYY, MM, DD, HH, MIN, SEC, ix, iy, iz, load, load type, time units   |
+-------------------------------+-------------------------------------------------------------------------+


Example of input ``JSON`` file:

.. code-block:: json

    {
        "METADATA": {
            "Comment": "synthetic loading",
            "Source": "test_1"
        },
        "1": {
            "Chemical_name": "species_A",
            "Compartment_name": "SCALARAQUIFER",
            "Type": "source",
            "Units": "kg",
            "Data_Format": "JSON",
            "Data": {
                "1": ["all","all","all","all","all","all","all","all","all",0.0001,"discrete"],
                "2": ["all","all","all","all","all","all","all","all","all",0.0001,"continuous","min"],
                "3": [2018,"all",4,6,30,30,1,1,1,100,"discrete"],
                "4": [2019,"all",4,6,30,"all",1,1,1,10,"continuous","min"],
                "5": [2019,2,4,6,30,15,1,1,1,350,"discrete"]
            }
        },
        "2": {
            "Chemical_name": "species_B",
            "Compartment_name": "SCALARAQUIFER",
            "Type": "source",
            "Units": "kg",
            "Data_Format": "ASCII",
            "Data": {
                "Filepath": "SS_speciesA_ScalarAquifer_test.csv",
                "Delimiter": ",",
                "Number_of_header_rows": 3,
                "Header_key_row": 3
            }
        }
    }

File: SS_speciesA_ScalarAquifer_test.csv

.. image:: ss_ascii.png

**-> If** ``DATA_FORMAT``: ``"HD5F"`` (only applicable to ``External Fluxes``)

This is used in the cases where we want to use the output of another host_model-OpenWQ model for the ``"External Fluxes"`` concentrations.
This is useful when we use two externally coupled models, such as a hydrological model using precipitation data from an atmospheric model.
In these cases, we need to have both models coupled to OpenWQ, such as shown in the diagram.

.. image:: coupled_host_models.PNG
    :width: 300
    :alt: Coupling two host_model-OpenWQ coupled models


These are the general steps to run both host_model-OpenWQ models:

1. Run the upstream host_model-OpenWQ coupled model. Make sure to export data for the upstream model compartment from where the inter-model fluxes originate.
2. Run the upstream host_model-OpenWQ coupled model. Make sure to sep up the EWF json with the additional key:values listed below.

+---------------------------------+-----------------------------------------------------------------------------------------------------------------+
| ``"EXTERNAL_COMPARTMENT_NAME"`` | - Name of the compartment in the upstream model from where the inter-model fluxes originate                     |
|                                 | - Example: "ATMOSPHERE_LAYER"                                                                                   |
+---------------------------------+-----------------------------------------------------------------------------------------------------------------+
| ``"INTERACTION_INTERFACE"``     | - Array specifying the interface elements between the upstream and downstream models                            |
|                                 | - Format: [``"(i#)"`` or "all, ``"(i#)"`` or "all", ``"(i#)"`` or "all"]                                        |
|                                 | - Example: ["all", "all", 1] (for interaction along the x-y plane) (see diagrams below)                         |
+---------------------------------+-----------------------------------------------------------------------------------------------------------------+
| ``"INTERPOLATION"``             | - Method to time-interpolate the timestamp-concentrations provided in the ``"HD5F"`` files                      |
|                                 | - Options: ``STEP``, ``NEAREST``, and ``LINEAR``                                                                |
+---------------------------------+-----------------------------------------------------------------------------------------------------------------+
| ``"FOLDERPATH"``                | - Path to ``"HD5F"`` file that contains the compartment ``"EXTERNAL_COMPARTMENT_NAME"`` from the upstream model.|
|                                 | - NOTE: Make sure that the ``"Log_OpenWQ.txt"`` produced by the upstream-OpenWQ instance is also in this folder.|
+---------------------------------+-----------------------------------------------------------------------------------------------------------------+

**Diagrams showing examples of how to set up ``"INTERACTION_INTERFACE"`` for different upstream-downstream host-model configuration:**:

.. image:: ewf_h5_loading_interface_convention.PNG
    :width: 500
    :alt: Interface characterization for two host_model-OpenWQ coupled models

Example of input ``JSON`` file:

.. code-block:: json

    {
        "METADATA": {
            "Comment": "synthetic loading",
            "Source": "test_1"
        },
        "1": {
            "DATA_FORMAT": "HDF5",
            "UNITS": "mg/l",
            "EXTERNAL_COMPARTMENT_NAME": "ATMOSPHERE_LAYER",
            "EXTERNAL_INPUTFLUX_NAME": "PRECIP",
            "INTERACTION_INTERFACE": ["all", 1, 1],
            "INTERPOLATION": "LINEAR",
            "FOLDERPATH": "openwq_ewf_h5"
	    }
    }

The JSON file supports C/C++ syntax for comments: single-line comment (``//``) or comment blocks (``/*`` and ``*/``).

The symbol ``(i#)`` refers to a integer number sequence.. The symbol ``(s#)`` refers to a string input. The symbol ``<f#>`` refers to a float input value.
