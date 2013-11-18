

CPPCOMPILE = $(CCACHE) g++
INSTALL_DIR=$(HOME)/run/myGenSvr
INSTALL_LOG_DIR = $(INSTALL_DIR)/log
INSTALL_BIN_DIR = $(INSTALL_DIR)/bin
OPT_DIR=$(HOME)/opt/

INCLUDEDIR = -I../BaseCommon \
	-I../BaseServer \
	-I$(OPT_DIR)/include
LIBDIR = -L../BaseCommon \
	 -L../BaseServer \
	 -L$(OPT_DIR)/lib
APPENDLIB = -lpthread -lrt \
	-lBaseCommon \
	-lBaseServer \
	-lprofiler
DEP_LIB = 

CFLAGS = -Wall -g -pipe $(SRCDEBUG) -O2
LDFLAGS = $(LIBDIR) $(APPENDLIB)

SRCS = $(wildcard *.cpp) 
OBJS = $(SRCS:.cpp=.o) 
BIN = myGenSvr

all:$(BIN)

$(BIN):$(OBJS) $(DEP_LIB)
	$(CPPCOMPILE) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	
$(OBJS):%.o:%.cpp
	$(CPPCOMPILE)  $(CFLAGS) -c $(INCLUDEDIR) $< -o $@

clean:
	rm -f *.o
	rm -f $(BIN)
	
installbin:
	make
	cp $(BIN) $(INSTALL_BIN_DIR)

install:
	make
	mkdir -p $(INSTALL_LOG_DIR)
	mkdir -p $(INSTALL_BIN_DIR)
	cp $(BIN) $(INSTALL_BIN_DIR)
		
	