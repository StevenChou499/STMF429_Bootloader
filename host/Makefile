include ../toolchain.mk

CPP_SRC = main.cpp \
          host_app.cpp

CPP_INC = -I/host_app.hpp

CPP_FLAGS = -Wall \
            -g

OBJS = host_app.o \
       main.o

all: $(OUT)/$(BIN)

$(OUT)/%.o: $(CURDIR)/%.cpp
	@$(CPP_TOOL) -c $(CPP_FLAGS) $(CPP_INC) $< -o $@

$(OUT)/$(BIN): $(OBJS)
	@$(CPP_TOOL) $(OBJS) -o $@