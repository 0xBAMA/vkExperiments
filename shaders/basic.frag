#version 450
layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

void main() {
	// if(int(gl_FragCoord.x)%2==0&&int(gl_FragCoord.y)%2==0)
		// discard;
	outColor = vec4(fragColor, 1.0);
}
