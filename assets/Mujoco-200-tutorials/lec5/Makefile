GLFWROOT?=$(shell brew --prefix)
# MUJOCOPATH?=/Applications/MuJoCo.app/Contents/Frameworks
MUJOCOPATH=..

CFLAGS=-O2 -F$(MUJOCOPATH) -I$(GLFWROOT)/include -pthread
CXXFLAGS=$(CFLAGS) -std=c++17 -stdlib=libc++
ALLFLAGS=$(CXXFLAGS) -L$(GLFWROOT)/lib -Wl,-rpath,$(MUJOCOPATH)

TARGET=test

.PHONY: all
all:
	clang++ $(ALLFLAGS)    $(TARGET).cc      -framework mujoco -lglfw -o $(TARGET)
clean:
	rm $(TARGET)
