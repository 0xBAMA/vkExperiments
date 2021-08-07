CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

vkExperiment: main.cc
	g++ $(CFLAGS) -o vkExperiment main.cc $(LDFLAGS)

.PHONY: test clean

test: vkExperiment
	./vkExperiment

clean:
	rm -f vkExperiment
