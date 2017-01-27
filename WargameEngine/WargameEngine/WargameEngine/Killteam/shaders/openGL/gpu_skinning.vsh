#version 330 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;
layout (location = 3) in ivec4 weightIndices;
layout (location = 4) in vec4 weights;

uniform mat4 mvp_matrix;
uniform mat4 model_matrix;
uniform mat4 joints[256];

out vec3 v_normal;
out vec2 v_texCoord;

void main() {
    vec4 newVertex = vec4(Position, 1.0);
	vec4 newNormal = vec4(Normal, 1.0);
    
	if(weights.x > 0.0)//there are weights
	{
		newVertex = vec4(0.0);
		newNormal = vec4(0.0);
		for(int i = 0; i < 4; ++i)
		{
			newVertex += (vec4(Position, 1.0) * joints[weightIndices[i]]) * weights[i];
			newNormal += (vec4(Normal, 1.0) * joints[weightIndices[i]]) * weights[i];
		}
	}
	
    v_normal = normalize(mat3(transpose(inverse(model_matrix))) * newNormal.xyz); 
    gl_Position = mvp_matrix * newVertex;
    v_texCoord = TexCoord;
}