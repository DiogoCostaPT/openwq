Synthetic tests
==============================

A total of 10 synthetic tests have been prepared to verify OpenWQ's outputs against analytical solutions. Such tests should be performed regularly during development to ensure that OpenWQ is still performing accurately as expected.


The json-input files for each of these synthetic tests can be obtained from the folowing GitHub repository: https://github.com/ue-hydro/synthetic_tests

The table below summarizes the different tests:

+-----------------------+-----------------------------------+---------------------------------------------------------------------------------------------------------------------------+
| Transport Scheme      | Designation                       | Description                                                                                                               |
+-----------------------+-----------------------------------+---------------------------------------------------------------------------------------------------------------------------+
| No transport          | ``1_noTrans_1species_1storder``   | Batch reactor involving 1 chemical species subject to first-order decay                                                   |
+                       +-----------------------------------+---------------------------------------------------------------------------------------------------------------------------+
|                       | ``2_noTrans_1species 2ndorder``   | Batch reactor involving 1 chemical species subject to second-order decay                                                  |
+                       +-----------------------------------+---------------------------------------------------------------------------------------------------------------------------+
|                       | ``3_noTrans_2species``            | Batch reactor involving 2 chemical species subject to first-order decay                                                   |
+                       +-----------------------------------+---------------------------------------------------------------------------------------------------------------------------+
|                       | ``4_noTrans_3species``            | Batch reactor involving 3 chemical species subject to first-order decay                                                   |
+                       +-----------------------------------+---------------------------------------------------------------------------------------------------------------------------+
|                       | ``5_noTrans_nitrogencycle``       | Batch reactor representing the nitrogen cycle                                                                             |
+                       +-----------------------------------+---------------------------------------------------------------------------------------------------------------------------+
|                       | ``6_noTrans_oxygenBODcycle``      | Batch reactor representing the dissolved oxygen cycle                                                                     |
+-----------------------+-----------------------------------+---------------------------------------------------------------------------------------------------------------------------+
| Advection-Dispersion  | ``7_trans_contS``                 | PorMedia Non-reactive transport of 1 chemical species in porous media subject to a continuous load                        |
+                       +-----------------------------------+---------------------------------------------------------------------------------------------------------------------------+
|                       | ``8_trans_contS``                 | PorMedia 1storder Reactive (linear decay) transport of 1 chemical species in porous media subject to a continuous load    |
+                       +-----------------------------------+---------------------------------------------------------------------------------------------------------------------------+
|                       | ``9_trans_instS``                 | PorMedia Non-reactive transport of 1 chemical species in porous media subject to an instantaneous load                    |
+                       +-----------------------------------+---------------------------------------------------------------------------------------------------------------------------+
|                       | ``10_trans_instS``                | PorMedia 1storder Reactive (linear decay) transport of 1 chemical species in porous media subject to an instantaneous load|
+-----------------------+-----------------------------------+---------------------------------------------------------------------------------------------------------------------------+