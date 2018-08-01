.PHONY: all test tool futharkdoc clean

TOOL_BASE_FILES=tool/src/tool.c tool/src/tbf.h tool/src/tbf.c tool/src/frei0r_dynamic.h tool/src/frei0r_dynamic.c tool/src/pam_image.h tool/src/pam_image.c
TOOL_COMMAND_FILES=tool/src/commands/describe_plugin.h tool/src/commands/apply_on_image.h tool/src/commands/compare_effects.h tool/src/commands/generate_c_futhark_wrapper.h
TOOL_SOURCE_FILES=$(TOOL_BASE_FILES) $(TOOL_COMMAND_FILES)

CORE_PATHS=filter/colorize
BUILD_PATHS=$(patsubst %,build/%.so,$(CORE_PATHS))
FUTHARK_PATHS=$(patsubst %,lib/github.com/nqpz/fut0r/%.fut,$(CORE_PATHS))
FUTHARK_PATHS+=lib/github.com/nqpz/fut0r/base.fut lib/github.com/nqpz/fut0r/effects.fut

all: tool $(BUILD_PATHS) futharkdoc

tool: tool/tool

tool/tool: $(TOOL_SOURCE_FILES)
	gcc -Wall -Wextra -I frei0r/include -o tool/tool tool/src/tool.c tool/src/tbf.c tool/src/frei0r_dynamic.c tool/src/pam_image.c -ldl

build/%.so: lib/github.com/nqpz/fut0r/%.fut
	./tool/generate-so $(patsubst build/%.so,%,$@)

futharkdoc: doc/index.html

doc/index.html: $(FUTHARK_PATHS)
	futhark-doc -o doc lib/github.com/nqpz/fut0r/effects.fut

test: all
	tool/compare-effects $(CORE_PATHS)

clean:
	rm -f tool/tool
	rm -rf build doc
