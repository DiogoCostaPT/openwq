Overview
==================================

Standard input file format
~~~~~~~~~~~~~~~~~~~~~~~~~~
The configuration and forcing of the model is provided via JSON `(JavaScript Object Notation) <https://en.wikipedia.org/wiki/JSON>`_ files.

JSON files in OpenWQ
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
JSON is an ideal format for larger data that have a hierarchical structured relationship.
The data are in ``key``/``value`` pairs, e.g. ``"key_name_1"``: ``58``. Data objects are separated by commas.
Unlike the native JSON files, the implementation is OpenWQ supports C/C++ syntax for comments: single-line comment (``//``) or comment blocks (``/*`` and ``*/``).

JSON's basic data types supported:

* ``Number``: a signed decimal number that may contain a fractional part and may use exponential E notation, but cannot include non-numbers such as ``NaN``. The format makes no distinction between integer and floating-point; it uses a double-precision floating-point format for all its numeric values.
* ``"String"``: a sequence of zero or more Unicode characters. Strings are delimited with double-quotation marks ```""`` and support a backslash ``\``` escaping syntax.
* ``Boolean``: either of the values ``true`` or ``false``
* ``Array``: an ordered list of zero or more elements, each of which may be of any type. Arrays use square bracket notation with comma-separated elements, e.g., ``[2, 4, 5]```.
* ``null``: an empty value, using the word ``null``

The entries (keys or values) are not case sensitive. You can use both capital or lower case characters, or a mix of the two, for both the keys and values.
For example, you can write the JSON key ``OPENWQ_INPUT`` like that or like that ``openwq_input`` or like that ``OpenWQ_Input`` or like that ``OpEnWq_inPUt``, or whatever your prefer. OpenWQ will recognize the commands.

Example:

.. code-block:: json

    {
        "Key_name_1": 456,
        "Key_name_2": "String_2",
        "Key_name_3": true,
        "Key_name_4": [45, 66],
        "Key_name_5": null,
        "Key_name_6": {
            "Sub_Key_name_1": 4820,
            "Sub_Key_name_2": [5,7],
            "Sub_Key_name_3": {
                "Sub_Sub_Key_name_1": false,
                "Sub_Sub_Key_name_2": [45, "String_3"],
            }
        }
    }

For the sake of clarity, we wrote all OpenWQ JSON ``keys`` in capital letters through this document.
