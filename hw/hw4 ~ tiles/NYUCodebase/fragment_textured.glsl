
uniform sampler2D diffuse;
varying vec2 texCoordVar;
//Color tint
uniform vec4 tint;

vec4 tintify(in vec4 c)
{
  float average = (c.r + c.g + c.b) / 3.0;
  return vec4(average, average, average, 1.0) * vec4(tint.rgb, 1.0);
}

void main() {
	vec4 color = texture2D(diffuse, texCoordVar);
	if (color.a < 0.5)
		discard;
	
    gl_FragColor = tintify(color) * tint.a + color * (1.0 - tint.a);
}