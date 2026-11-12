CXX = g++

ARGUS_DIR = /usr/src/jetson_multimedia_api/argus
UTILS_DIR = $(ARGUS_DIR)/samples/utils

INCLUDES = -I$(ARGUS_DIR)/include \
           -I$(UTILS_DIR)

CXXFLAGS = -std=c++17 -Wall -Wextra $(INCLUDES)

LIBDIRS = -L/usr/lib/aarch64-linux-gnu/tegra \
          -L/usr/lib/aarch64-linux-gnu

LIBS = -lnvargus

TARGET = oneShot

OBJECTS = main.o ArgusHelpers.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBDIRS) $(LIBS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

ArgusHelpers.o: $(UTILS_DIR)/ArgusHelpers.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)
