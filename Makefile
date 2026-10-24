all:
	g++ -std=gnu++20 -O2 -o main main.cpp app.cpp mesh.cpp -lSDL3 -lglew32 -lopengl32