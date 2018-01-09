### FAnToM Session
### API Version: 20140515
### Used core version:    2c6e4dc72b1c3dceb3b624eacc44d50301928abf (praktikum/visPraktikum2015)
### Used toolbox version: 72513120e56f15bb7fc83f9b6ed8f494a779bc98 (stable)

################################################################
###                  Reset GUI                               ###
################################################################
fantom.ui.setCamera( 0, fantom.ui.Camera( fantom.math.Vector3(4.86254, 0.0639636, 16.3943), fantom.math.Vector3(4.86569, 0.0650036, 15.3943), fantom.math.Vector3(0.00623076, 0.99998, 0.00105961), 1, -3.47652e-310 ) )
fantom.ui.setCamera( 1, fantom.ui.Camera( fantom.math.Vector3(0, 3.8637, 0), fantom.math.Vector3(0, 2.8637, 0), fantom.math.Vector3(0, 0, 1), 0, 1 ) )
fantom.ui.setCamera( 2, fantom.ui.Camera( fantom.math.Vector3(0, 0, 3.8637), fantom.math.Vector3(0, 0, 2.8637), fantom.math.Vector3(0, 1, 0), 0, 1 ) )
fantom.ui.setCamera( 3, fantom.ui.Camera( fantom.math.Vector3(3.8637, -0.2, 0), fantom.math.Vector3(2.8637, -0.2, 0), fantom.math.Vector3(0, 0, 1), 0, 1 ) )

fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 0, fantom.math.Matrix4( (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 1, fantom.math.Matrix4( (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 2, fantom.math.Matrix4( (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 3, fantom.math.Matrix4( (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 4, fantom.math.Matrix4( (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 5, fantom.math.Matrix4( (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) ), False ) )

fantom.ui.setBackgroundColor( fantom.math.Color(0, 0, 0, 1) )

fantom.ui.setRotationCenter( fantom.ui.RotationCenter( fantom.math.Vector3(0, 0, 0), True, True, True ) )


################################################################
###                  Create algorithms                       ###
################################################################
Load_VTK = fantom.makeAlgorithm("Load/VTK")
Load_VTK.setName( "Load/VTK" )
Load_VTK.setOption("Input File", "/home/visprak11/fantom/TestData/JetVelocity.vtk")
Load_VTK.setOption("Big Endian", True)
Load_VTK.setOption("Dimension", "2D if third component is zero")
Load_VTK.setOption("Time List", "")
fantom.ui.setAlgorithmPosition(Load_VTK, fantom.math.Vector2(0, 35))

Grid_ShowGrid = fantom.makeAlgorithm("Grid/Show Grid")
Grid_ShowGrid.setName( "Grid/Show Grid" )
Grid_ShowGrid.setOption("Line color", fantom.math.Color(0, 0, 1, 0.2941))
Grid_ShowGrid.setOption("Random jittering of color", True)
Grid_ShowGrid.setOption("Random seed", 0)
Grid_ShowGrid.setOption("Line width", 1.5)
fantom.ui.setAlgorithmPosition(Grid_ShowGrid, fantom.math.Vector2(266, 17))
Grid_ShowGrid.setVisualOutputVisible('grid', True)

Hauptaufgabe_StreamLineDrawer = fantom.makeAlgorithm("Hauptaufgabe/StreamLineDrawer")
Hauptaufgabe_StreamLineDrawer.setName( "Hauptaufgabe/StreamLineDrawer" )
Hauptaufgabe_StreamLineDrawer.setOption("Method", "Euler")
Hauptaufgabe_StreamLineDrawer.setOption("Stepwidth", 0.05)
Hauptaufgabe_StreamLineDrawer.setOption("Adaptiver Grenzwert", 0.1)
Hauptaufgabe_StreamLineDrawer.setOption("Number of Steps", 100000)
Hauptaufgabe_StreamLineDrawer.setOption("Color", fantom.math.Color(0.75, 0.75, 0, 1))
Hauptaufgabe_StreamLineDrawer.setOption("Delta Streamline", 0.03)
fantom.ui.setAlgorithmPosition(Hauptaufgabe_StreamLineDrawer, fantom.math.Vector2(33, 337))
Hauptaufgabe_StreamLineDrawer.setVisualOutputVisible('Streamlines', True)

Hauptaufgabe_CriticalPoints = fantom.makeAlgorithm("Hauptaufgabe/CriticalPoints")
Hauptaufgabe_CriticalPoints.setName( "Hauptaufgabe/CriticalPoints" )
fantom.ui.setAlgorithmPosition(Hauptaufgabe_CriticalPoints, fantom.math.Vector2(171, 232))

Hauptaufgabe_SeedpointTemplates = fantom.makeAlgorithm("Hauptaufgabe/SeedpointTemplates")
Hauptaufgabe_SeedpointTemplates.setName( "Hauptaufgabe/SeedpointTemplates" )
Hauptaufgabe_SeedpointTemplates.setOption("Delta Seed", 2.5)
fantom.ui.setAlgorithmPosition(Hauptaufgabe_SeedpointTemplates, fantom.math.Vector2(326, 305))

Hauptaufgabe_PoissonDiscDistribution = fantom.makeAlgorithm("Hauptaufgabe/PoissonDiscDistribution")
Hauptaufgabe_PoissonDiscDistribution.setName( "Hauptaufgabe/PoissonDiscDistribution" )
Hauptaufgabe_PoissonDiscDistribution.setOption("Delta Seed", 0.2)
Hauptaufgabe_PoissonDiscDistribution.setOption("Grow Rate", 30)
Hauptaufgabe_PoissonDiscDistribution.setOption("Number Start Points", 1)
fantom.ui.setAlgorithmPosition(Hauptaufgabe_PoissonDiscDistribution, fantom.math.Vector2(293.016, 143.52))



################################################################
###                     Make Connections                     ###
################################################################
Load_VTK.connect("Fields", Hauptaufgabe_SeedpointTemplates, "Field")
Hauptaufgabe_PoissonDiscDistribution.connect("Seedpoints", Hauptaufgabe_SeedpointTemplates, "Random Points")
Hauptaufgabe_CriticalPoints.connect("Critical Points", Hauptaufgabe_SeedpointTemplates, "Critical Points")
Load_VTK.connect("Fields", Hauptaufgabe_StreamLineDrawer, "Field")
Load_VTK.connect("Fields", Hauptaufgabe_CriticalPoints, "Field")
Load_VTK.connect("Grid", Grid_ShowGrid, "Grid")
Hauptaufgabe_SeedpointTemplates.connect("Seedpoints", Hauptaufgabe_StreamLineDrawer, "Seedpoints")
Load_VTK.connect("Grid", Hauptaufgabe_PoissonDiscDistribution, "Grid")


################################################################
###                      Run algorithms                      ###
################################################################
fantom.scheduleAllNecessary()