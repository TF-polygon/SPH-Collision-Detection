# SPH-Collision-Detection

This system includes Smoothed Particles Hydrodynamics (SPH) with a collision detection with specific collision object. Also, there is a signal which is able to provide a feedback for the interaction among users in real-time. Total system configuration is included a computer which is able to implement SPH model and a single-board computer called raspberry pi 4 using TCP/IP protocol socket communication. I'm looking forward to use this system as basic of interaction system of virtual reality or metaverse environment.

<br>

## Collision Detection

First of all, there's a class to detect any collision named 'Controller'. Most functions of the class are meant to manage the value of the collider and create a collider. Creating a collider is similar to creating a fluid particle, but I eventually found it unnecessary. (So this class contin some not valuable variables or functions). Core functions in this project are in `System.cpp`. In this file, the function `parallelForce()` is crucial for collision detection. The code that detects collisions with the collider and fluid particles is implemented in the function and processed in parallel using the thread() function. Collision Detection status is stored specific variable according to boolean in `Particle` class. 
Currently, I completed to develope the system that is able to send datas storing status of collision detection for a interaction in real-time when GLUT functions are looped in main function.<br><br>

## C++ class
Other class in addition to these are not necessary to experiment only simulation.
|Class|Function|Purpose|
|------|---|---|
|Camera|Camera|Responsible for vision of a user within the simulation|
|Geometry|Mesh & Shader|Building a objects and surface of the objects in the simulation|
|Particle|Particle|Initialization of 3-Dimensional particles sequence|
|System(solver)|Building Tables & Update|Building the hash table for computation among particles and Drawing particles and parallel processing data|
|Engine|Main|The main function of the simulation system|

<br>

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
If you input this command in the Raspberry Pi terminal, you could get a IPv4 IP address of your raspberry pi. Among the files which are included this repository, `Server.cpp` is a TCP/IP socket file for server in raspberry Pi. So you don't have to build it in C++ simulator but in raspberry Pi using terminal. Please be sure to keep this in mind. <br>
## Demo Video
This section will show you video how simulator is operated in window. The experimental environment is not high performance. The CPU used in the experiment is Intel(R) Core(TM) i3-7100U and GPU is Intel(R) HD Graphics 620.
### SPH Simulation and Collision Detection
This simulator was using 1.7K particles and experimental environments are Intel i3-7100U CPU and Intel(R) HD Graphics 620. Because it was not a very good experimental environment, it was difficult to expect high-performance simulation. If you run the simulation in a better experimental environment, you can expect better results. <br><br>
<img src="https://github.com/TF-polygon/TF-polygon/assets/111733156/b0adfc18-f0e2-4592-9a0f-51f1fec02646" width="500" height="350">

### Raspberry Pi Terminal Status with Collision Detection and TCP/IP Socket Communication
If the simulator detects collisions among fluid particles and collider, it sends the string data `Collision Detection`. In this demo video, the Raspberry Pi terminal is displayed in the background and output data that is printed in the terminal is the string data `Collision Detection` from simulator. Perhaps you can check that the output data is continuously printed on the terminal even thought demo video is blurry a lot. This is because data is processed in parallel in the simulation, so the form in which the data is transmitted will also be processed in parallel. So, you can see that 'Collision Detection' is output continuously rather than sequentially from the terminal.
<br><br>
<img src="https://github.com/TF-polygon/SPH-Collision-Detection/assets/111733156/013f29e0-af8e-4fab-9a78-362f24c5a868" width="400" height="400">
<br>
Nevertheless, the speed of simulation is not delayed.
<br>

## Optimization
C++ simulator sends the collision state data of each particle one by one at every moment. Therefore, communication among simulator and raspberry pi is not smooth. Also, Simulator may sends the NOT collision state even though the collider and the particle collided because simulator check collision state to all particles of fluid. Thus, I used a method to packetizing data, and in raspberryPi, a method of grouping and processing data. First, packet unit is 4 and if even 1 out of 4 collisions are detected, simulator sends the data that collision is occured. In raspberryPi that is received the datas, distinguish two values for collision detection. Collision Detection : `1`, NOT : `0`. And then, Divide the received data into 100 picese and sum all. If sum of 100 datas value is over 1, it is considered a collision. Conversely, if it is 0, it is considered that there is no collision. In the end, we could improve pace of simulation by TCP/IP socket communication and pace of data communication about raspberryPi LED processing.
<br><br>
<img src="https://github.com/TF-polygon/SPH-Collision-Detection/assets/111733156/704041f4-eb2b-4596-9aeb-92dd6411131e" width="500" height="350">
<br>
## Control
You can control this simulation using `S`, `R`, `M` keys as follow:
- `S` : Begin simulation
- `R` : Fluid particles reset
- `M` : Create a collider<br>

You can move camera by mouse moving using `LB`, `RB` keys as follow:
- Mouse `LB` : Change angle to camera
- Mouse `RB` : Zoom In and Out 
<br>

If you create a collider, you can move the collider even if you don't click on mouse.



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
  
