Configuration
==================================

The transportation and biogeochemical configuration file is a JSON file made up of the following components:

+-----------------------------------------------------------------------------------------------------------------------+--------------------------------------------+
| Key -> Sub-key(s)                                                                                                     | Description                                |
+===============================+====================================================================================================================================+
| ``BIOGEOCHEMISTRY_CONFIGURATION``                                                                                                                                  |
+-----------------------------------------------------------------------------------------------------------------------+--------------------------------------------+
| ``BIOGEOCHEMISTRY_CONFIGURATION`` -> ``(Compt_Name)``                                                                                                              |
+-----------------------------------------------------------------------------------------------------------------------+--------------------------------------------+
| ``BIOGEOCHEMISTRY_CONFIGURATION`` -> ``(Compt_Name)`` -> ``CYCLING_FRAMEWORK`` -> [``(s#)``, ``(s#)``]                | List of cycling frameworks                 |
+-----------------------------------------------------------------------------------------------------------------------+--------------------------------------------+
| ``BIOGEOCHEMISTRY_CONFIGURATION`` -> ``(Compt_Name)`` -> ``INITIAL_CONDITIONS`` -> ``(Chem_Name)`` -> ``[(f#),(s#)]`` | Initial concentrations ``[value, units]``  |
+-----------------------------------------------------------------------------------------------------------------------+--------------------------------------------+

The JSON file supports C/C++ syntax for comments: single-line comment (``//``) or comment blocks (``/*`` and ``*/``).
The symbol ``(Compt_Name)`` refers to compartment names as defined by the host model. We can add as many of such compartment json-key blocks as desired. The compartment names should match those defined in the hostmodel.
The symbol ``(Chem_Name)`` refers to chemical species names as defined in the `Biogeochemical cycling configuration file <https://openwq.readthedocs.io/en/latest/4_1_3BGC.html#>`_.
The symbol ``(f#)`` refers to a float input value. In this case, it refers to the concentration initial conditions.
The symbol ``(s#)`` refers to a string input. In this case, it refers to the concentration units.

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
          "GW":{
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
          }
      } 
  }