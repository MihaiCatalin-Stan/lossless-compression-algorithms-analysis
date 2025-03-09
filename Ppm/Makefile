# 
# Reference arithmetic coding
# 
# Copyright (c) Project Nayuki
# MIT License. See readme file.
# https://www.nayuki.io/page/reference-arithmetic-coding
# 


CXXFLAGS += -std=c++11 -O1 -Wall -Wextra -fsanitize=undefined


.SUFFIXES:

.SECONDARY:

.DEFAULT_GOAL = all
.PHONY: all clean


OBJ = ArithmeticCoder.o BitIoStream.o PpmModel.o FrequencyTable.o
MAINS = PpmCompress PpmDecompress

all: $(MAINS)

clean:
	rm -f -- $(OBJ) $(MAINS:=.o) $(MAINS)
	rm -rf .deps

%: %.o $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp .deps/timestamp
	$(CXX) $(CXXFLAGS) -c -o $@ -MMD -MF .deps/$*.d $<

.deps/timestamp:
	mkdir -p .deps
	touch .deps/timestamp

-include .deps/*.d
