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

Open a Terminal and go to that folder:

``cd <hydro model code directory>``

Then, type the command below to create a new folder ``OpenWQ_code``.

``mkdir OpenWQ_code``

Then go inside the new folder

``cd OpenWQ_code``

Finally, run the following command to clone ``openwq`` source code:

``git clone https://github.com/DiogoCostaPT/openwq.git``

This will create a new folder named ``openwq`` inside ``OpenWQ_code` where the source code will be copied.

ATENTION: never change the code inside the ``OpenWQ_code``, except if for updating (see STEP 3).

Once the clonning has been completed, you should see the following directory tree:

.. image:: source_code_folder.png
    :width: 500
    :alt: Folder tree


STEP 3: Keep OpenWQ updated
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We are continuously working on improvements to OpenWQ. Thus, it is important that you regularly ``pull`` changes from OpenWQ's remote repository in GitHub.
If you follow the instruction prescribed in STEP 2, updating ``openwq`` will not break OpenWQ's API calls.

To update OpenWQ, regularly:

    * Open a Terminal,
    * Go inside the ``OpenWQ_code`` folder, and
    * Run the following command: ``git pull``