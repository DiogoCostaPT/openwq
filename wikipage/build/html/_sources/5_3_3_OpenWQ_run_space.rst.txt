API 3 ``RunSpaceStep``
============================================

API 3 ``RunSpaceStep`` is inside Class ``OpenWQ_couplercalls``.

.. code-block:: guess

    void OpenWQ_couplercalls::RunSpaceStep(
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
        time_t simtime,                            // simulation time in seconds since seconds since 00:00 hours, Jan 1, 1970 UTC
        const int source,
        const int ix_s,
        const int iy_s,
        const int iz_s,
        const int recipient,
        const int ix_r,
        const int iy_r,
        const int iz_r,
        const double wflux_s2r,
        const double wmass_source)