BUILD_DIR=build
include $(N64_INST)/include/n64.mk

PROJECT_NAME = "repeat64"

N64_CXXFLAGS += -std=gnu++23 -Os -fno-exceptions -fsingle-precision-constant

src = $(wildcard src/*.cpp)
src += $(wildcard src/tests/*.cpp)
src += $(wildcard src/framework/*.cpp)
src += $(wildcard src/renderer/*.cpp)
src += $(wildcard src/utils/*.cpp)

assets_png = $(wildcard assets/*.rgba16.png)
assets_test = $(wildcard assets/*.test.7z)
assets_conv = $(patsubst assets/%,filesystem/%,$(assets_png:%.png=%.sprite))
# assets_conv += $(patsubst assets/%,filesystem/%,$(assets_test:%.test.7z=%.test))
test_dumps = $(assets_test:assets/%.test.7z=pack_data/%.test)

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

pack_data/%.test: assets/%.test.7z
	@mkdir -p $(dir $@)
	@echo "    [TEST-DUMP] $@ $<"
	# cp "$<" $@
	7z e "$<" -o$(dir $@) -y -bso0 -bsp0
	$(N64_BINDIR)/mkasset -c 3 -o $(dir $@) $@

filesystem/tests.pack: tools/data_converter tools/data_bundler

filesystem/tests.pack: $(test_dumps)
	@mkdir -p $(dir $@)
	@echo "    [PACK] $@"
	tools/data_bundler pack_data filesystem/tests.pack

# PC side tools
tools/data_converter: tools/dataConverter.cpp
	make -C tools

tools/data_bundler: tools/dataBundler.cpp
	make -C tools

$(BUILD_DIR)/$(PROJECT_NAME).dfs: $(assets_conv) filesystem/tests.pack
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
# sc64deployer --remote 192.168.0.6:9064 upload --tv ntsc *.z64
	sc64deployer upload --tv ntsc *.z64
	curl 192.168.0.6:9065/on

sc64_pal:
	make -j8
	curl 192.168.0.6:9065/off
	sleep 1
	sc64deployer --remote 192.168.0.6:9064 upload --tv pal *.z64
	curl 192.168.0.6:9065/on

clean:
	rm -rf $(BUILD_DIR) *.z64 src/testList.h

cleanAll:
	rm -rf $(BUILD_DIR) *.z64 src/testList.h
	rm -rf filesystem

cleanDumps:
	rm -rf filesystem/*.test
	rm -rf pack_data/*.test

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
