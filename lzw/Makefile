CXXFLAGS += -std=c++11 -O1 -Wall -Wextra -fsanitize=undefined

.DEFAULT_GOAL = all
.PHONY: all clean

OBJ = lzw.o
MAINS = lzw

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
