BUILD_DIR = build
SHADER_DIR = src/shaders
EXEX = engine

all: build_project

configure:
	@if [ ! -d "$(BUILD_DIR)" ]; then \
		cmake -S . -B $(BUILD_DIR); \
	fi

build_project: configure
	cmake --build $(BUILD_DIR)

build_shaders:
	glslc -fshader-stage=vertex $(SHADER_DIR)/vertex.glsl -o $(SHADER_DIR)/vertex.spv
	glslc -fshader-stage=fragment $(SHADER_DIR)/fragment.glsl -o $(SHADER_DIR)/fragment.spv

run: build_project build_shaders
	./$(BUILD_DIR)/engine

clean:
	cmake --build build --target clean
	rm -rf shaders/*

.PHONY: all configure build_project build_shaders run clean

