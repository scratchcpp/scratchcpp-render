uniform mat4 u_projectionMatrix;
uniform mat4 u_modelMatrix;
in vec2 a_position;
in vec2 a_texCoord;

out vec2 v_texCoord;

void main() {
    gl_Position = u_projectionMatrix * u_modelMatrix * vec4(a_position, 0, 1);
    v_texCoord = a_texCoord;
}
