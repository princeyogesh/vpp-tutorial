CXXFLAGS:=-std=c++14 -Wextra -Wall -g -O2
CXXFLAGS+= -Wno-unused-parameter
#CXXFLAGS+= -O0
LDFLAGS:=-g
LDLIBS:=-lvapiclient
# Customize the lines below if VPP headers and libraries are in non-standard paths
CXXFLAGS+= -I/home/yogesh/vpp-tutorial/vpp/src/vpp-api/vapi
CXXFLAGS+= -I/usr/include
#CXXFLAGS+= -I/home/yogesh/vpp-tutorial/vpp/src
#LDFLAGS+= -Wl,-rpath,/your/vpp/build-root/install-vpp_debug-native/vpp/lib
#LDFLAGS+= -L/your/vpp/build-root/install-vpp_debug-native/vpp/lib

all: api_example

clean:
	$(RM) api_example

.PHONY: all clean