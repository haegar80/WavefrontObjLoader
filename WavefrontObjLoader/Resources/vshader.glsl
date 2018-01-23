#ifdef GL_ES

// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 modelview_matrix;
uniform mat4 projection_matrix;

//layout (location = 0) in vec3 in_Position;
attribute vec3 in_Position;
attribute vec3 in_Normal;
attribute vec2 in_Texture;

varying vec3 position_cameraspace;
varying vec3 normal_cameraspace;
varying vec2 frag_textureCoord;

void main()
{
    position_cameraspace = (modelview_matrix * vec4(in_Position, 1.0)).xyz;
    normal_cameraspace = (modelview_matrix * vec4(in_Normal, 0.0)).xyz;
    frag_textureCoord = in_Texture;

    gl_Position = projection_matrix * modelview_matrix * vec4(in_Position, 1.0);
}