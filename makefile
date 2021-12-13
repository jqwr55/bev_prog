COMPILER = g++

INCLUDE_PATH = ./common
COMPILER_FLAGS = -g -I $(INCLUDE_PATH)

SRC_FILES = $(wildcard ./chapter*/solution?.cpp)
BIN_FILES = $(subst .cpp,,$(SRC_FILES))

define make-target
$(subst .cpp,,$1): $1
	$(COMPILER) $(COMPILER_FLAGS) $1 -o $(subst .cpp,,$1)
endef

build: $(BIN_FILES)
	$(info ready to run)

clean_build: clean build

run: build
	./run.sh

clean:
	rm $(BIN_FILES)

$(foreach element, $(SRC_FILES), $(eval $(call make-target,$(element))))