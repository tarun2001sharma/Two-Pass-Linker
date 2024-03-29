# Specifies the compiler to be used for compiling the C++ source files.
CXX = g++

# Defines the flags to be passed to the compiler. Here, '-w' suppresses all warnings,
# and '-std=c++2a' specifies the C++ standard to use (C++20 in this case).
CXXFLAGS = -w -std=c++2a

# Lists all the source files that need to be compiled. If you have multiple source files,
# add them to this list separated by spaces. For example, SOURCES = main.cpp file1.cpp file2.cpp
SOURCES = linker.cpp

# Converts the list of source files in the SOURCES variable to a list of object files.
# This substitution replaces the '.cpp' extension of each source file name with '.o'.
OBJECTS = $(SOURCES:.cpp=.o)

# Specifies the name of the final executable that will be generated by the linker.
EXECUTABLE = linker

# The default target. It depends on the EXECUTABLE target below. Typing 'make' without any arguments
# will execute this target. It ensures that all source files are compiled and the final executable is built.
all: $(SOURCES) $(EXECUTABLE)

# This target links the object files into the final executable. It depends on the object files listed in OBJECTS.
# The command uses the compiler $(CXX) to link the object files $(OBJECTS), producing the executable named $(EXECUTABLE).
$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@

# This is a pattern rule for compiling any '.cpp' file to its corresponding '.o' object file.
# '$<' is an automatic variable that represents the name of the prerequisite (the source file),
# and '$@' represents the name of the target (the object file).
.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

# The 'clean' target is a phony target that doesn't correspond to a file name. It's used to remove
# all the object files and the executable to clean up the directory. It can be executed by calling 'make clean'.
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)