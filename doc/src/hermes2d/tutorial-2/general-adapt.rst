Adaptivity for General 2nd-Order Linear Equation (12)
-----------------------------------------------------

**Git reference:** Tutorial example `12-general-adapt <http://git.hpfem.org/hermes.git/tree/HEAD:/hermes2d/tutorial/12-general-adapt>`_. 

Model problem
~~~~~~~~~~~~~

This example does not bring anything substantially new and its purpose is solely to 
save you work adding adaptivity to the tutorial example 
`07-general <http://git.hpfem.org/hermes.git/tree/HEAD:/hermes2d/tutorial/07-general>`_. 
Feel free to adjust this example for your own applications.

Sample results
~~~~~~~~~~~~~~

Solution:

.. image:: 12/12-solution.png
   :align: center
   :width: 465
   :height: 400
   :alt: Solution to the general 2nd-order linear equation example.

Final hp-mesh:

.. image:: 12/12-mesh.png
   :align: center
   :width: 450
   :height: 400
   :alt: Final finite element mesh for the general 2nd-order linear equation example.

Convergence graphs of adaptive h-FEM with linear elements, h-FEM with quadratic elements
and hp-FEM.

.. image:: 12/conv_dof.png
   :align: center
   :width: 600
   :height: 400
   :alt: DOF convergence graph for tutorial example 12-general-adapt.

Convergence comparison in terms of CPU time. 

.. image:: 12/conv_cpu.png
   :align: center
   :width: 600
   :height: 400
   :alt: CPU convergence graph for tutorial example 12-general-adapt.
