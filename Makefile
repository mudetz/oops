# C++ Makefile Template
# Copyright (c) 2018	Manuel Weitzman
# You may use/distribute this template under the terms of the MIT LICENSE

# HowTo:
#	Create a src/ dir for all .cpp files
#	Create a head/ dir for all .h files
#	In .cpp files import .h files as if they were in the same dir
#	You have available:
#		make			Compile binaries
#		make install		Install final exec to /usr/bin
#		make uninstall		Remove final exec from /usr/bin
#		make clean		Remove intermediate .o files
#		make distclean		Remove final executable
#		make cleanall		clean+distclean

# Final executable name
EXEC = oops

# Directories for sourcefiles, headers and object files
SRCDIR = src
HEADDIR = head
OBJDIR = obj

# Files will be detected automatically (they shall not be in subdirectories
# though)
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))

# Compiler options
CXX ?= /usr/bin/g++
CPPFLAGS = $(addprefix -I, $(HEADDIR)) -MMD -MP
CFLAGS = -O2 -Wall -Weffc++ -std=gnu++17 -Wextra \
	-Wcast-align -Wcast-qual -Wchar-subscripts -Wcomment \
	-Wdisabled-optimization -Wfloat-equal -Wformat -Wformat=2 \
	-Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport \
	-Winit-self -Winvalid-pch -Wunsafe-loop-optimizations \
	-Wlong-long -Wmissing-braces -Wmissing-field-initializers \
	-Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn \
	-Wpacked -Wparentheses -Wpointer-arith -Wredundant-decls \
	-Wreturn-type -Wsequence-point -Wshadow -Wsign-compare \
	-Wstack-protector -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch \
	-Wswitch-default -Wswitch-enum -Wtrigraphs -Wuninitialized \
	-Wunknown-pragmas -Wunreachable-code -Wunused -Wunused-function \
	-Wunused-label -Wunused-parameter -Wunused-value -Wunused-variable \
	-Wvariadic-macros -Wvolatile-register-var -Wwrite-strings \
	# -Waggregate-return -Wconversion -Winline -Wpadded -g
LDFLAGS =
LDLIBS = -lboost_program_options -lpthread

# Utilities used for output and others
ECHO = echo
RM = rm -rf
MKDIR = mkdir
INSTALL = install
FIND = find
CP = cp

# Makefile rules
.PHONY: all
all: $(OBJDIR) $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJDIR):
	@$(MKDIR) -p $@

.PHONY: install
install:
	$(INSTALL) $(EXEC) /usr/bin/$(EXEC)

.PHONY: uninstall
uninstall:
	$(RM) /usr/bin/$(EXEC)

.PHONY: cleanall
cleanall: clean distclean

.PHONY: clean
clean:
	$(FIND) . -iname '*.d'            -type f -delete
	$(FIND) . -iname '*.o'            -type f -delete
	$(FIND) . -iname '*.ko'           -type f -delete
	$(FIND) . -iname '*.obj'          -type f -delete
	$(FIND) . -iname '*.elf'          -type f -delete
	$(FIND) . -iname '*.ilk'          -type f -delete
	$(FIND) . -iname '*.map'          -type f -delete
	$(FIND) . -iname '*.exp'          -type f -delete
	$(FIND) . -iname '*.gch'          -type f -delete
	$(FIND) . -iname '*.pch'          -type f -delete
	$(FIND) . -iname '*.lib'          -type f -delete
	$(FIND) . -iname '*.a'            -type f -delete
	$(FIND) . -iname '*.la'           -type f -delete
	$(FIND) . -iname '*.lo'           -type f -delete
	$(FIND) . -iname '*.dll'          -type f -delete
	$(FIND) . -iname '*.so'           -type f -delete
	$(FIND) . -iname '*.so.*'         -type f -delete
	$(FIND) . -iname '*.dylib'        -type f -delete
	$(FIND) . -iname '*.exe'          -type f -delete
	$(FIND) . -iname '*.out'          -type f -delete
	$(FIND) . -iname '*.app'          -type f -delete
	$(FIND) . -iname '*.i*86'         -type f -delete
	$(FIND) . -iname '*.x86_64'       -type f -delete
	$(FIND) . -iname '*.hex'          -type f -delete
	$(FIND) . -iname '*.su'           -type f -delete
	$(FIND) . -iname '*.idb'          -type f -delete
	$(FIND) . -iname '*.pdb'          -type f -delete
	$(FIND) . -iname '*.mod*'         -type f -delete
	$(FIND) . -iname '*.cmd'          -type f -delete
	$(FIND) . -iname 'modules.order'  -type f -delete
	$(FIND) . -iname 'Module.symvers' -type f -delete
	$(FIND) . -iname 'Mkfile.old'     -type f -delete
	$(FIND) . -iname 'dkms.conf'      -type f -delete
	$(FIND) . -iname '*.dSYM'        -type d -empty -delete
	$(FIND) . -iname '.tmp_versions' -type d -empty -delete
	$(FIND) . -iname 'obj'           -type d -empty -delete

.PHONY: distclean
distclean:
	$(RM) $(EXEC)

-include $(wildcard $(OBJDIR)/*.d)
