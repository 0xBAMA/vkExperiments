CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

vkExperiment: main.cc app.cc shaders
	g++ $(CFLAGS) -o vkExperiment main.cc app.cc $(LDFLAGS)

shaders: vert.spv frag.spv
vert.spv: basic.vert
	glslc ./basic.vert -o vert.spv
frag.spv: basic.frag
	glslc ./basic.frag -o frag.spv

test: vkExperiment
	./vkExperiment
