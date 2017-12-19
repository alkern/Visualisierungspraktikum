### FAnToM Session
### API Version: 20140515
### Used core version:    2c6e4dc72b1c3dceb3b624eacc44d50301928abf (praktikum/visPraktikum2015)
### Used toolbox version: 72513120e56f15bb7fc83f9b6ed8f494a779bc98 (stable)

################################################################
###                  Reset GUI                               ###
################################################################
fantom.ui.setCamera( 0, fantom.ui.Camera( fantom.math.Vector3(4.62686, 0.332386, 18.2), fantom.math.Vector3(4.62686, 0.332386, 17.2), fantom.math.Vector3(0, 1, 0), 1, 3.45685e-310 ) )
fantom.ui.setCamera( 1, fantom.ui.Camera( fantom.math.Vector3(0, 3.8637, 0), fantom.math.Vector3(0, 2.8637, 0), fantom.math.Vector3(0, 0, 1), 0, 1 ) )
fantom.ui.setCamera( 2, fantom.ui.Camera( fantom.math.Vector3(0, 0, 3.8637), fantom.math.Vector3(0, 0, 2.8637), fantom.math.Vector3(0, 1, 0), 0, 1 ) )
fantom.ui.setCamera( 3, fantom.ui.Camera( fantom.math.Vector3(3.8637, 0, 0), fantom.math.Vector3(2.8637, 0, 0), fantom.math.Vector3(0, 0, 1), 0, 1 ) )

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
Grid_ShowGrid.setOption("Line color", fantom.math.Color(0, 0, 1, 1))
Grid_ShowGrid.setOption("Random jittering of color", True)
Grid_ShowGrid.setOption("Random seed", 0)
Grid_ShowGrid.setOption("Line width", 1.5)
fantom.ui.setAlgorithmPosition(Grid_ShowGrid, fantom.math.Vector2(0, 163))
Grid_ShowGrid.setVisualOutputVisible('grid', True)

Hauptaufgabe_SeedPointPicker = fantom.makeAlgorithm("Hauptaufgabe/SeedPointPicker")
Hauptaufgabe_SeedPointPicker.setName( "Hauptaufgabe/SeedPointPicker" )
Hauptaufgabe_SeedPointPicker.setOption("Startline_start_x", 0)
Hauptaufgabe_SeedPointPicker.setOption("Startline_start_y", -3)
Hauptaufgabe_SeedPointPicker.setOption("Startline_start_z", 0)
Hauptaufgabe_SeedPointPicker.setOption("Startline_end_x", 0)
Hauptaufgabe_SeedPointPicker.setOption("Startline_end_y", 3)
Hauptaufgabe_SeedPointPicker.setOption("Startline_end_z", 0)
Hauptaufgabe_SeedPointPicker.setOption("Startline_3D_x", 9)
Hauptaufgabe_SeedPointPicker.setOption("Startline_3D_y", -3)
Hauptaufgabe_SeedPointPicker.setOption("Startline_3D_z", 0)
Hauptaufgabe_SeedPointPicker.setOption("Number", 10)
fantom.ui.setAlgorithmPosition(Hauptaufgabe_SeedPointPicker, fantom.math.Vector2(0, 249))

Hauptaufgabe_StreamLineDrawer = fantom.makeAlgorithm("Hauptaufgabe/StreamLineDrawer")
Hauptaufgabe_StreamLineDrawer.setName( "Hauptaufgabe/StreamLineDrawer" )
Hauptaufgabe_StreamLineDrawer.setOption("Method", "Euler")
Hauptaufgabe_StreamLineDrawer.setOption("Stepwidth", 0.05)
Hauptaufgabe_StreamLineDrawer.setOption("Adaptiver Grenzwert", 0.1)
Hauptaufgabe_StreamLineDrawer.setOption("Number of Steps", 100000)
Hauptaufgabe_StreamLineDrawer.setOption("Color", fantom.math.Color(0.75, 0.75, 0, 1))
fantom.ui.setAlgorithmPosition(Hauptaufgabe_StreamLineDrawer, fantom.math.Vector2(0, 335))
Hauptaufgabe_StreamLineDrawer.setVisualOutputVisible('Streamlines', True)



################################################################
###                     Make Connections                     ###
################################################################
Hauptaufgabe_SeedPointPicker.connect("Seedpoints", Hauptaufgabe_StreamLineDrawer, "Seedpoints")
Load_VTK.connect("Fields", Hauptaufgabe_StreamLineDrawer, "Field")
Load_VTK.connect("Grid", Grid_ShowGrid, "Grid")


################################################################
###                      Run algorithms                      ###
################################################################
fantom.scheduleAllNecessary()