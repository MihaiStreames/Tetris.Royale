CXX = g++
CXXFLAGS = -I/usr/local/include -Wall -Wextra -std=c++17
LDFLAGS = -lftxui-component -lftxui-dom -lftxui-screen

SOURCES = loginMenu.cpp registerMenu.cpp mainMenu.cpp preLobbyMenu.cpp viewTestData.cpp menuHandler.cpp settingsMenu.cpp inLobbyMenu.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean

