BUILD_TYPE ?= Debug
BUILD_DIR ?= build

.PHONY: build

build:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	$(MAKE) -C $(BUILD_DIR)


