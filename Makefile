#
# Makefile
#

SUBDIRS = asrt doctest fmt
CC = gcc
CXX = g++
CXXFLAGS = -DNDEBUG

default: build

build:
	for subdir in $(SUBDIRS); do (cd $$subdir && $(MAKE) build); done

install:
	for subdir in $(SUBDIRS); do (cd $$subdir && $(MAKE) install); done

clean:
	for subdir in $(SUBDIRS); do (cd $$subdir && $(MAKE) clean); done
