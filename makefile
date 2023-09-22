.PHONY: clean setup compile run test
all: run

setup:
	@if [ ! -e build/compile_commands.json ]; then \
		CXX=$(CXX) meson setup --wipe build; \
  fi

clean:
	$(RM) -r build

compile: setup
	meson compile -C build

run: compile
	./build/lev

test: compile
	./build/lev-tester
