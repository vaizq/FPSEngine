#version 330 core

/*
uniform vec3 color;
in vec3 fragPosition;
in vec3 normal;
out vec4 fragColor; // Output variable for fragment shader
vec3 lightPosition = vec3(0.0, 100.0, 0.0);

void main()
{
    vec3 lightDirection = fragPosition - lightPosition;
    float lightIntensity = max(0.5, dot(normalize(normal), normalize(-lightDirection)));
    fragColor = vec4(color, 1.0f);
}
*/

in vec2 texCoord;
in vec3 fragPosition;
in vec3 normal;
uniform vec3 color;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;
out vec4 fragColor; // Output variable for fragment shader

void main()
{
    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    // ambient
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 lightDirection = fragPosition - lightPosition;
    float diff = max(dot(normalize(normal), normalize(-lightDirection)), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 reflectDirection = normalize(reflect(-lightDirection, normal));
    vec3 viewDirection = normalize(cameraPosition - fragPosition);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 16);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = ambient + diffuse + specular;

    fragColor = vec4(result * color, 1.0);
}

