
uniform sampler2D diffuse;
varying vec2 texCoordVar;

void main() {
	vec4 color = texture2D(diffuse, texCoordVar);
	if (color.a < 0.5)
		discard;
    gl_FragColor = color;
}