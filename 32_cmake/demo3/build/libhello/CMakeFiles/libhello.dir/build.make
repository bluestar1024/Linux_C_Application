# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dong/CApplication/32_cmake/demo3

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dong/CApplication/32_cmake/demo3/build

# Include any dependencies generated for this target.
include libhello/CMakeFiles/libhello.dir/depend.make

# Include the progress variables for this target.
include libhello/CMakeFiles/libhello.dir/progress.make

# Include the compile flags for this target's objects.
include libhello/CMakeFiles/libhello.dir/flags.make

libhello/CMakeFiles/libhello.dir/hello.c.o: libhello/CMakeFiles/libhello.dir/flags.make
libhello/CMakeFiles/libhello.dir/hello.c.o: ../libhello/hello.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dong/CApplication/32_cmake/demo3/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object libhello/CMakeFiles/libhello.dir/hello.c.o"
	cd /home/dong/CApplication/32_cmake/demo3/build/libhello && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/libhello.dir/hello.c.o   -c /home/dong/CApplication/32_cmake/demo3/libhello/hello.c

libhello/CMakeFiles/libhello.dir/hello.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/libhello.dir/hello.c.i"
	cd /home/dong/CApplication/32_cmake/demo3/build/libhello && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dong/CApplication/32_cmake/demo3/libhello/hello.c > CMakeFiles/libhello.dir/hello.c.i

libhello/CMakeFiles/libhello.dir/hello.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/libhello.dir/hello.c.s"
	cd /home/dong/CApplication/32_cmake/demo3/build/libhello && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dong/CApplication/32_cmake/demo3/libhello/hello.c -o CMakeFiles/libhello.dir/hello.c.s

libhello/CMakeFiles/libhello.dir/hello.c.o.requires:

.PHONY : libhello/CMakeFiles/libhello.dir/hello.c.o.requires

libhello/CMakeFiles/libhello.dir/hello.c.o.provides: libhello/CMakeFiles/libhello.dir/hello.c.o.requires
	$(MAKE) -f libhello/CMakeFiles/libhello.dir/build.make libhello/CMakeFiles/libhello.dir/hello.c.o.provides.build
.PHONY : libhello/CMakeFiles/libhello.dir/hello.c.o.provides

libhello/CMakeFiles/libhello.dir/hello.c.o.provides.build: libhello/CMakeFiles/libhello.dir/hello.c.o


# Object files for target libhello
libhello_OBJECTS = \
"CMakeFiles/libhello.dir/hello.c.o"

# External object files for target libhello
libhello_EXTERNAL_OBJECTS =

lib/libhello.a: libhello/CMakeFiles/libhello.dir/hello.c.o
lib/libhello.a: libhello/CMakeFiles/libhello.dir/build.make
lib/libhello.a: libhello/CMakeFiles/libhello.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dong/CApplication/32_cmake/demo3/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library ../lib/libhello.a"
	cd /home/dong/CApplication/32_cmake/demo3/build/libhello && $(CMAKE_COMMAND) -P CMakeFiles/libhello.dir/cmake_clean_target.cmake
	cd /home/dong/CApplication/32_cmake/demo3/build/libhello && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/libhello.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
libhello/CMakeFiles/libhello.dir/build: lib/libhello.a

.PHONY : libhello/CMakeFiles/libhello.dir/build

libhello/CMakeFiles/libhello.dir/requires: libhello/CMakeFiles/libhello.dir/hello.c.o.requires

.PHONY : libhello/CMakeFiles/libhello.dir/requires

libhello/CMakeFiles/libhello.dir/clean:
	cd /home/dong/CApplication/32_cmake/demo3/build/libhello && $(CMAKE_COMMAND) -P CMakeFiles/libhello.dir/cmake_clean.cmake
.PHONY : libhello/CMakeFiles/libhello.dir/clean

libhello/CMakeFiles/libhello.dir/depend:
	cd /home/dong/CApplication/32_cmake/demo3/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dong/CApplication/32_cmake/demo3 /home/dong/CApplication/32_cmake/demo3/libhello /home/dong/CApplication/32_cmake/demo3/build /home/dong/CApplication/32_cmake/demo3/build/libhello /home/dong/CApplication/32_cmake/demo3/build/libhello/CMakeFiles/libhello.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : libhello/CMakeFiles/libhello.dir/depend

