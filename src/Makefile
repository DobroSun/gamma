.PHONY: debug release clean

CXX  := g++
DEPS := main.o buffer.o console.o init.o font.o interp.o hotloader.o
EXE_NAME  := main
CXX_FLAGS := -std=c++17 -fno-exceptions -fno-rtti -Wall -Wextra

LINK := -lSDL2 -lSDL2_image -lSDL2_ttf


debug: CXX_FLAGS := $(CXX_FLAGS) -g -O0
debug: $(DEPS)
	$(CXX) -o $(EXE_NAME) $(DEPS) $(CXX_FLAGS) $(LINK)

release: CXX_FLAGS := $(CXX_FLAGS) -DNDEBUG -O3
release: $(DEPS)
	$(CXX) -o $(EXE_NAME) $(DEPS) $(CXX_FLAGS) $(LINK)
	strip $(EXE_NAME)

buffer.cpp: array.h gap_buffer.h pch.h 
main.cpp: pch.h

%.o: %.cpp 
	$(CXX) -c -o $@ $^ $(CXX_FLAGS)

clean:
	rm -f $(DEPS) $(EXE_NAME)
