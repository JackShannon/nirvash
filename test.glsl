-- Vertex
#version 150
uniform mat4 ModelViewProjection;

in vec3 vPosition;
in vec3 vNormal;

out vec3 Normal;

void main()
{
	Normal = vNormal;
	gl_Position = ModelViewProjection * vec4(vPosition, 1.0);
}

-- Fragment
#version 150

uniform vec3 LightDirection;

in vec3 Normal;

out vec4 FragColor;

void main()
{
	float intensity = dot(Normal, LightDirection);
	FragColor = vec4(vec3(intensity), 1.0);
}
