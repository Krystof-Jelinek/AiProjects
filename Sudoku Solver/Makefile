# Název výsledného spustitelného souboru
TARGET = a.out

# Překladač a jeho volby
CXX = g++
CXXFLAGS = -Wall -std=c++17

# Soubory
SRC = sudoku.cpp
OBJ = $(SRC:.cpp=.o)

# Linkovací volby (libcurl)
LDLIBS = -lcurl

# Výchozí cíl
all: $(TARGET)

# Pravidlo pro sestavení spustitelného souboru
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

# Pravidlo pro čištění
clean:
	rm -f $(TARGET) $(OBJ)
