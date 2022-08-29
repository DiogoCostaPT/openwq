Source/Sink
==================================

The chemical sink and source files used by OpenWQ are defined in the master configuration file. Each sink/source is defined in an individual .json file. 

**Principal Key 1**: ``METADATA``

+-----------+--------------------------------------------------+
| Comment   | Comments or relevant information about the data  |
| Source    | Additional information or additional comments    |
+-----------+--------------------------------------------------+

**Principal Key 2**: ``(i#)`` (input number in sequential order)

+-------------------------+-------------------------------------------------------------------------------------------------------------------------+
| ``CHEMICAL_NAME``       | - Chemical species names                                                                                                |
|                         | - As defined in `Biogeochemical cycling configuration file <https://openwq.readthedocs.io/en/latest/4_1_3BGC.html#>`_   |
+-------------------------+-------------------------------------------------------------------------------------------------------------------------+
| ``COMPARTMENT_NAME``    | - Compartment name                                                                                                      |
|                         | - As defined in the host model.                                                                                         |
+-------------------------+-------------------------------------------------------------------------------------------------------------------------+
| ``TYPE``                | Type of input, ``"source"`` or ``"sink"``                                                                               |
+-------------------------+-------------------------------------------------------------------------------------------------------------------------+
| ``UNITS``               | Units of input, e.g, ``"mg/l"``                                                                                         |
+-------------------------+-------------------------------------------------------------------------------------------------------------------------+
| ``DATA_FORMAT``         | Data format: ``"JSON"`` or ``"ASCII"``                                                                                  |
+-------------------------+-------------------------------------------------------------------------------------------------------------------------+
| ``DATA``                | Input data.                                                                                                             |
|                         | Format will depend on the ``DATA_FORMAT``                                                                               |
|                         | See details below.                                                                                                      |
+-------------------------+-------------------------------------------------------------------------------------------------------------------------+

**ADDITIONAL KEYS**

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


The JSON file supports C/C++ syntax for comments: single-line comment (``//``) or comment blocks (``/*`` and ``*/``).

The symbol ``(i#)`` refers to a integer number sequence.. The symbol ``(s#)`` refers to a string input. The symbol ``<f#>`` refers to a float input value.


Example:


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
            "Data_Format": "ASCII",
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
            "Data_Format": "JSON",
            "Data": {
                "Filepath": "SS_speciesA_ScalarAquifer_test.csv",
                "Delimiter": ",",
                "Number_of_header_rows": 3,
                "Header_key_row": 3
            }
        }
    }