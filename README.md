# Position Based Dynamics

This program was written from scratch for sole learning purposes. 
I intended to implement most of the techniques from:
- [Meshless Deformations Based on Shape Matching (2005)](http://matthias-mueller-fischer.ch/publications/MeshlessDeformations_SIG05.pdf)
-  [Position Based Dynamics (2007)](http://matthias-mueller-fischer.ch/publications/posBasedDyn.pdf) 
-  [Unified Particle Physics for Real-Time Applications (2014)](http://mmacklin.com/uppfrta_preprint.pdf)

PBD methods seem to become a standard for games and animation related simulation. By coding these things from the ground up, I was hoping to have a better understanding of typical parameters and how they relate to each other.  (e.G. mass, time step size and constraint iterations) 
<br>

[![IMAGE ALT TEXT](https://i.vimeocdn.com/video/797617207_640.jpg)](https://vimeo.com/347321461 "Position Based Dynamics C++ Qt App")
<br>
As in 'Unified Particle Physics' all physical motion is based on sphere primitives. A simple HashGrid is used for the Collision Broadphase. I managed to compile it with OpenMP  and run the program on multiple threads, but it is left single threaded for now and the code is far from ideal. It rather stands as a prototype / experiment. 

Built with cmake on OSX 10.13 including libraries :
-  Qt5.10
- OpenGL (3.3 core profile)
- Assimp
- Eigen3

<br>

**Links :**
-  [LearnOpenGL](https://learnopengl.com)  the 'everything-to-do-with starting OpenGL go-to'
-  [QtOpenGL blog TrentReed](https://www.trentreed.net/blog/qt5-opengl-part-0-creating-a-window/) a nice blog if you want to use Qt's latest OpenGL classes. I also used the inputManager class from Trent's blog
-  [Real-Time Collision Detection](http://www.r-5.org/files/books/computers/algo-list/realtime-3d/Christer_Ericson-Real-Time_Collision_Detection-EN.pdf) A solid reference and resource also with example-code
-  [GameProgrammingPatterns](https://gameprogrammingpatterns.com) Without much clue about 'game programming' as such, to me this book is a neat guide and advisor, presenting relevant design patterns.



