# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
CMAKE_SOURCE_DIR = /tmp/tmp.0l7QRR63ui/nginx_memory_pool

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/tmp.0l7QRR63ui/nginx_memory_pool/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/nginx_memory_pool.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/nginx_memory_pool.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/nginx_memory_pool.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/nginx_memory_pool.dir/flags.make

CMakeFiles/nginx_memory_pool.dir/main.cpp.o: CMakeFiles/nginx_memory_pool.dir/flags.make
CMakeFiles/nginx_memory_pool.dir/main.cpp.o: /tmp/tmp.0l7QRR63ui/nginx_memory_pool/main.cpp
CMakeFiles/nginx_memory_pool.dir/main.cpp.o: CMakeFiles/nginx_memory_pool.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/tmp/tmp.0l7QRR63ui/nginx_memory_pool/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/nginx_memory_pool.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/nginx_memory_pool.dir/main.cpp.o -MF CMakeFiles/nginx_memory_pool.dir/main.cpp.o.d -o CMakeFiles/nginx_memory_pool.dir/main.cpp.o -c /tmp/tmp.0l7QRR63ui/nginx_memory_pool/main.cpp

CMakeFiles/nginx_memory_pool.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/nginx_memory_pool.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.0l7QRR63ui/nginx_memory_pool/main.cpp > CMakeFiles/nginx_memory_pool.dir/main.cpp.i

CMakeFiles/nginx_memory_pool.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/nginx_memory_pool.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.0l7QRR63ui/nginx_memory_pool/main.cpp -o CMakeFiles/nginx_memory_pool.dir/main.cpp.s

# Object files for target nginx_memory_pool
nginx_memory_pool_OBJECTS = \
"CMakeFiles/nginx_memory_pool.dir/main.cpp.o"

# External object files for target nginx_memory_pool
nginx_memory_pool_EXTERNAL_OBJECTS =

nginx_memory_pool: CMakeFiles/nginx_memory_pool.dir/main.cpp.o
nginx_memory_pool: CMakeFiles/nginx_memory_pool.dir/build.make
nginx_memory_pool: CMakeFiles/nginx_memory_pool.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/tmp/tmp.0l7QRR63ui/nginx_memory_pool/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable nginx_memory_pool"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/nginx_memory_pool.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/nginx_memory_pool.dir/build: nginx_memory_pool
.PHONY : CMakeFiles/nginx_memory_pool.dir/build

CMakeFiles/nginx_memory_pool.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/nginx_memory_pool.dir/cmake_clean.cmake
.PHONY : CMakeFiles/nginx_memory_pool.dir/clean

CMakeFiles/nginx_memory_pool.dir/depend:
	cd /tmp/tmp.0l7QRR63ui/nginx_memory_pool/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/tmp.0l7QRR63ui/nginx_memory_pool /tmp/tmp.0l7QRR63ui/nginx_memory_pool /tmp/tmp.0l7QRR63ui/nginx_memory_pool/cmake-build-debug /tmp/tmp.0l7QRR63ui/nginx_memory_pool/cmake-build-debug /tmp/tmp.0l7QRR63ui/nginx_memory_pool/cmake-build-debug/CMakeFiles/nginx_memory_pool.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/nginx_memory_pool.dir/depend

