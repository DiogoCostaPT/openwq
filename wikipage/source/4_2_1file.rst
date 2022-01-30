Output formats
==================================

OpenWQ supports the following output data formats:

* `CSV <https://en.wikipedia.org/wiki/Comma-separated_values>`_ (json key/value: ``"FORMAT"``: ``"CSV"``)
* `HDF5 <https://en.wikipedia.org/wiki/Hierarchical_Data_Format>`_ (json key/value: ``"FORMAT"``: ``"HDF5"``)
* `VTU <https://people.math.sc.edu/Burkardt/data/vtu/vtu.html>`_ (json key/value: ``"FORMAT"``: ``"VTU"``)

Note that some data formats may not have been enabled in certain hydro-model couplings.
Please check with the respective coupling team.

The results will be printed in a folder with the name ``Output_OpenWQ``.

To select the data type go to the ``Master`` file and change the value of the sub-key ```"FORMAT"`` inside key ``"OPENWQ_OUTPUT"``

Example:

.. code-block:: json

    {
        "...": "...",
        "OPENWQ_OUTPUT": {
            "...": "...",
            "FORMAT": "HDF5",
            "...": "...",
            }
        "...": "...",
        }
    }




