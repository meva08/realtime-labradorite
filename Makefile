all:
	g++ -std=gnu++17 -O2 -o main main.cpp app.cpp -lSDL3 -lglew32 -lopengl32