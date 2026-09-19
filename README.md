
# OpenGL Realistic Black Hole Simulation
### Ray traced light paths calculated using the geodesic equation from Einstein's General Relativity
Very much under early development and not complete.

<br>

### To compile and run the project, navigate to the BlackHoleSim directory and run the bash commands below:

#### Linux (GLFW pre-installed via `sudo apt-get install libglfw3 libglfw3-dev`):
```bash
g++ -std=c++17 src/main.cpp src/Engine/*.cpp src/Core/*.cpp dependencies/glad.c -I dependencies/include -I src -lglfw -o bin/app && ./bin/app
```

#### MacOS:
Using the included GLFW library:
```bash
g++ -std=c++17 src/main.cpp src/Engine/*.cpp src/Core/*.cpp dependencies/glad.c -I dependencies/include -I src dependencies/lib/libglfw.3.3.dylib -o bin/app && ./bin/app
```

Or if GLFW is installed via Homebrew (`brew install glfw`):
```bash
g++ -std=c++17 src/main.cpp src/Engine/*.cpp src/Core/*.cpp dependencies/glad.c -I dependencies/include -I src -lglfw -o bin/app && ./bin/app
```