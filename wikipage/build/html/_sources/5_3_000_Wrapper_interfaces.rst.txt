Language binding
================

If your hydro-model is written in a language other than C or C++, you'll need to use ``wrapper libraries``.

What is binding?
~~~~~~~~~~~~~~~~
Binding generally refers to a mapping of one thing to another.
In the context of software libraries, bindings are wrapper libraries that bridge two programming languages, so that a library written for one language can be used in another language.

Wrapper libraries for Fortran
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

OpenWQ has been coupled to two Fortran models, *SUMMA* and *MizuRoute*.
The wrapper functions were developed using ```iso_c_binding`` <https://gcc.gnu.org/onlinedocs/gfortran/ISO_005fC_005fBINDING.html>`_, which is a standard intrinsic module that defines named constants, types, and procedures for C interoperability.

In order to minimize the amount of code added to the host-models, the interface functions have been stored in a series of ``.f90``, ``*.cpp`` and ``*.h`` files that can be used as a starting point for the coupling of OpenWQ to new host-models based on Fortran.

These interface functions, which can be obtained from the respective GitHub repositories of the aforementioned coupled models (`link <https://openwq.readthedocs.io/en/latest/5_3_0_Hydro_coupled_models.html>`_), are listed below (ordered in terms of information flow):

* (Fortran) ``summa_openWQ.f90`` (SUMA coupling) or ``mizuroute_openwq.f90`` (MizuRoute coupling)
* (Fortran) ``openWQInterface.f90``
* (Fortran) ``openWQ.f90``
* (C++) ``OpenWQ_interface.cpp`` and ``OpenWQ_interface.h``
* (C++) ``OpenWQ_hydrolink.cpp`` and ``OpenWQ_hydrolink.h``


Wrapper libraries for other languages
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Coming soon!

