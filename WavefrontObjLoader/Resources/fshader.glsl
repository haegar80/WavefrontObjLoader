#ifdef GL_ES

// It was expressed that some drivers required this next line to function properly
precision highp float;
#endif

uniform vec3 color;

void main()
{
    // Pass through our original color with full opacity.
    gl_FragColor = vec4(color, 0.0);
}