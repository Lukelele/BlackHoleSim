
# Realistic Black Simulation
## using Ray Tracing and Einstein's General Relativity

Very much under early development and not complete



## To compile and run the project, navigate to the BlackHoleSim directory and run the bash command below:


For Linux (Make sure GLFW is pre-installed using "sudo apt-get install libglfw3 libglfw3-dev"):

`g++ src/*.cpp glad.c -I dependencies/include -lglfw -o bin/app`



## For MacOS (Make sure to install GLFW using "brew install glfw"):

`g++ src/*.cpp glad.c -I dependencies/include dependencies/lib/libglfw.3.3.dylib -o bin/app`