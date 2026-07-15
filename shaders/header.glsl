#define MAX_LIGHTS 4

struct directionalLightUBO {

    vec3 direction;
    float padding0;

    vec3 color;
    float padding1;
};

struct PointLightUBO {

    vec3 position;
    float constant;

    vec3 color;
    float linear;

    vec3 padding0;
    float quadratic;
};

struct SpotLightUBO {

    vec3 position;
    float cutOffangle;

    vec3 direction;
    float outerCutOff;

    vec3 color;
    float constant;

    float linear;
    float quadratic;

    bool isVisible;
};