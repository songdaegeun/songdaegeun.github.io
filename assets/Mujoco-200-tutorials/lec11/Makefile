GLFWROOT?=$(shell brew --prefix)
# MUJOCOPATH?=/Applications/MuJoCo.app/Contents/Frameworks
MUJOCOPATH=..

CFLAGS=-O2 -F$(MUJOCOPATH) -I$(GLFWROOT)/include -pthread
CXXFLAGS=$(CFLAGS)  -stdlib=libc++
ALLFLAGS=$(CXXFLAGS) -L$(GLFWROOT)/lib -w -Wl,-rpath,$(MUJOCOPATH)

TARGET=test

SRCS = $(TARGET)
SRCS_ = $(addsuffix .c, $(SRCS))

# clang++ -std=c++17

.PHONY: all
all:
	gcc $(ALLFLAGS)    $(SRCS_)  -framework mujoco -lglfw -lnlopt -lm -o $(TARGET) 
clean:
	rm $(TARGET)
