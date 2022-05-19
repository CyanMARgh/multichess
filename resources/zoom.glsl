#version 330

uniform float u_time;
uniform vec2 u_bl, u_tr, u_mouse;
uniform sampler2D u_src;

uniform float mystery = -1;

struct ray {
    vec3 p0, dir;
};

ray refrFace(vec3 norm, ray r0, float n1, float n2) {
    vec3 i_n = (dot(r0.dir, norm) / dot(norm, norm)) * norm;
    vec3 i_t = r0.dir - i_n;

    vec3 o_t = (n1/n2) * i_t;
    vec3 o_n = -sqrt(1 - dot(o_t, o_t)) * norm;

    return ray(r0.p0, normalize(o_t + o_n));
}
ray reflFace(vec3 norm, ray r0) {
    vec3 i_n = (dot(r0.dir, norm) / dot(norm, norm)) * norm;
    vec3 i_t = r0.dir - i_n;

    return ray(r0.p0, normalize(i_t - i_n));
}
vec2 sphIntersect(ray r, in vec3 center, float rad) {
    vec3 oc = r.p0 - center;
    float b = dot(oc, r.dir);
    float c = dot(oc, oc) - rad*rad;
    float h = b*b - c;
    if (h<0.0) return vec2(-1.0);
    h = sqrt(h);
    return vec2(-b-h, -b+h);
}
float heightMap(ray r) {
    vec2 D = sphIntersect(r, vec3(0, 0, 0), 1);
    return D.x < 0 ? 100 : D.x;
}
ray rayByUv(vec2 uv) {
    return ray(vec3(0, 0, -25), normalize(vec3(uv.xy, 5)));
}
vec3 traceImage(ray r) {
    if (r.dir.z==0) return vec3(0);
    float L = -r.p0.z / r.dir.z;
    if (L<0) return vec3(0);
    vec2 uv = r.p0.xy + L * r.dir.xy;
    uv = uv / 5 + .5; uv.y = 1 - uv.y;
    if (uv.x<0||uv.x>1||uv.y<0||uv.y>1)return vec3(1, 0, 1);
    return texture2D(u_src, uv).rgb;
}

ray refractSphere(ray r, vec3 center, float rad, float n) {
    vec2 inter = sphIntersect(r, center, rad);

    if (inter.x > 0) {
        vec3 p_ = r.p0 + inter.x * r.dir;
        if (p_.z < 0) {
            r.p0 = p_;
            r = refrFace(normalize(p_ - center), r, 1, n);
            inter = sphIntersect(r, center, rad);
            p_ = inter.y * r.dir + r.p0;
            if (p_.z < 0) {
                r.p0 = p_;
                r = refrFace(-normalize(r.p0 - center), r, n, 1);
            }
        }
    }
    return r;
}
ray refractLens(ray r, vec3 center, float F1, float F2) {
    return ray(vec3(0),vec3(0));
}



vec3 trace(ray r, vec2 mouse) {
    //    vec3 sphereCenter = vec3(sin(u_time)*1.8, cos(3*u_time/5)*1.8, -1);
    vec3 sphereCenter = vec3(mouse, -1);
    float sphereRadius = 1;
    r = refractSphere(r, sphereCenter, sphereRadius, 1.3);
    return traceImage(r);
}

void main() {
    vec2 size = u_tr-u_bl;
    vec2 uv = (gl_FragCoord.st-u_bl)/size - .5;
    ray r0 = rayByUv(uv);
    vec2 mouse = (u_mouse-u_bl) / size - .5;
    gl_FragColor = vec4(trace(r0, mouse * 5), 1);
    //float H = (1 - heightMap(rayByUv(uv)) / 10) * 5;

    //gl_FragColor = vec4(H, H, H, 1);
    //    vec2 uvm = (u_mouse-u_bl)/(u_tr-u_bl);
    //    if (length(uv-uvm)<.1)uv = (uv-uvm)*.3+uvm;

    //    uv.y = 1 - uv.y;
    //    gl_FragColor = vec4(texture2D(u_src, uv).rgb, 1.);
    //    gl_FragColor = vec4(uv, mod(u_time, 1.), 1.);
    //    gl_FragColor = vec4(uv, u_W, 1.);

    if (mystery > 0)  {
        gl_FragColor = vec4(1.) * u_time * u_bl.x * u_tr.x * u_mouse.x * texture2D(u_src, vec2(0)).x;
        // * texture2D(u_src, vec2(0.));
    }
}
