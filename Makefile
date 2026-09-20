CXX = clang++
CC = clang
CXXFLAGS = -std=c++17 -O3 -Wall -Wno-deprecated -I dependencies/include -I dependencies/include/imgui -I src
CFLAGS = -O3 -Wall -Wno-deprecated -I dependencies/include
LDFLAGS = dependencies/lib/libglfw.3.3.dylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

SRCS_CPP = src/main.cpp \
           src/Engine/Camera.cpp \
           src/Engine/Mesh.cpp \
           src/Engine/Object.cpp \
           src/Engine/Primitives.cpp \
           src/Engine/Renderer.cpp \
           src/Engine/Shader.cpp \
           src/Engine/VertexArray.cpp \
           src/Engine/VertexBuffer.cpp \
           src/Core/BlackHole.cpp \
           src/Core/Photon.cpp \
           src/Core/Spacetime.cpp \
           dependencies/include/imgui/imgui.cpp \
           dependencies/include/imgui/imgui_draw.cpp \
           dependencies/include/imgui/imgui_tables.cpp \
           dependencies/include/imgui/imgui_widgets.cpp \
           dependencies/include/imgui/imgui_impl_glfw.cpp \
           dependencies/include/imgui/imgui_impl_opengl3.cpp

SRCS_C = dependencies/glad.c

OBJS = $(patsubst %.cpp,obj/%.o,$(SRCS_CPP)) $(patsubst %.c,obj/%.o,$(SRCS_C))

TARGET = bin/app

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p bin
	$(CXX) $(OBJS) $(LDFLAGS) -o $(TARGET)
	@echo "Build complete: $(TARGET)"

obj/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

test:
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) tests/test_physics.cpp src/Core/BlackHole.cpp src/Core/Spacetime.cpp src/Core/Photon.cpp -o bin/test_physics
	./bin/test_physics

clean:
	rm -rf bin/app bin/test_physics bin/*.dSYM obj

.PHONY: all run test clean
