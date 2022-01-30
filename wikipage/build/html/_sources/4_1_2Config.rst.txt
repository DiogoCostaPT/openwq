Configuration file
==================================

The transportation and biogeochemical configuration file is a JSON file made up of the following components:

+-------------------------------+-----------------------------------------------------------------+
| Key                           | Description                                                     |
+===============================+=================================================================+
| TRANSPORT_CONFIGURATION       | Define transport mechanisms                                     |                 
+-------------------------------+-----------------------------------------------------------------+
| BIOGEOCHEMISTRY_CONFIGURATION | Define cycling framework and initial conditions                 | 
|                               | for each model compartment                                      |
+-------------------------------+-------------------+---------------------------------------------+
|                               | Model compartment | Defined by host model                       |         
+-------------------------------+-------------------+---------------------------------------------+
|                               | CYCLING_FRAMEWORK | List cycling frameworks                     |
+-------------------------------+-------------------+---------------------------------------------+
|                               | INITIAL_CONDITIONS| Initial conditions in format                |
|                               |                   | ``chemical:[value, units]``                 |
+-------------------------------+-------------------+---------------------------------------------+

The JSON file supports C/C++ syntax for comments: single-line comment (``//``) or comment blocks (``/*`` and ``*/``). 

Example:

.. code-block:: json

  {
      "TRANSPORT_CONFIGURATION": {
          "disperson_x_m2/s": 0.3,
          "disperson_y_m2/s": 0.3,
          "disperson_z_m2/s": 0.3
      },
      "BIOGEOCHEMISTRY_CONFIGURATION":{
          "SWE":{
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
          "SSR":{
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
          "SD":{
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
          "SOIL_LOWER":{
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
          "SURFSOIL":{
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