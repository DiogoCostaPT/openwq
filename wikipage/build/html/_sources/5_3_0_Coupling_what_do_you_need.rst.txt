Overview
=================

OpenWQ can be coupled to existing hydro-models through the series of generic coupler functions.
These coupler functions have three main purposes:

* Convert datatypes between the hydro-model and OpenWQ
* Call OpenWQ's APIs

The main OpenWQ APIs are:

    * ``OpenWQ::decl`` (`see API reference <https://openwq.readthedocs.io/en/latest/5_3_1_OpenWQ_decl.html>`_)
    * ``OpenWQ::run_time_start`` (`see API reference <https://openwq.readthedocs.io/en/latest/5_3_2_OpenWQ_run_time_star.html>`_)
    * ``OpenWQ::run_space`` (`see API reference <https://openwq.readthedocs.io/en/latest/5_3_3_OpenWQ_run_space.html>`_)
    * ``OpenWQ::run_time_end`` (`see API reference <https://openwq.readthedocs.io/en/latest/5_3_4_OpenWQ_run_time_end.html>`_)
