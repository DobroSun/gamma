.PHONY: debug release clean

CXX  := g++
DEPS := main.o buffer.o console.o init.o font.o interp.o
EXE_NAME  := main
CXX_FLAGS := -std=c++17 -Wall -Wextra -fno-exceptions -fno-rtti

LINK := -lSDL2 /usr/lib/x86_64-linux-gnu/libSDL2_image.so /usr/lib/x86_64-linux-gnu/libSDL2_ttf.so lua/liblua54.a /usr/lib/x86_64-linux-gnu/libdl.so


debug: $(DEPS)
	$(CXX) -o $(EXE_NAME) $(DEPS) $(CXX_FLAGS) -g -O0 $(LINK)

release: $(DEPS)
	$(CXX) -o $(EXE_NAME) $(DEPS) $(CXX_FLAGS) -DNDEBUG -O3 $(LINK)
	strip $(EXE_NAME)

%.o: %.cpp
	$(CXX) -c -o $@ $^ $(CXX_FLAGS) -g -O0

clean:
	rm -f $(DEPS) $(EXE_NAME)
