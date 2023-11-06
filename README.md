# SPH-Collision-Detection

This simulation is Smoothed Particles Hydrodynamics(SPH) included collision detection function with specific sphere shaped object. Indeed, we're developing and researching about haptic feedback for realistic contents using direct created haptic device applied Arduino kits or RaspberryPi GPIO etc. So it may contain some class for network. 

<br>

## Collision Detection

First of all, it needs an object to detect any collision and create a new class named `Controller`. Most functions of the class are meant to manage the value of the collider and create a collider. The way to create a collider is the same as how to create a one of fluid's particle, but I realized there was no need for that. (So this class contain some not valuable variables or functions). Core functions in this project are in `System.cpp`. In this file, the function `parallelForce()` is crucial for collision detection. The code that detects collisions with the collider and fluid particles is implemented in the function and processed in parallel using the thread() function. Collision Detection status is stored specific variable according to boolean in `Particle` class.  <br>
Currently, We're developing to send datas which is stored status of collision detection for haptic feedback by haptic device when GLUT functions are looped in main function. Using either raspberryPi or Arduino.

If you who were developed at past such as this project, I hope there is a helpful issues to us.
<br><br><br>
## C++ class
Other class in addition to these are not necessary to experiment only simulation.
- Camera class
- Mesh class
- Shader class
- Particle class
- Controller class
- System class
- Engine class
<br><br>
## Demo gif

<img src="https://github.com/TF-polygon/TF-polygon/assets/111733156/b0adfc18-f0e2-4592-9a0f-51f1fec02646">


<br>

## Dependencies

- Windows Visual Studio 2019 / Release x64
- GLUT / freeglut
- GLEW
- GLM
- OpenGL
