Heat Equation
=============

.. math::

    \frac{\partial u}{\partial t} -\Delta u = f


Input file:
-----------

.. code-block:: yaml

   mesh:
     type: RectangleMesh
     nx: 2
     ny: 2

   problem:
     type: HeatEquationProblem
     start_time: 0
     end_time: 0.1
     dt: 0.001

   ics:
     all:
       type: ConstantInitialCondition
       value: [ 300 ]

   bcs:
     left:
       type: DirichletBC
       boundary: [ 'left' ]
       value: [ '310' ]
     right:
       type: DirichletBC
       boundary: [ 'right' ]
       value: [ '300' ]

   output:
     vtk:
       type: VTKOutput
       file: 'out'
