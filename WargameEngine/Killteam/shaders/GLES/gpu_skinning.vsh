#version 300 es
in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
in ivec4 weightIndices;
in vec4 weights;

uniform mat4 mvp_matrix;
uniform mat4 joints[128];

out vec3 v_normal;
out vec2 v_texCoord;

void main() {
    vec4 pos = vec4(Position, 1.0);
	vec4 norm = vec4(Normal, 1.0);
	
	vec4 newVertex = pos;
	vec4 newNormal = norm;
    
	if(weights.x > 0.0)//there are weights
	{
		newVertex = vec4(0.0);
		newNormal = vec4(0.0);
		for(int i = 0; i < 4; ++i)
		{
			newVertex += (pos * joints[weightIndices[i]]) * weights[i];
			newNormal += (norm * joints[weightIndices[i]]) * weights[i];
		}
	}
	
    v_normal = normalize(mvp_matrix * newNormal).xyz; 
    gl_Position = mvp_matrix * newVertex;
    v_texCoord = TexCoord;
}