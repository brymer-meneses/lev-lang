BUILD_TYPE ?= Debug
BUILD_DIR ?= build

.PHONY: build


build: 
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	$(MAKE) -C $(BUILD_DIR)

test: build
	./$(BUILD_DIR)/test_runner

testdata: build
	python3 scripts/generate_test_data.py
	

