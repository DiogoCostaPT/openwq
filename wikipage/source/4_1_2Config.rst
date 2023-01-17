Configuration
==================================

The transportation and biogeochemical configuration file is a JSON file made up of the following components:

**Principal Key 1**: ``BIOGEOCHEMISTRY_CONFIGURATION``

+---------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------+
| ``<Compt_Name>`` -> ``CYCLING_FRAMEWORK``                     | - List of cycling frameworks                                                                                          |
|                                                               | - As defined in `Biogeochemical cycling configuration file <https://openwq.readthedocs.io/en/latest/4_1_3BGC.html#>`_ |
|                                                               | - Format: ``[<s#>, <s#>, ...]``                                                                                       |
|                                                               | - Example: ``["N_org", "N_inorg", "P_org", "P_inorg"]``                                                               |
+---------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------+
| ``<Compt_Name>`` -> ``INITIAL_CONDITIONS`` -> ``DATA_FORMAT`` | - Format of IC input.                                                                                                 |
|                                                               | - Options: ``"JSON"`` or ``"H5"``                                                                                     |
+---------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------+

**ADDITIONAL KEYS**:

**-> If** ``DATA_FORMAT``: ``"JSON"``

Then IC data in ``DATA`` is provided in the following format:
``<Chem_Name>``: {``"(i#)"``: ``[ix,iy,iz,conc/load, units]``}.
If no information is provided for a particular compartment and/or chemical species, the initial conditions (concentrations and masss) will be assumed ZERO.

+---------------------+-------------------------------------------------------------------------+
| ``<i#>``            | Input numbering; as many entries as needed.                             |
+---------------------+-------------------------------------------------------------------------+
| ``<ix>``            | Input ix index: ``"(i#)"`` or ``"all"``                                 |
+---------------------+-------------------------------------------------------------------------+
| ``<iy>``            | Input iy index: ``"(i#)"`` or ``"all"``                                 |
+---------------------+-------------------------------------------------------------------------+
| ``<iz>``            | Input iz index: ``"(i#)"`` or ``"all"``                                 |
+---------------------+-------------------------------------------------------------------------+
| ``<conc/load>``     | Input load: ``"(f#)"``  (concentration or mass; units specified below)  |
+---------------------+-------------------------------------------------------------------------+
| ``<units>``         | Units of the previous field, e.g., ``"mg/l"``, ```kg``                  |
+---------------------+-------------------------------------------------------------------------+

**-> If** ``DATA_FORMAT``: ``"H5"``
Then IC data is provided using H5 files with the format as exported when running the model.
The following additional json-keys need be included.

+---------------------+--------------------------------------------------------------------------+
| ``FOLDERPATH``      | Folder that contains the ``*.h5`` files containing the IC data.          |
+---------------------+--------------------------------------------------------------------------+
| ``TIMESTAMP``       | - Timestamp to be extracted.                                             |
|                     | - Format: ``YYYYMMMDD-HH:MM:SS``                                         |
|                     | - Example: ``"1950Apr01-12:00:00"``                                      |
+---------------------+--------------------------------------------------------------------------+
| ``UNITS``           | - Units of the input data, which will be used to located the IC h5 files.|
|                     | - Example: ``"mg"``, ``"mg/l"``                                          |
+---------------------+--------------------------------------------------------------------------+

The JSON file supports C/C++ syntax for comments: single-line comment (``//``) or comment blocks (``/*`` and ``*/``).

The symbol ``<Compt_Name>`` refers to compartment names as defined by the host model. We can add as many of such compartment json-key blocks as desired (see example below). The compartment names should match those defined in the hostmodel.

The symbol ``<Chem_Name>`` refers to chemical species names as defined in the `Biogeochemical cycling configuration file <https://openwq.readthedocs.io/en/latest/4_1_3BGC.html#>`_.

The symbol ``(i#)`` refers to a integer number sequence.. The symbol ``(s#)`` refers to a string input. The symbol ``<f#>`` refers to a float input value.

Example:

.. code-block:: json

  {
      "BIOGEOCHEMISTRY_CONFIGURATION":{
          "RUNOFF":{
              "CYCLING_FRAMEWORK": ["N_inorg","P_inorg"],
              "INITIAL_CONDITIONS":{
                "species_A": {
                    "1": ["all","all","all",2,"mg/l"],
                    "2": [1,5,1,2,"mg/l"]
                },
                "species_B": {
                    "1": ["all","all","all",5,"kg"]
                }
              }
          },
          "SOIL_RECHR":{
              "CYCLING_FRAMEWORK": ["N_inorg","P_inorg","N_soil_org","P_soil_org"],
              "INITIAL_CONDITIONS":{
                    "Data_Format": "H5",
                    "FOLDERPATH": "openwq_ic_h5",
                    "TIMESTAMP": "1950Apr01-12:00:00",
                    "UNITS": "mg"
              }
          }
      }
  }