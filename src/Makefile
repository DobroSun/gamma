.PHONY: debug release clean

CXX  := g++
DEPS := main.o buffer.o console.o init.o font.o interp.o hotloader.o input.o
EXE_NAME  := main
CXX_FLAGS := -std=c++17 -fno-exceptions -fno-rtti -Wall -Wextra

LINK := ../lib/lib/libSDL2.so -lSDL2_image -lSDL2_ttf


debug: CXX_FLAGS := $(CXX_FLAGS) -g -O0
debug: $(DEPS)
	$(CXX) -o $(EXE_NAME) $(DEPS) $(CXX_FLAGS) $(LINK)

release: CXX_FLAGS := $(CXX_FLAGS) -DNDEBUG -O3
release: $(DEPS)
	$(CXX) -o $(EXE_NAME) $(DEPS) $(CXX_FLAGS) $(LINK)
	strip $(EXE_NAME)

buildSDL2:
	mkdir -p ../lib
	cd /gamma/SDL2/; @make clean; ./configure --prefix=/gamma/lib; make -j8; make install -j8
	@echo 'Done.'

%.o: %.cpp 
	$(CXX) -c -o $@ $^ $(CXX_FLAGS)

clean:
	rm -f $(DEPS) $(EXE_NAME)


