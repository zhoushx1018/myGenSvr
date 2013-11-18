################OPTION###################
CCOMPILE = gcc
CPPCOMPILE = $(CCACHE) g++
#COMPILEOPTION = -fPIC -c -g
COMPILEOPTION = -c -g -O2

OPT_DIR=$(HOME)/opt/

INCLUDEDIR = -I../BaseCommon \
	-I$(OPT_DIR)/include
AR = ar
OUTPUT = libBaseServer.a
AR_OPTION = rcs
LIBDIRS =

SRCS = $(wildcard *.cpp) 
OBJS = $(SRCS:.cpp=.o)

SHAREDLIB = 
APPENDLIB = 
$(OUTPUT): $(OBJS) 
	$(AR) $(AR_OPTION) $(OUTPUT) $(LIBDIRS) $(OBJS) 

clean: 
	rm -f $(OBJS)
	rm -f $(OUTPUT)

all: clean $(OUTPUT)
.cpp.o:
	$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR)  $*.cpp $(SRCDEBUG)
