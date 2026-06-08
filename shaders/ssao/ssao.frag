#version 450 core

out float FragColor;
in  vec2  vTexCords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform mat4 projection;
uniform mat4 view;

// tile noise texture over screen, based on screen dimensions divided by noise size
const vec2 noiseScale = vec2(1280.0/4.0, 720.0/4.0); // screen resolution

void main() {

    vec3 fragPos   = (view * vec4(texture(gPosition, vTexCords).xyz, 1.0)).xyz;
    vec3 normal    = normalize(mat3(view) * texture(gNormal, vTexCords).xyz);
    vec3 randomVec = texture(texNoise, vTexCords * noiseScale).xyz;

    vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN       = mat3(tangent, bitangent, normal);
    
    float bias      = 0.025;
    float radius    = 2.5;
    float occlusion = 0.0;

    for (int i = 0; i < 64; i++) {

        vec3 samplePos = TBN * samples[i];

        samplePos = fragPos + samplePos * radius;
        vec4 offset = vec4(samplePos, 1.0);

        offset      = projection * offset;
        offset.xyz /= offset.w;
        offset.xyz  = offset.xyz * 0.5 + 0.5;

        float sampleDepth = (view * vec4(texture(gPosition, offset.xy).xyz, 1.0)).z;

        // occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0);

        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / 64.0);
    FragColor = occlusion;
    // FragColor = 1.0;
}