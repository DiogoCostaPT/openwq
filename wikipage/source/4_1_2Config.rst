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
| ``<Compt_Name>`` -> ``INITIAL_CONDITIONS``                    | - Initial concentrations.                                                                                             |
|                                                               | - See details below.                                                                                                  |
+---------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------+

**ADDITIONAL KEYS**:

The value of the ``INITIAL_CONDITIONS`` key has the following format:
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
                    "1": ["all","all","all",5,"mg/l"]
                }
              }
          },
          "SOIL_RECHR":{
              "CYCLING_FRAMEWORK": ["N_inorg","P_inorg","N_soil_org","P_soil_org"],
              "INITIAL_CONDITIONS":{
                "species_A": {
                    "1": ["all","all","all",3,"mg/l"],
                },
                "species_B": {
                    "1": ["all","all","all",0,"mg/l"],
                    "2": ["all","all","all",0,"mg/l"],
                    "3": ["all","all","all",0,"mg/l"],
                    "4": ["all","all","all",0,"mg/l"]
                }
              }
          }
      }
  }