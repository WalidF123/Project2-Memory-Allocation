# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Werror

# Targets
all: firstfit bestfit

# Build firstfit program
firstfit: firstfit.o
	$(CXX) $(CXXFLAGS) -o firstfit firstfit.o

# Build bestfit program
bestfit: bestfit.o
	$(CXX) $(CXXFLAGS) -o bestfit bestfit.o

# Compile firstfit.o
firstfit.o: firstfit.cpp
	$(CXX) $(CXXFLAGS) -c firstfit.cpp

# Compile bestfit.o
bestfit.o: bestfit.cpp
	$(CXX) $(CXXFLAGS) -c bestfit.cpp

# Clean up object and executable files
clean:
	rm -f *.o firstfit bestfit