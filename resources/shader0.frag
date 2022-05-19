#version 330
#ifdef GL_ES
precision mediump float;
#endif

uniform float u_time;
uniform vec2 u_tr;
uniform vec2 u_bl;
uniform vec2 u_size;
uniform vec2 u_wsize;

float bw(float q) {
    return q > 0. && q < 1. ? 1. : .5;
}

void main() {
    vec2 st = (gl_FragCoord.st-u_bl)/(u_tr-u_bl);
    //gl_FragColor = vec4(bw(st.x), bw(st.y), 0.0, 1.0);
    gl_FragColor = vec4(st.x, st.y, mod(u_time,1.), 1.);
}