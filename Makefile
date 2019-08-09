MAIN=app

all:
	g++ $(MAIN).cpp -o $(MAIN) -lstdc++fs `wx-config --libs --cxxflags --gl-libs` -lGL -lGLU -lGLEW -limebra -std=c++17

clean:
	rm $(MAIN)
