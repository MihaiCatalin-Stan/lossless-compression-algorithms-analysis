CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O3
INCLUDES := -I.

SRCS := RLE.cpp
OBJS := $(SRCS:.cpp=.o)
DEPS := $(SRCS:.cpp=.d)
TARGET := RLE

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MM $< | sed 's,\(.*\)\.o[ :]*,\1.o $@ : ,g' > $*.d

-include $(DEPS)

.PHONY: clean
clean:
	$(RM) $(OBJS) $(DEPS) $(TARGET)

