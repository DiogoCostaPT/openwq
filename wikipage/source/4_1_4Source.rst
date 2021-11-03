Source file 
==================================

The chemical sink and source files used by OpenWQ are defined in the master configuration file. Each sink/source is defined in an individual .json file. 

.. list-table:: 
   :header-rows: 1

   * - Key 
     - Description
   * - METADATA
     - Comment and data source strings for storing metadata  
   * - Chemical_name
     - Species name, e.g., ``NO3``
   * - Compartment_name
     - Compartment name as used in host model
   * - Type
     - Define type, e.g., ``source`` or ``sink``
   * - Units
     - Units of source/sink load 
   * - Data
     - Time series of load in format ``{YYYY,MM,DD,HH,ix,iy,iz,load}``

The JSON file supports C/C++ syntax for comments: single-line comment (``//``) or comment blocks (``/*`` and ``*/``). 

Example:


.. code-block:: json 

    {
        "METADATA": {
            "Comment": "N loading from fertilizer",
            "Source": "bla bla"
        },
        "1": {
            "Chemical_name": "NO3",
            "Compartment_name": "SURFSOIL",
            "Type": "source",
            "Units": "kg",
            "Data": { // YYYY,MM,DD,HH,ix,iy,iz,load
                "1": [2017,8,20,12,30,304,1,1,20000],
                "2": [2017,8,20,12,30,284,1,1,20000],
                "3": [2018,8,20,12,30,304,1,1,20000],
                "4": [2018,8,20,12,30,800,1,1,20000],
                "5": [2019,8,20,12,30,304,1,1,20000],
                "6": [2019,8,20,12,30,1,304,2,20000]
                }
        },
        "2": {
            "Chemical_name": "NH4",
            "Compartment_name": "SURFSOIL",
            "Type": "source",
            "Units": "kg",
            "Data": { // YYYY,MM,DD,HH,ix,iy,iz,load
                "1": [2017,8,20,12,30,304,1,1,20000],
                "2": [2017,8,20,12,30,284,1,1,20000],
                "3": [2018,8,20,12,30,304,1,1,20000],
                "4": [2018,8,20,12,30,800,1,1,20000],
                "5": [2019,8,20,12,30,304,1,1,20000],
                "6": [2019,8,20,12,30,1,304,2,20000]
            }
        },
        "3": {
            "Chemical_name": "NO3",
            "Compartment_name": "SOIL_RECHR",
            "Type": "source",
            "Units": "kg",
            "Data": { // YYYY,MM,DD,HH,ix,iy,iz,load
                "1": [2017,8,20,12,30,304,1,1,20000],
                "2": [2017,8,20,12,30,284,1,1,20000],
                "3": [2018,8,20,12,30,304,1,1,20000],
                "4": [2018,8,20,12,30,800,1,1,20000],
                "5": [2019,8,20,12,30,304,1,1,20000],
                "6": [2019,8,20,12,30,1,304,2,20000]
            }
        },
        "4": {
            "Chemical_name": "NH4",
            "Compartment_name": "SOIL_RECHR",
            "Type": "source",
            "Units": "kg",
            "Data": { // YYYY,MM,DD,HH,ix,iy,iz,load
                "1": [2017,8,20,12,30,304,1,1,20000],
                "2": [2017,8,20,12,30,284,1,1,20000],
                "3": [2018,8,20,12,30,304,1,1,20000],
                "4": [2018,8,20,12,30,800,1,1,20000],
                "5": [2019,8,20,12,30,304,1,1,20000],
                "6": [2019,8,20,12,30,1,304,2,20000]
            }
        }
    }