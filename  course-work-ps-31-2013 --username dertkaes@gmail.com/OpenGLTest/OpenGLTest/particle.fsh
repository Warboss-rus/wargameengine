uniform sampler2D Flame;
uniform float particleShape;
varying float vColor;
varying vec2  vTexCoord;

void main(void)
{
   // Fade the particle to a circular shape
   float fade = max(pow(dot(vTexCoord, vTexCoord), particleShape), 0.0);
   gl_FragColor = (1.0 - fade) * texture2D(Flame, vec2(vColor,0.0));
   
  
}