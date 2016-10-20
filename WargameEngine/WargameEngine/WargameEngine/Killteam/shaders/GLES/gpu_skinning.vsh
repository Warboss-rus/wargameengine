attribute vec4 Position;
attribute vec3 Normal;
attribute vec2 TexCoord;
attribute vec4 weightIndices;
attribute vec4 weights;

uniform mat4 mvp_matrix;
uniform mat4 joints[128];
uniform mat4 invBindMatrices[128];

varying vec3 v_normal;
varying vec2 v_texCoord;

void main() {
    vec4 newVertex = Position;
	vec4 newNormal = vec4(Normal, 1.0);
    
	if(weights.x > 0.0)//there are weights
	{
		newVertex = vec4(0.0);
		newNormal = vec4(0.0);
		for(int i = 0; i < 4; ++i)
		{
			int index = int(weightIndices[i]);
			newVertex += (Position * invBindMatrices[index] * joints[index]) * weights[i];
			newNormal += (vec4(Normal, 1.0) * invBindMatrices[index] * joints[index]) * weights[i];
		}
	}
	
    v_normal = normalize(mvp_matrix * newNormal).xyz; 
    gl_Position = mvp_matrix * newVertex;
    v_texCoord = TexCoord;
}