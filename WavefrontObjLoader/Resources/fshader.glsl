#ifdef GL_ES

// It was expressed that some drivers required this next line to function properly
precision highp float;
#endif

varying vec3 f_Color;

void main()
{
    // Pass through our original color with full opacity.
    gl_FragColor = vec4(f_Color, 0.0);
}