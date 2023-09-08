
varying vec3 v_vertex;
varying vec3 v_vertNormal;
varying vec2 v_uv;

uniform vec3 u_lightPos;
uniform vec3 u_color;
uniform sampler2D u_texture;

void main() {
   vec3 L = normalize(u_lightPos - v_vertex);
   float NL = clamp(abs(dot(normalize(v_vertNormal), L)), -1.0, 1.0);
   float spec = pow(NL, 32.0);
   vec3 color = u_color * texture2D(u_texture, v_uv).xyz;
   vec3 col = color * (0.5 + 0.5 * NL + spec * 0.0);
   gl_FragColor = vec4(col, 1.0);
}
