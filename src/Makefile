CIBLE = Main
SRCS =  Camera.cpp Main.cpp Mesh.cpp Node.cpp Octree.cpp Ray.cpp VoxelGrid.cpp #Shader.cpp
LIBS =  -lglut -lGLU -lGL -lm #-lGLEW

CC = clang
CPP = clang++

FLAGS = -Wall -g -std=c++11 #-DGPU_SHADERS

CFLAGS = $(FLAGS)
CXXFLAGS = $(FLAGS)

OBJS = $(SRCS:.cpp=.o)

$(CIBLE): $(OBJS)
	$(CPP) $(LDFLAGS) -o $(CIBLE) $(OBJS) $(LIBS)
clean:
	rm -f  *~  $(CIBLE) $(OBJS)

Camera.o: Camera.h Vec3.h
Main.o: Vec3.h Camera.h Vertex.h Triangle.h Mesh.h Quad.h Ray.h Node.h VoxelGrid.h VoxelDAG.h Octree.h #Shader.h
Mesh.o: Mesh.h Triangle.h Vertex.h Ray.h Quad.h Node.h Vec3.h
Node.o: Node.h Mesh.h Triangle.h Vertex.h Ray.h Quad.h Vec3.h
Octree.o: Octree.h VoxelGrid.h Mesh.h Triangle.h Vertex.h Ray.h Quad.h Node.h Vec3.h
Ray.o: Ray.h Vec3.h
VoxelGrid.o: VoxelGrid.h Mesh.h Triangle.h Vertex.h Ray.h Quad.h Node.h Vec3.h
#Shader.o: Shader.cpp Shader.h


