# 3DEngine

**Q: What do you get when you cross C++, Linear Algebra, and tears?**  
A: A slightly awful graphics engine!

![Rotating Block](/src/Assets/Images/rotating.gif)

If you've ever wanted to see 3d objects in 10 frames per second, you've come to the right place! Now you may wonder, why is it so slow? The answer is simple: It's because it's running on the CPU instead of the GPU like a real engine should. This begs the question: *Why?* Because past me thought it would be a good idea to write everything from scratch. On the bright side, it was a cool learning experience as my first program in C++ and I'm pretty proud of the result.

## Getting Started

To run this scuffed 3D Engine, simply clone the repository and run the `make run` command. You will need to have SDL2 installed for this to work though. If you're using Debian/Ubuntu, you can install it with `sudo apt install libsdl2-dev`. If not, I trust you know what you're doing.

If you don't want to touch any code, you can also just download the engine.exe file and run it. **Warning**: This will most likely not work so use at your own risk.

## Features

- Camera movement 
    - WASD + mouse (hold left mouse button)
- Lighting 
    - Directional light
    - More soon?
- OBJ parser
- Models   
    - Grass Block
    - Utah Teapot
- Rendering
    - Wireframe
    - Textures

## Showcase

### Minecraft Grass Block with Lighting 
![Grass Block](/src/Assets/Images/grass.png)
### Utah Teapot with Lighting
![Utah Teapot](/src/Assets/Images/teapot.png)
### Utah Teapot with Direction Shader
![Funky Teapot](/src/Assets/Images/funky_teapot.png)

## Memorable Fails

![Fail1](/src/Assets/Images/Fails/Fail1.png)
![Fail2](/src/Assets/Images/Fails/Fail2.png)
![Fail3](/src/Assets/Images/Fails/Fail3.png)
![Fail4](/src/Assets/Images/Fails/Fail4.png)
![Fail5](/src/Assets/Images/Fails/Fail5.png)
![Fail6](/src/Assets/Images/Fails/Fail6.png)