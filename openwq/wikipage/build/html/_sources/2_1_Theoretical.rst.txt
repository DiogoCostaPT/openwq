Theoretical Foundation
==================================


Conservation Equations
~~~~~~~~~~~~~~~~~~~~~~

There are two options available for computing the conservative transport of solutes (dissolved in water) or fully suspended sediments.
The first option accounts for transport through both advection and dispersion processes. It solves the hyperbolic-parabolic advection-dispersion PDE, which takes 1D-3D forms depending on the characteristics of the host hydro-model:

.. math::
    :nowrap:

    \begin{equation}
    \begin{split}
    \dfrac{\partial (h \cdot c_s)}{\partial t} + \nabla( h \cdot \vec{v} \cdot c_s) = \nabla (h \cdot D \cdot \nabla c_s) + S  \\
    \text{extended up to 3D} \rightarrow \frac{\partial{(h \cdot c_s)}}{\partial t}+\frac{\partial{(h \cdot u_x \cdot c_s)}}{\partial x}+\frac{\partial{(h \cdot u_y \cdot c_s)}}{\partial y} + \frac{\partial{(h \cdot u_z \cdot c_s)}}{\partial z} = \\ \frac{\partial}{\partial x}\Bigl( h \cdot E_x \frac{\partial c_s}{\partial x} \Bigr)+
    \frac{\partial}{\partial y}\Bigl( h \cdot E_y \frac{\partial c_s}{\partial y} \Bigr) +
    \frac{\partial}{\partial z}\Bigl( h \cdot E_z \frac{\partial c_s}{\partial z} \Bigr) + S
    \end{split}
    \label{ADE_option1}
    \end{equation}

The second option only accounts for transport through advection:

.. math::
    :nowrap:

    \begin{equation}
    \begin{split}
    \dfrac{\partial (h \cdot c_s)}{\partial t} + \nabla( h \cdot \vec{v} \cdot c_s) = S  \\
    \text{extended up to 3D} \rightarrow \frac{\partial{(h \cdot c_s)}}{\partial t}+\frac{\partial{(h \cdot u_x \cdot c_s)}}{\partial x}+\frac{\partial{(h \cdot u_y \cdot c_s)}}{\partial y} + \\ \frac{\partial{(h \cdot u_z \cdot c_s)}}{\partial z} = S
    \end{split}
    \label{AE_option2}
    \end{equation}


where :math:`c_s` is the concentration of a given dissolved substance :math:`[ML^{-3}]`; :math:`u` is the velocity in the x, y and z direction :math:`[LT^{-1}]` :math:`E` is the horizontal diffusivity :math:`[L^2T^{-1}]`; and :math:`S` is a source term :math:`[ML^{-2}T^{-1}]` that is linked to the chemistry modules. The :math:`E` term accounts for the combined effect of different mixing phenomena:

.. math::
    \begin{equation}
    E=E^{turb}+E^{tsd}+E^{sgt}+E_d
    \label{SW_eq5}
    \end{equation}

where :math:`E^{turb}` is the turbulent or eddy diffusivity, which is a complex phenomena with multifractal behaviour dominated by friction forcess, :math:`E^{tsd}` is the so-called Taylor-shear dispersion that arises from the unresolved vertical variation of the horizontal flow, :math:`E^{sgt}` accounts for the sub-grid eddy viscosity and diffusivity arising from unresolved mixing occurring at sub-grid scale, and :math:`E` is the background molecular diffusion resulting from the probabilistic Brownian motion concept occurring at particle scales.
:math:`E` is approximated to the dominant turbulent dispersion based on the eddy viscosity concept: :math:`E^{turb} = \nu_t / \sigma`, where :math:`\sigma` is the Prandtl-Schmidt number. Turbulent viscosity (:math:`\nu_t`) depends on shear velocity (:math:`u^*`) and on a turbulent length scale (:math:`l_t=0.07h`).
In this model, this relationship is approximated by an algebraic expression: :math:`\nu_t \approx k u^* l_t`, where :math:`k` is a user-defined scaling factor to account for sub-grid scale eddies.


Biogeochemistry: Reaction networks
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

These expressions are flexible and defined by the user.
They are powered by the comprehensive C++ Mathematical Expression Toolkit Library (``ExprTk``) developed by `Arash Partow (1999-2020) <http://www.partow.net/programming/exprtk/index.html>`_.
The implementation of ``ExprTk`` in OpenWQ is simple to use and provides an extremely efficient run-time mathematical expression parser and evaluation engine.

``ExprTk`` supports numerous forms of functional, logical and vector processing semantics and is very easily extendible. In computational biogeochemistry, reaction kinetics usually take the general form of :math:`1^{st}`, :math:`2^{nd}` or :math:`3^{rd}`-multispecies `reaction kinetics <https://en.wikipedia.org/wiki/Chemical_kinetics>`_.

.. math::
    :nowrap:

    \begin{equation}
    \frac{\partial{c_a}}{\partial t} = - k \cdot B \cdot c_a
    \label{1storder_eq}
    \end{equation}

.. math::
    :nowrap:

    \begin{equation}
    \frac{\partial{c_a}}{\partial t} = - k \cdot B \cdot c_a^2
    \label{2ndorder_eq}
    \end{equation}

.. math::
    :nowrap:

    \begin{equation}
    \frac{\partial{c_a}}{\partial t} = - k \cdot B \cdot c_a^2 * c_b
    \label{3rdorder_eq}
    \end{equation}

where :math:`c_a` and :math:`c_b` :math:`[ML^{-3}]` are the concentrations of chemical species :math:`a` and :math:`b`, parameter/variable :math:`B` represents weather/hydrological dependencies (such as soil moisture and temperature) and :math:`k` is the reaction rate :math:`[ML^{-3}T^{-1}]`. The reaction rate :math:`k` can be provided as the reaction rate (usually using standard maximum at a reference temperature, often :math:`20^oC`) or using expressions that can include relationships with the hydrological/weather dependency variables/parameters.
