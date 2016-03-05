# Makefile for RoKost
# Jakob 
# 2016-03-01

BUILD?=dbg # rel

CXX=g++
FLAGS_dbg= -g
FLAGS_rel= -O3
CXXFLAGS=-std=c++11 -Wall -fPIC $(FLAGS_$(BUILD))

#LDFLAGS=

# Applications built include
#		rokostd	- web interface
#		harvest - comand line 'daemon' that reads csv and stores in database, does some magic
#		roman		- comman dline tool to create accounts, might be discontinued.

# Source files
rokostd_src=$(wildcard src/rokostd/*.cc)
harvest_src=$(wildcard src/harvest/*.cc)
rokostd_obj=$(patsubst src/%.cc,build/%.o,$(rokostd_src))
harvest_obj=$(patsubst src/%.cc,build/%.o,$(harvest_src))

INCLUDES=-Iinclude

.SECONDARY:	build bin

all: rokostd harvest

run:	rokostd docroot
	./bin/rokostd  --docroot ./docroot --http-address 0.0.0.0 --http-port 8081

docroot:	docroot/.
	ln -s /usr/share/Wt/resources docroot

rokostd:	LIBS+=-lwt -lwthttp -lwtdbo -lwtdbopostgres
rokostd:	$(rokostd_obj) | bin/.
	$(CXX)	$(CXXFLAGS) -o bin/$@ $^ $(LIBS)

harvest:	LIBS+=-lwt -lwtdbo -lwtdbopostgres
harvest:	$(harvest_obj) | bin/.
	$(CXX)	$(CXXFLAGS) -o bin/$@ $^ $(LIBS)

clean:
	-@rm -r ./build
	-@rm -r ./bin
	-@rm -r ./docroot

build/%.o:	src/%.cc #| build/.
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $^ -o $@

%.o : %.cc
	echo $@ from $^

%/.:
	mkdir -p $@
