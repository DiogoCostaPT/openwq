Overview
==========================================

* OpenWQ can be coupled to an existing model through the following API calls.

    * ``OpenWQ::decl`` (`see API <file:///C:/Users/bewise/OpenWQ/openwq/wikipage/build/html/5_3_1_OpenWQ_decl.html>`_)
    * ``OpenWQ::run_time_start`` (`see API. <file:///C:/Users/bewise/OpenWQ/openwq/wikipage/build/html/5_3_2_OpenWQ_run_time_star.html>`_)
    * ``OpenWQ::run_space`` (`see API <file:///C:/Users/bewise/OpenWQ/openwq/wikipage/build/html/5_3_3_OpenWQ_run_space.html>`_)
    * ``OpenWQ::run_time_end`` (`see API. <file:///C:/Users/bewise/OpenWQ/openwq/wikipage/build/html/5_3_4_OpenWQ_run_time_end.html>`_)

* How to perform the coupling?

Identify the following components of your hydro-model:

.. image:: coupling_steps_1.jpg


OpenWQ APIs need to be called from the following hydro-model components/functions (refer to image above):

* API 1 (``OpenWQ::decl``): should be called from the main host-model ``CONFIGURATION`` function/block, before the time and space loops are initiated
* API 2 (``OpenWQ::run_time_start``): should be called from the host-model function where the runtime workflow is located at ``SIMULATION > TIME LOOP > Before space loop``
* API 3 (``OpenWQ::run_space``): should be called from main host-model function/block within the ``SPACE LOOP``
* API 4 (``OpenWQ::run_time_end``): should be called from the host-model function/bock where the runtime workflow is located at ``SIMULATION > TIME LOOP > After space loop``

