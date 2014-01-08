This project need:
-libQGLViewer (2.4.0)
-OpenMesh (2.4)
-Qt 5.1.1

The ManMadeObjectEditor.pro must be updated accordingly.

How to use:
-Move the vertices using the right-click of the mouse.
-Add new vertices on the edges using right-click + Ctrl (Windows) or cmd (MacOS).
-Delete vertices using right-click + shift.
-Move the scene using the left-click.

-When simplififying either the floor plan or the profile (assuming the simplification window is open), use right-click on a vertex
to define the begining and again right click on an other vertex to define the end of the
part that will be simplified. Do not try to play with the floor plan or the profile when
the simplification window is open.

-When merging profiles (assuming the merging window is open), select two different profiles using right-click on the edges of the floor plan
and then select the vertices on both profiles that must be kept. A new profile will be generated using the provided vertices.
Do not try to play with the floor plan or the profile when the merge window is open.

-To create a new profile for an edge, first select the edge with right-click, then use Edit -> New Profile.