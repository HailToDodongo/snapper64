BUILD_DIR=build
include $(N64_INST)/include/n64.mk

PROJECT_NAME = "repeat64"

N64_CXXFLAGS += -std=gnu++20 -Os -fno-exceptions -fsingle-precision-constant

src = $(wildcard src/*.cpp)
src += $(wildcard src/tests/*.cpp)
src += $(wildcard src/framework/*.cpp)
src += $(wildcard src/renderer/*.cpp)
src += $(wildcard src/utils/*.cpp)

assets_png = $(wildcard assets/*.rgba16.png)
assets_test = $(wildcard assets/*.test)
assets_conv = $(patsubst assets/%,filesystem/%,$(assets_png:%.png=%.sprite))
assets_conv += $(patsubst assets/%,filesystem/%,$(assets_test:%.test=%.test))

all: $(PROJECT_NAME).z64

# Generate testList.h by scanning files in src/tests/
src/testList.h: $(sort $(wildcard src/tests/*.cpp))
	@echo "    [DEMO LIST] $@"
	@echo "// This file is auto-generated. Do not edit directly." > $@
	@for f in $(wildcard src/tests/*.cpp); do \
		name=$$(basename $$f .cpp); \
		echo "TEST_ENTRY($$name)" >> $@; \
	done

$(BUILD_DIR)/src/main.o: src/testList.h

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o $(dir $@) "$<"

filesystem/%.test: assets/%.test
	@mkdir -p $(dir $@)
	@echo "    [TEST-DUMP] $@ $<"
	cp "$<" $@
	$(N64_BINDIR)/mkasset -c 1 -o $(dir $@) $@

$(BUILD_DIR)/$(PROJECT_NAME).dfs: $(assets_conv)
$(BUILD_DIR)/$(PROJECT_NAME).elf: $(src:%.cpp=$(BUILD_DIR)/%.o)

$(PROJECT_NAME).z64: N64_ROM_TITLE="Repeat64"
$(PROJECT_NAME).z64: $(BUILD_DIR)/$(PROJECT_NAME).dfs

fonts:
	node tools/createFont.mjs assets/font.png src/font.h
	node tools/createFontDobs
	elta.mjs assets/font64.png src/font64.h

sc64:
	make -j8
	curl 192.168.0.6:9065/off
	sleep 1
	sc64deployer --remote 192.168.0.6:9064 upload --tv ntsc *.z64
	curl 192.168.0.6:9065/on

sc64_pal:
	make -j8
	curl 192.168.0.6:9065/off
	sleep 1
	sc64deployer --remote 192.168.0.6:9064 upload --tv pal *.z64
	curl 192.168.0.6:9065/on

clean:
	rm -rf $(BUILD_DIR) *.z64 src/testList.h
	rm -rf filesystem

cleanDumps:
	rm -rf assets/*.test
	rm -rf filesystem/*.test

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
