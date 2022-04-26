# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/bscuser/new/io-scheduler

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/bscuser/new/io-scheduler/build

# Include any dependencies generated for this target.
include src/network/CMakeFiles/network_engine.dir/depend.make

# Include the progress variables for this target.
include src/network/CMakeFiles/network_engine.dir/progress.make

# Include the compile flags for this target's objects.
include src/network/CMakeFiles/network_engine.dir/flags.make

src/network/CMakeFiles/network_engine.dir/rpcs.cpp.o: src/network/CMakeFiles/network_engine.dir/flags.make
src/network/CMakeFiles/network_engine.dir/rpcs.cpp.o: ../src/network/rpcs.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bscuser/new/io-scheduler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/network/CMakeFiles/network_engine.dir/rpcs.cpp.o"
	cd /home/bscuser/new/io-scheduler/build/src/network && /usr/bin/g++-10  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/network_engine.dir/rpcs.cpp.o -c /home/bscuser/new/io-scheduler/src/network/rpcs.cpp

src/network/CMakeFiles/network_engine.dir/rpcs.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/network_engine.dir/rpcs.cpp.i"
	cd /home/bscuser/new/io-scheduler/build/src/network && /usr/bin/g++-10 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bscuser/new/io-scheduler/src/network/rpcs.cpp > CMakeFiles/network_engine.dir/rpcs.cpp.i

src/network/CMakeFiles/network_engine.dir/rpcs.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/network_engine.dir/rpcs.cpp.s"
	cd /home/bscuser/new/io-scheduler/build/src/network && /usr/bin/g++-10 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bscuser/new/io-scheduler/src/network/rpcs.cpp -o CMakeFiles/network_engine.dir/rpcs.cpp.s

# Object files for target network_engine
network_engine_OBJECTS = \
"CMakeFiles/network_engine.dir/rpcs.cpp.o"

# External object files for target network_engine
network_engine_EXTERNAL_OBJECTS =

src/network/libnetwork_engine.a: src/network/CMakeFiles/network_engine.dir/rpcs.cpp.o
src/network/libnetwork_engine.a: src/network/CMakeFiles/network_engine.dir/build.make
src/network/libnetwork_engine.a: src/network/CMakeFiles/network_engine.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/bscuser/new/io-scheduler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libnetwork_engine.a"
	cd /home/bscuser/new/io-scheduler/build/src/network && $(CMAKE_COMMAND) -P CMakeFiles/network_engine.dir/cmake_clean_target.cmake
	cd /home/bscuser/new/io-scheduler/build/src/network && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/network_engine.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/network/CMakeFiles/network_engine.dir/build: src/network/libnetwork_engine.a

.PHONY : src/network/CMakeFiles/network_engine.dir/build

src/network/CMakeFiles/network_engine.dir/clean:
	cd /home/bscuser/new/io-scheduler/build/src/network && $(CMAKE_COMMAND) -P CMakeFiles/network_engine.dir/cmake_clean.cmake
.PHONY : src/network/CMakeFiles/network_engine.dir/clean

src/network/CMakeFiles/network_engine.dir/depend:
	cd /home/bscuser/new/io-scheduler/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/bscuser/new/io-scheduler /home/bscuser/new/io-scheduler/src/network /home/bscuser/new/io-scheduler/build /home/bscuser/new/io-scheduler/build/src/network /home/bscuser/new/io-scheduler/build/src/network/CMakeFiles/network_engine.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/network/CMakeFiles/network_engine.dir/depend

