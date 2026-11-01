CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Werror -I../tests/stubs -I../include

PET_TEST_OBJS = build/pet_tests.o build/pet.o build/species_chirp.o build/thumb_format.o build/audio_stub.o
BATTLE_TEST_OBJS = build/battle_tests.o build/battle.o

.PHONY: test web-test clean

test: build/pet_tests build/battle_tests web-test
	./build/pet_tests
	./build/battle_tests

web-test:
	node web_serial_tests.mjs

build:
	mkdir -p build

build/pet_tests: $(PET_TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(PET_TEST_OBJS) -o $@

build/pet_tests.o: pet_tests.cpp ../include/pet.h ../include/dex.h | build
	$(CXX) $(CXXFLAGS) -c pet_tests.cpp -o $@

build/pet.o: ../src/pet.cpp ../include/pet.h ../include/dex.h ../include/audio.h | build
	$(CXX) $(CXXFLAGS) -c ../src/pet.cpp -o $@

build/species_chirp.o: ../src/species_chirp.cpp ../include/species_chirp.h ../include/dex.h | build
	$(CXX) $(CXXFLAGS) -c ../src/species_chirp.cpp -o $@

build/thumb_format.o: ../src/thumb_format.cpp ../include/thumb_format.h | build
	$(CXX) $(CXXFLAGS) -c ../src/thumb_format.cpp -o $@

build/audio_stub.o: audio_stub.cpp ../include/audio.h | build
	$(CXX) $(CXXFLAGS) -c audio_stub.cpp -o $@

build/battle_tests: $(BATTLE_TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(BATTLE_TEST_OBJS) -o $@

build/battle_tests.o: battle_tests.cpp ../include/battle.h | build
	$(CXX) $(CXXFLAGS) -c battle_tests.cpp -o $@

build/battle.o: ../src/battle.cpp ../include/battle.h | build
	$(CXX) $(CXXFLAGS) -c ../src/battle.cpp -o $@

clean:
	rm -rf build
