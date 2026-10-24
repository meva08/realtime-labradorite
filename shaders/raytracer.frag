#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D mTexture;

// const local variables
const float thresholds[4] = float[4](0.2, 0.7, 0.8, 0.9);

// colors (0-255 RGB, divided by 255 for GLSL)
const vec3 flatBlue    = vec3(18.0,  48.0,  70.0)  / 255.0;
const vec3 periwinkle  = vec3(28.0,  70.0,  88.0)  / 255.0;
const vec3 lightBlue   = vec3(38.0,  95.0,  112.0) / 255.0;
const vec3 cyan = vec3(40.0, 95.0, 105.0) / 255.0;
const vec3 grey        = vec3(14.0,  44.0,  54.0)  / 255.0;

const vec3 teal        = vec3(60.0,  108.0,  88.0)  / 255.0;
const vec3 paleTeal    = vec3(100.0, 125.0,  78.0)  / 255.0;

const vec3 yellow      = vec3(120.0, 130.0,  70.0)  / 255.0;
const vec3 darkYellow  = vec3(115.0, 123.0,  68.0)  / 255.0;
const vec3 paleYellow  = vec3(234.0, 192.0,  101.0)  / 255.0;

const vec3 orange      = vec3(110.0, 100.0,  65.0)  / 255.0;
const vec3 darkOrange  = vec3(95.0,   88.0,  60.0)  / 255.0;
const vec3 lightOrange = vec3(125.0, 115.0,  75.0)  / 255.0;

vec3 ramp3(float n, float t0, vec3 c0, float t1, vec3 c1, float t2, vec3 c2)
{
    if (n <= t0) return c0;

    if (n < t1)
    {
        float t = smoothstep(t0, t1, n);
        return mix(c0, c1, t);
    }

    if (n < t2)
    {
        float t = smoothstep(t1, t2, n);
        return mix(c1, c2, t);
    }

    return c2;
}

vec3 ramp4(float n, float t0, vec3 c0, float t1, vec3 c1, float t2, vec3 c2, float t3, vec3 c3)
{
if (n <= t0) return c0;

    if (n < t1)
    {
        float t = smoothstep(t0, t1, n);
        return mix(c0, c1, t);
    }

    if (n < t2)
    {
        float t = smoothstep(t1, t2, n);
        return mix(c1, c2, t);
    }

    if (n < t3)
    {
        float t = smoothstep(t2, t3, n);
        return mix(c2, c3, t);
    }

    return c3;
}

// color functions determine the final color (based off of shader tree in original paper)
vec3 blueColor(float nmed, float nsmall)
{
    vec3 left = ramp3(nsmall, 0.3, cyan, 0.37, lightBlue, 1.0, periwinkle);
    vec3 right = ramp3(nsmall, 0.4, cyan, 0.5,  lightBlue, 0.8, grey);
    return mix(left, right, smoothstep(0.5, 0.7, nmed));

}

vec3 tealColor(float nmed, float nsmall)
{
    vec3 a = ramp3(nsmall, 0.1, cyan, 0.3, teal, 0.9, paleTeal);
    vec3 b = ramp3(nsmall, 0.4, cyan, 0.8, teal, 0.9, paleTeal);
    vec3 c = mix(teal, flatBlue, 0.5);

    vec3 color = mix(a, b, smoothstep(0.35, 0.55, nmed));
    color = mix(color, c, smoothstep(0.50, 0.70, nmed));
    return color;
}

vec3 yellowColor(float nmed, float nsmall)
{
    vec3 color;
    color = ramp3(nsmall, 0.3, yellow, 0.5, darkYellow, 0.9, paleYellow);
    color = mix(yellow, color, 0.25);
    return color;
}

vec3 orangeColor(float nmed, float nsmall)
{
    vec3 color;

    vec3 left = ramp4(nsmall, 0.17, orange, 0.17, darkOrange, 0.5, orange, 1.0, lightOrange);
    vec3 right = ramp3(nsmall, 0.15, orange, 0.17, darkOrange, 1.0, lightOrange);

    color = mix(left, right, smoothstep(0.5, 0.7, nmed));
    return color;
}

void main()
{
    // get actual noise val
    vec4 tex = texture(mTexture, TexCoord);
    float nlarge = tex.r;
    float nmed = tex.g;
    float nsmall = tex.b;


    vec3 cB = blueColor(nmed, nsmall);
    vec3 cT = tealColor(nmed, nsmall);
    vec3 cY = yellowColor(nmed, nsmall);
    vec3 cO = orangeColor(nmed, nsmall);



    float blueTeal = smoothstep(0.15, 0.65, nlarge);
    float tealYellow = smoothstep(0.35, 0.85, nlarge);
    float yellowOrange = smoothstep(0.60, 1.0, nlarge);

    vec3 color = mix(cB, cT, blueTeal);
    color = mix(color, cY, tealYellow);
    color = mix(color, cO, yellowOrange);

    FragColor = vec4(color, 1.0);
}