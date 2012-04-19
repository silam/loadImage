#version 150

in vec4 vPosition; 
in vec2 texCoord;


in vec4 vAmbientDiffuseColor;
in vec3 vNormal;
in vec4 vSpecularColor;
in float vSpecularExponent;

out vec4 color;
out vec3 position;
out vec3 vN;

out vec4 AmbientDiffuseColor;
out vec4 SpecularColor;
out float SpecularExponent;


//you'll need to pass the texture coordinates over to the fragment shader, so you'll need an out

uniform mat4 model_view;
uniform mat4 projection;

out vec2 vtexCoord;


void
main()
{	
	
	vec4 vNormal = vec4(vNormal, 0.0);
    
	AmbientDiffuseColor = vAmbientDiffuseColor;
	SpecularColor = vSpecularColor;
	SpecularExponent = vSpecularExponent;
	
	vec4 veyepos = model_view*vPosition;
	
	vN = normalize(model_view * vNormal).xyz;
	
	//spec = vec4(1,1,1,1);
	position = veyepos.xyz;
	
	//don't forget to pass your texture coordinate through!
	vtexCoord = texCoord;

	gl_Position = projection * model_view*vPosition;
	

}