
attribute vec4 a_vertex;
attribute vec3 a_normal;
attribute vec2 a_uv;

varying vec3 v_vertex;
varying vec3 v_vertNormal;
varying vec2 v_uv;

uniform mat4 u_projMatrix;
uniform mat4 u_mvMatrix;
uniform mat3 u_normalMatrix;

void main() {
   v_vertex = a_vertex.xyz;
   v_vertNormal = u_normalMatrix * a_normal;
   v_uv = a_uv;
   gl_Position = u_projMatrix * u_mvMatrix * a_vertex;
}
