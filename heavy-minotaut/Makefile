############################################################################################
#											   #
#                          	Makefile for Heavy MinOTAut				   #
#											   #
############################################################################################

 CFLAGS=-std=c++11 \
#  -pedantic-errors \
#  -Wextra \
  -Wall \
  -Wsign-compare \
  -Wfloat-equal \
  -Wctor-dtor-privacy \
  -Woverloaded-virtual \
  -Wpointer-arith \
  -Wcast-align \
  -Wstrict-overflow=5 \
  -Wwrite-strings \
  -Wcast-qual \
  -Wswitch-default \
  -Wswitch-enum \
  -Wunreachable-code \
  -Wctor-dtor-privacy \
  -Woverloaded-virtual \
  -Winit-self \
  -ftrapv \
  -fdiagnostics-show-option \
  -Wctor-dtor-privacy \
  -g


#############################################################################################

# /usr/local/{include,lib} are already included in the compilation path by default

INCLUDE=-Iinclude/
INCLUDE_LVATA=-I../ondrik-libvata/include
LIBS_ADD=-L../ondrik-libvata/build/src -L/usr/local/lib
LIBS=-lvata -lboost_system -lboost_filesystem
SRCS= $(wildcard src/*.cpp) $(wildcard src/Common/*.cpp) $(wildcard src/*\/*.cpp) $(wildcard src/*\/*\/*.cpp)  # depth = 3
OBJS = $(patsubst src/%.cpp,build/%.o,$(SRCS))
TESTERS= $(wildcard testers/*.cpp) $(wildcard testers/*\/*.cpp) $(wildcard testers/*\/*\/*.cpp)  # depth = 3
OBJS_TESTERS = $(patsubst %.cpp,build/%.o,$(TESTERS))
OBJS_NOT_OPT = $(patsubst src/%.cpp,buildNotOpt/%.o,$(SRCS))
OBJS_TESTERS_NOT_OPT = $(patsubst %.cpp,buildNotOpt/%.o,$(TESTERS))
RM=rm -rf

# $< feeds the first dependency to the command
# $^ feeds all the dependencies to the command (?)

.PHONY: all minotaut

all: clean minotaut

notOpt: clean_NotOpt minotaut_NotOpt


minotaut: $(OBJS) $(OBJS_TESTERS)
	$(CXX) -O3 $(CFLAGS) $(INCLUDE) $(INCLUDE_LVATA) $^ $(LIBS_ADD) $(LIBS) -o $@ 

build/testers/%.o: testers/%.cpp 
	$(CXX) -c -O3 $(CFLAGS) $(INCLUDE) $(INCLUDE_LVATA) $< -o $@ 

build/%.o: src/%.cpp 
	$(CXX) -c -O3 $(CFLAGS) $(INCLUDE) $(INCLUDE_LVATA) $< -o $@ 

delete_objs:
	$(RM) $(OBJS) $(OBJS_TESTERS)

clean: delete_objs
	$(RM) minotaut


# For debugging we use -g (enables symbols) and also the lowest optimization level -O0

minotaut_NotOpt: $(OBJS_NOT_OPT) $(OBJS_TESTERS_NOT_OPT)
	$(CXX) -g -O0 $(CFLAGS) $(INCLUDE) $(INCLUDE_LVATA) $^ $(LIBS_ADD) $(LIBS) -o $@ 

buildNotOpt/testers/%.o: testers/%.cpp 
	$(CXX) -g -c -O0 $(CFLAGS) $(INCLUDE) $(INCLUDE_LVATA) $< -o $@ 

buildNotOpt/%.o: src/%.cpp 
	$(CXX) -g -c -O0 $(CFLAGS) $(INCLUDE) $(INCLUDE_LVATA) $< -o $@ 

delete_objs_NotOpt:
	$(RM) $(OBJS_NOT_OPT) $(OBJS_TESTERS_NOT_OPT)

clean_NotOpt: delete_objs_NotOpt
	$(RM) minotaut_NotOpt


#############################################################################################
#
#   Explanation of some CFLAGS:
#
# -pedantic-errors: 
#	 In some cases, the C and C++ standards specify that certain extensions are forbidden.
# -Wall:
#    Turn on "all" warnings.
# -Wextra:
#	 And then some more.
# -Wfloat-equal
#	 Because usually testing floating-point numbers for equality is bad.
# -Wshadow
#	 Warns whenever a local variable shadows another local variable, 
#	 parameter or global variable or whenever a built-in function is shadowed.
# -Wpointer-arith
#	 Warns if anything depends upon the size of a function or of void.
# -Wcast-align
# 	 Warns whenever a pointer is cast such that the required alignment of the target is 
#	 increased. For example, warn if a char * is cast to an int * on machines 
#	 where integers can only be accessed at two- or four-byte boundaries.
# -Wstrict-prototypes
#	 Warns if a function is declared or defined without specifying the argument types.
# -Wstrict-overflow=5
# 	 Warns about cases where the compiler optimizes based on the assumption that 
#	 signed overflow does not occur. (The value 5 may be too strict, see the manual page.)
# -Wwrite-strings 
#	 Gives string constants the type const char[length] so that copying the address of one 
#	 into a non-const char * pointer will get a warning.
# -Waggregate-return
#	 Warns if any functions that return structures or unions are defined or called.
# -Wcast-qual
#	 Warsn whenever a pointer is cast to remove a type qualifier from the target type.
# -Wswitch-default
#	 Warns whenever a switch statement does not have a default case.
# -Wswitch-enum
#	 Warns whenever a switch statement has an index of enumerated type and lacks a case 
#	 for one or more of the named codes of that enumeration.
# -Wconversion
#	 Warns for implicit conversions that may alter a value.
# -Wunreachable-code
#	 Warns if the compiler detects that code will never be executed.
# -Wctor-dtor-privacy
#    Warns when a class seems unusable, because all the constructors or destructors in 
#	 a class are private and the class has no friends or public static member functions.
# -Weffc++
#    Warns about violations of various style guidelines from Scott Meyers' Effective C++ books.
#	 If you use this option, you should be aware that the standard library headers do not obey
#	 all of these guidelines; you can use `grep -v' to filter out those warnings. 
# -Woverloaded-virtual
#    Warns when a derived class function declaration may be an error in defining a virtual 
#	 function. In a derived class, the definitions of virtual functions must match the type 
#	 signature of a virtual function declared in the base class. With this option, the 
#	 compiler warns when you define a function with the same name as a virtual function, but 
#	 with a type signature that does not match any declarations from the base class. 
# -fdiagnostics-show-option
#	 To have each controllable warning amended with the option which controls it, to 
#	 determine what to use with this option. 
# -Winit-self
# -g
#	 Puts debugging information into the executable such that you can debug it and step 
#	 through the source (unless you're proficient and reading assembly and like the 
#	 stepi command) of a program whilst it's executing.
# -ftrapv
#	 Will cause the program to abort on signed integer overflow (formally 
# 	 "undefined behaviour" in C).
#
#############################################################################################

