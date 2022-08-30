Configuration
==================================

The transportation and biogeochemical configuration file is a JSON file made up of the following components:

**Principal Key 1**: ``BIOGEOCHEMISTRY_CONFIGURATION``

+---------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------+
| ``<Compt_Name>`` -> ``CYCLING_FRAMEWORK``                     | List of cycling frameworks                                                                                            |
|                                                               | As defined in `Biogeochemical cycling configuration file <https://openwq.readthedocs.io/en/latest/4_1_3BGC.html#>`_   |
|                                                               | Format: ``[<s#>, <s#>, ...]``                                                                                         |
|                                                               | Example: ``["N_org", "N_inorg", "P_org", "P_inorg"]``                                                                 |
+---------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------+
| ``<Compt_Name>`` -> ``INITIAL_CONDITIONS`` -> ``<Chem_Name>`` | Initial concentrations ``[value, units]``                                                                             |
|                                                               | Format: ``[<f#>,<s#>]``                                                                                               |
|                                                               | Example: ``"species_A": [10,"mg"]``                                                                                   |
+---------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------+

The JSON file supports C/C++ syntax for comments: single-line comment (``//``) or comment blocks (``/*`` and ``*/``).

The symbol ``<Compt_Name>`` refers to compartment names as defined by the host model. We can add as many of such compartment json-key blocks as desired (see example below). The compartment names should match those defined in the hostmodel.

The symbol ``<Chem_Name>`` refers to chemical species names as defined in the `Biogeochemical cycling configuration file <https://openwq.readthedocs.io/en/latest/4_1_3BGC.html#>`_.

The symbol ``<f#>`` refers to a float input value. In this case, it refers to the concentration initial conditions. The symbol ``<s#>`` refers to a string input.

Example:

.. code-block:: json

  {
      "BIOGEOCHEMISTRY_CONFIGURATION":{
          "RUNOFF":{
              "CYCLING_FRAMEWORK": ["N_inorg","P_inorg"],
              "INITIAL_CONDITIONS":{
                  "NO3": [7,"mg/l"],
                  "NH4": [11,"mg/l"],
                  "N2": [2,"ug/l"],
                  "DON": [2,"mg/l"],
                  "labile_orgN": [2.5,"kg/m3"],
                  "refractory_orgN": [3,"kg/m3"],
                  "SRP": [3,"mg/l"],
                  "DOP": [2,"mg/l"],
                  "partP": [2,"mg/l"],
                  "labile_orgP": [2.5,"kg/m3"],
                  "refractory_orgP": [3,"kg/m3"]
              }
          },
          "SOIL_RECHR":{
              "CYCLING_FRAMEWORK": ["N_inorg","P_inorg","N_soil_org","P_soil_org"],
              "INITIAL_CONDITIONS":{
                  "NO3": [5,"mg/l"],
                  "NH4": [3,"mg/l"],
                  "N2": [4,"ug/l"],
                  "DON": [2,"mg/l"],
                  "labile_orgN": [4,"kg/m3"],
                  "refractory_orgN": [5,"kg/m3"],
                  "SRP": [2,"mg/l"],
                  "DOP": [7,"mg/l"],
                  "partP": [2,"mg/l"],
                  "labile_orgP": [4,"kg/m3"],
                  "refractory_orgP": [6,"kg/m3"]
              }
          },
          "GW":{
              "CYCLING_FRAMEWORK": ["N_inorg","P_inorg"],
              "INITIAL_CONDITIONS":{
                  "NO3": [8,"mg/l"],
                  "NH4": [1,"mg/l"],
                  "N2": [3,"ug/l"],
                  "DON": [4,"mg/l"],
                  "labile_orgN": [7,"kg/m3"],
                  "refractory_orgN": [8,"kg/m3"],
                  "SRP": [2,"mg/l"],
                  "DOP": [3,"mg/l"],
                  "partP": [5,"mg/l"],
                  "labile_orgP": [4,"kg/m3"],
                  "refractory_orgP": [2,"kg/m3"]
              }
          }
      } 
  }