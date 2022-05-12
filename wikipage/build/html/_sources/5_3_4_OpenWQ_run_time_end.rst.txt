API 4 ``RunTimeLoopEnd``
==============================================

API 4 ``RunTimeLoopEnd`` is inside Class ``OpenWQ_couplercalls``.

.. code-block:: guess

    void OpenWQ_couplercalls::RunTimeLoopEnd(
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_json& OpenWQ_json,                    // create OpenWQ_json object
        OpenWQ_wqconfig& OpenWQ_wqconfig,            // create OpenWQ_wqconfig object
        OpenWQ_units& OpenWQ_units,                  // functions for unit conversion
        OpenWQ_readjson& OpenWQ_readjson,            // read json files
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_initiate& OpenWQ_initiate,            // initiate modules
        OpenWQ_watertransp& OpenWQ_watertransp,      // transport modules
        OpenWQ_chem& OpenWQ_chem,                   // biochemistry modules
        OpenWQ_sinksource& OpenWQ_sinksource,        // sink and source modules)
        OpenWQ_solver& OpenWQ_solver,
        OpenWQ_output& OpenWQ_output,
        time_t simtime)
