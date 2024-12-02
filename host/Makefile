include ../toolchain.mk

CPP_SRC = host_app.cpp

CPP_INC = -I/host_app.hpp

CPP_FLAGS = -Wall \
            -g

OBJS = host_app.o

all: $(OUT)/$(BIN)

$(OUT)/%.o: $(CURDIR)/%.cpp
	@$(CPP_TOOL) -c $(CPP_FLAGS) $(CPP_INC) $< -o $@

$(OUT)/$(BIN): $(OBJS)
	@$(CPP_TOOL) $(OBJS) -o $@