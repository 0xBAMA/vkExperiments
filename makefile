CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

vkExperiment: main.cc app.cc
	g++ $(CFLAGS) -o vkExperiment main.cc app.cc $(LDFLAGS)

test: vkExperiment
	./vkExperiment
