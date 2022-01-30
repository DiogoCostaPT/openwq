Theoretical Foundation
==================================


\subsection{OpenWQ: Conservation Equations}

\subsubsection{Conservative Solute Transport: Advection-Dispersion}

There are two options available for computing the conservative transport of solutes (dissolved in water) or fully suspended sediments. The first option accounts for both advection and dispersion and solves the hyperbolic-parabolic advection-dispersion PDE that is extended up to 3D depending on the host model (Equation \ref{ADE_option1}). The second option, only accounts for advection (Equation \ref{AE_option2}).

\begin{equation}
\begin{split}
\dfrac{\partial (h \cdot c_s)}{\partial t} + \nabla( h \cdot \vec{v} \cdot c_s) = \nabla (h \cdot D \cdot \nabla c_s) + S  \\
\text{extended up to 3D} \rightarrow \frac{\partial{(h \cdot c_s)}}{\partial t}+\frac{\partial{(h \cdot u_x \cdot c_s)}}{\partial x}+\frac{\partial{(h \cdot u_y \cdot c_s)}}{\partial y} + \frac{\partial{(h \cdot u_z \cdot c_s)}}{\partial z} = \\ \frac{\partial}{\partial x}\Bigl( h \cdot E_x \frac{\partial c_s}{\partial x} \Bigr)+
\frac{\partial}{\partial y}\Bigl( h \cdot E_y \frac{\partial c_s}{\partial y} \Bigr) +
\frac{\partial}{\partial z}\Bigl( h \cdot E_z \frac{\partial c_s}{\partial z} \Bigr) + S
\end{split}
\label{ADE_option1}
\end{equation}

\begin{equation}
\begin{split}
\dfrac{\partial (h \cdot c_s)}{\partial t} + \nabla( h \cdot \vec{v} \cdot c_s) = S  \\
\text{extended up to 3D} \rightarrow \frac{\partial{(h \cdot c_s)}}{\partial t}+\frac{\partial{(h \cdot u_x \cdot c_s)}}{\partial x}+\frac{\partial{(h \cdot u_y \cdot c_s)}}{\partial y} + \\ \frac{\partial{(h \cdot u_z \cdot c_s)}}{\partial z} = S
\end{split}
\label{AE_option2}
\end{equation}

where $c_s$ is the concentration of a given dissolved substance [ML\textsuperscript{-3}]; $u$ is the velocity in the x, y and z direction [LT\textsuperscript{-1}] $E$ is the horizontal diffusivity [L\textsuperscript{2}T\textsuperscript{-1}]; and $S$ is a source term [ML\textsuperscript{-2}T\textsuperscript{-1}] that is linked to the chemistry modules. The $E$ term accounts for the combined effect of different mixing phenomena:
\begin{equation}
E=E^{turb}+E^{tsd}+E^{sgt}+E_d
\label{SW_eq5}
\end{equation}
\noindent where $E^{turb}$ is the turbulent or eddy diffusivity, which is a complex phenomena with multifractal behaviour dominated by friction forcess, $E^{tsd}$ is the so-called Taylor-shear dispersion that arises from the unresolved vertical variation of the horizontal flow, $E^{sgt}$ accounts for the sub-grid eddy viscosity and diffusivity arising from unresolved mixing occurring at sub-grid scale, and $E$ is the background molecular diffusion resulting from the probabilistic Brownian motion concept occurring at particle scales. $E$ is approximated to the dominant turbulent dispersion based on the eddy viscosity concept: $E^{turb} = \nu_t / \sigma$, where $\sigma$ is the Prandtl-Schmidt number. Turbulent viscosity ($\nu_t$) depends on shear velocity ($u^*$) and on a turbulent length scale ($l_t=0.07h$). In this model, this relationship is approximated by an algebraic expression: $\nu_t \approx k u^* l_t$, where $k$ is a user-defined scaling factor to account for sub-grid scale eddies \citep{Costa2016c}.\par

\subsubsection{Biogeochemistry: Reaction networks}

These expressions are user defined, but they usually take the form of 1$^{st}$ (Eq. \ref{1storder_eq}), 2$^{nd}$ (Eq. \ref{2ndorder_eq}) or 3$^{rd}$-multispecies (Eq. \ref{3rdorder_eq}) reaction kinetics.

\begin{equation}
\frac{\partial{(c_a)}}{\partial t} = - k \cdot B \cdot c_a
\label{1storder_eq}
\end{equation}

\begin{equation}
\frac{\partial{(c_a)}}{\partial t} = - k \cdot B \cdot c_a^2
\label{2ndorder_eq}
\end{equation}

\begin{equation}
\frac{\partial{(c_a)}}{\partial t} = - k \cdot B \cdot c_a^2 * c_b
\label{3rdorder_eq}
\end{equation}

\noindent where $c_a$ and $c_b$ [ML\textsuperscript{-3}] are the concentrations of chemical species $a$ and $b$, parameter/variable $B$ represents weather/hydrological dependencies (such as soil moisture and temperature) and $k$ is the reaction rate [ML\textsuperscript{-3}T\textsuperscript{-1}]. The reaction rate $k$ can be provided as the reaction rate (usually using standard maximum at a reference temperature, often 20$^o$C) or using expressions that can include relationships with the hydrological/weather dependency variables/parameters.
