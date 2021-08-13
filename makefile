CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

vkExperiment: main.cc app.cc shaders
	g++ $(CFLAGS) -o vkExperiment main.cc app.cc $(LDFLAGS)

shaders: shaders/vert.spv shaders/frag.spv
shaders/vert.spv: shaders/basic.vert
	glslc ./shaders/basic.vert -o shaders/vert.spv
shaders/frag.spv: shaders/basic.frag
	glslc ./shaders/basic.frag -o shaders/frag.spv

test: vkExperiment
	./vkExperiment
