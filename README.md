# SPH-Collision-Detection

This simulation includes Smoothed Particles Hydrodynamics (SPH) with a collision detection function with specific sphere shaped object. Indeed, we are developing and researching haptic feedback for realistic content using directly created haptic devices with Arduino kits or Raspberry Pi GPIO, among others. So it may contain some class for network. 

<br>

## Collision Detection

First of all, it needs an object to detect any collision and create a new class named `Controller`. Most functions of the class are meant to manage the value of the collider and create a collider. Creating a collider is similar to creating a fluid particle, but I eventually found it unnecessary. (So this class contain some not valuable variables or functions). Core functions in this project are in `System.cpp`. In this file, the function `parallelForce()` is crucial for collision detection. The code that detects collisions with the collider and fluid particles is implemented in the function and processed in parallel using the thread() function. Collision Detection status is stored specific variable according to boolean in `Particle` class.  <br>
Currently, We're developing to send datas which is stored status of collision detection for haptic feedback by haptic device when GLUT functions are looped in main function. Using either raspberryPi or Arduino.

If you who were developed at past such as this project, I hope there is a helpful issues to us.
<br><br>

## C++ class
Other class in addition to these are not necessary to experiment only simulation.
- Camera class
- Mesh class
- Shader class
- Particle class
- RaspberryPi_NetClient class
- Controller class
- System class
- Engine class
<br><br>

## Socket Communication TCP/IP
I tried to use Arduino kit for haptic feedback by external device at first. But I changed from Arduino to Raspberry Pi because Arduino could not receive datas sending from Simulator for real-time by faster pace of C++ loop. So I succeeded to connect between Simulator in Windows Visual C++ and Raspberry Pi's terminal using `TCP/IP` socket communication. <br>
At result, when simulator detect a collision between one of particles of fluid and specific collider, socket communication is able to send to Raspberry Pi the data which is capsulized to collision detection. You can check up the connection status under the gif file. Finally, we can do a lot of attempt by this connection. I hope I often update this repository.
### PORT number
It's important to select PORT number that raspberry pi is not using for any services in the OS. In my case, I found empty PORT numbers in terminal of raspberry Pi. Terminal Input:
```
cat /etc/services
```
If you input this command in the Raspberry Pi terminal, you could see a lot of PORT numbers. The PORT numbers in the list are not usable numbers for socket communication, so select a number that is not there. And you have to share the same PORT number to Simulator. In my case, I selected a PORT number either '60100' or '60010'.
### Raspberry Pi's IP address
Information that simulator should know in order to connect to raspberry pi for socket communication is not only PORT number but also raspberry pi's IP address. Terminal Input:
```
hostname -I
```
If you input this command in the Raspberry Pi terminal, you could get a IPv4 IP address of your raspberry pi.


## Demo Video
Thie section will show you video how simulator is operated in window.
### SPH Simulation and Collision Detection
This simulator was using 1.7K particles and experimental environments are Intel i3-7100U CPU and Intel(R) HD Graphics 620. Because it was not a very good experimental environment, it was difficult to expect high-performance simulation. If you run the simulation in a better experimental environment, you can expect better results. <br>
<img src="https://github.com/TF-polygon/TF-polygon/assets/111733156/b0adfc18-f0e2-4592-9a0f-51f1fec02646" width="500" height="350">

### Raspberry Pi Terminal Status with Collision Detection and TCP/IP Socket Communication
If the simulator detects collisions among fluid particles and collider, it sends the string data `Collision Detection`. In this demo video, the Raspberry Pi terminal is displayed in the background and output data that is printed in the terminal is the string data `Collision Detection` from simulator. Perhaps you can check that the output data is continuously printed on the terminal even thought demo video is blurry a lot. This is because data is processed in parallel in the simulation, so the form in which the data is transmitted will also be processed in parallel. So, you can see that 'Collision Detection' is output continuously rather than sequentially from the terminal.
<br>
<img src="https://github.com/TF-polygon/SPH-Collision-Detection/assets/111733156/013f29e0-af8e-4fab-9a78-362f24c5a868" width="400" height="400">
<br>
Nevertheless, the speed of simulation is not delayed.
<br>

## Keyboard Control


## Dependencies
**Windows OS**
- Visual Studio 2019 / Release x64
- GLUT / freeglut
- GLEW
- GLM
- OpenGL

**Raspberry Pi**
- Raspberry Pi 4
## Reference
- **SPH-Fluid-Simulator**,  *lijenicol* [https://github.com/lijenicol/SPH-Fluid-Simulator]<br>
#### Papers
- **SPH Fluids in Computer Graphics**, *State of The Art Report* <br>[https://cg.informatik.uni-freiburg.de/publications/2014_EG_SPH_STAR.pdf]
- **FlowHaptics: Mid-Air Haptic Representation of Liquid Flow**, *MDPI applied science* [https://www.mdpi.com/2076-3417/11/18/8447]
- **Physics-based Mesh Deformation with Haptic Feedback and Material Anisotropy**, *Avirup Mandal* et al.(2021) [https://arxiv.org/pdf/2112.04362.pdf]
  
