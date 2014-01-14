uniform float time;
uniform float particleSystemHeight;
uniform float particleSpeed;
uniform float particleSpread;
uniform float particleSystemShape;
uniform float particleShape;
uniform float particleSize;
uniform vec4  particleSystemPosition;

// The model for the particle system consists of a hundred quads.
// These quads are simple (-1,-1) to (1,1) quads where each quad
// has a z ranging from 0 to 1. The z will be used to differenciate
// between different particles

varying float vColor;
varying vec2 vTexCoord;

void main(void)
{
  
   // Loop particles
   float t = fract(gl_Vertex.z + particleSpeed * time);
   // Determine the shape of the system
   float s = pow(t, particleSystemShape);

   vec3 pos;
	// Spread particles in a semi-random fashion
	pos.x = particleSpread * (1 + t) * s * sin(62.0 * gl_Vertex.z);
	pos.z = particleSpread * (1 + t) * s * cos(62.0 * gl_Vertex.z);
	// Particles goes up
	pos.y = -particleSystemHeight * t * t + particleSystemHeight * t;

	/*pos.x = particleSpread * s * sin(62.0 * gl_Vertex.z);
	pos.z = particleSpread * s * cos(163.0 * gl_Vertex.z);
	// Particles goes up
	pos.y = particleSystemHeight * t;*/

   // Billboard the quads.
   // The view matrix gives us our right and up vectors.
   pos += particleSize * (gl_Vertex.x * gl_ModelViewMatrixInverse[0] + gl_Vertex.y * gl_ModelViewMatrixInverse[1]).xyz;
   // And put the system into place
   pos += particleSystemPosition.xyz;

   
   gl_Position = gl_ModelViewProjectionMatrix * vec4(pos,1.0);
   
   vTexCoord = gl_Vertex.xy;
   vColor    = 1.0 - t;
   
}