Get OpenWQ
=====================================

It is important that you follow the following steps in order to ``clone`` OpenWQ in your machine, but to also make sure that you can easily ``pull`` and integrate updates in an efficient and hassle-free manner.

STEP 1: Identify download folder
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

OpenWQ's source code should be cloned inside your hydro-model source code folder.


STEP 2: Clone OpenWQ
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To clone OpenWQ in your machine you should first install `Git <https://git-scm.com/>`_.
Once Git is installed, open a Terminal and go to the hydro-model source-code folder identified in STEP 1.

In the terminal, run the following command:

``git clone https://github.com/DiogoCostaPT/openwq.git``

This will create a new folder named ``openwq`` where the source code is located.


STEP 3: Keep OpenWQ updated
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We are continuously working on improvements to OpenWQ, so it is important that you regularly ``pull`` changes from OpenWQ's remote repository in GitHub.
Doing so, should not break OpenWQ's API call inside your ``OpenWQ_hydrolink.cpp`` file.

To update OpenWQ, regularly:

    * Open a Terminal,
    * Go inside the ``openwq`` folder created in the source-code folder identified in STEP 1
    * Run the following command: ``git pull``