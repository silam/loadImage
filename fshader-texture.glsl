#version 150
//You need the texture coordinates to be passed in from the vertex shader (interpolated)

//You also need your uniform sampler so you can use those coordinates to look up a texel
uniform sampler2D texture;
uniform sampler2D spectexture;

in vec2 vtexCoord;

out vec4  fColor;
in vec3 position;
in vec3 vN;

in vec4 AmbientDiffuseColor;
in vec4 SpecularColor;
in float SpecularExponent;

uniform vec4 light_position;
uniform vec4 light_color;
uniform vec4 ambient_light;

void main()
{
	
	vec3 L = normalize( light_position.xyz - position.xyz);
	vec3 E = normalize(-position.xyz);
	vec3 N = normalize(vN);

	vec3 H = normalize(L+E);
	vec4 amb = texture2D(texture, vtexCoord) * ambient_light;
	vec4 diff = max(dot(L,N), 0.0) * texture2D(texture, vtexCoord) * light_color;
	vec4 spec = pow( max (dot(N,H), 0.0), SpecularExponent) *  SpecularColor * light_color  ;
	if(dot(L,N) < 0.0){
		spec = vec4(0,0,0,1);
	}
	fColor = amb + diff + spec;

	//you need to look up the appropriate color for this fragment based on the texture map and the texture cordinates
	//fColor *= texture2D(texture, vtexCoord);
	
}