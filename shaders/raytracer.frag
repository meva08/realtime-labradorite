#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

// const local variables
const vec4 colors[4] = vec4[4](
    vec4(70.0,  60.0, 60.0, 50.0),   // x <= 0.5
    vec4(72.0,  67.0, 16.0, 16.0),   // x = 0.7
    vec4(90.0,  60.0, 20.0, 16.0),   // x = 0.8
    vec4(152.0, 60.0, 20.0, 16.0)    // x >= 0.9
);

const float thresholds[4] = float[4](0.2, 0.7, 0.8, 0.9);

void main()
{
    // get actual noise val
    float n = texture(ourTexture, TexCoord).r;

    vec4 color;

    if (n <= thresholds[0])
    {
        color = colors[0];
    }
    else if (thresholds[3] <= n)
    {
        color = colors[3];
    }
    else
    {
        // get interpolated value
        int i = 0;
        if ((thresholds[1] < n) && (n <= thresholds[2]))
        {
            i = 1;
        }
        else if ((thresholds[2] < n) && (n <= thresholds[3]))
        {
            i = 2;
        }

        float t = (n - thresholds[i]) / (thresholds[i + 1] - thresholds[i]);
        color = mix(colors[i], colors[i + 1], t);
    }


    FragColor = vec4(color.x / 152.0, color.y / 67.0, color.z / 60.0, 1.0); // convert color back into rgb
} 