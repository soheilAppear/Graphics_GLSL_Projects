#version 150 // Specify which version of GLSL we are using.

out vec4 fragColor;

uniform int red;

float spect_result;

in vec4 out_VertexPos; // Fragment position
in vec3 out_Normal;    // Fragment position normal vector

vec3 camPos = vec3(2,2,2);   // I'm not sure about this at all!
//vec3 camPos = out_Normal;


in vec4 light_position_in_camera; // position of the light in camera coordinates

vec3 frag_to_camera;
vec3 frag_to_light;

float diffuseScalar(vec3 normal, vec3 lightDir, bool frontBackSame)
{
	/* Basic equation for diffuse shading */

	float diffuse = dot(normalize(lightDir), normalize(normal.xyz));


	if(frontBackSame)
		diffuse = abs(diffuse);
	else
		diffuse = clamp(diffuse, 0, 1);

	diffuse = diffuse/2 + .5;

	return diffuse;
}


float specularScalar()
{
	frag_to_camera = camPos - out_VertexPos.xyz;
	frag_to_light = ((light_position_in_camera.xyz) - (out_VertexPos.xyz));

	vec3 half_vector = (normalize(frag_to_light) + normalize(frag_to_camera))/2;
	
	vec3 normalized_half_vector = (normalize(half_vector));
	
	float dot_pro_result_for_spec = dot(out_Normal, normalized_half_vector);   // Very Important Part!
	//float dot_pro_result_for_spec = dot(out_Normal, half_vector);

	if(dot_pro_result_for_spec < 0)
	{
		dot_pro_result_for_spec = 0;
		return dot_pro_result_for_spec;
	}

	else
	{
		spect_result = pow(dot_pro_result_for_spec,10);
		return spect_result;
	}
}


void main() 
{

	/* Get position of light in camera coordinates. When we do
	 * headlight style rendering, the light will be at the position of
	 * the camera! */
	
	vec3 lightPos = vec3(0,0,0);

	/* Calculate a vector pointing from our current position (in
	 * camera coordinates) to the light position. */
	vec3 lightDir = lightPos - out_VertexPos.xyz;

	/* Calculate diffuse shading */
	float diffuse = diffuseScalar(out_Normal.xyz, lightDir, true);

	float specular = specularScalar();

	fragColor.xyz = (specular + diffuse)*vec3(.5, .5, .5);
	fragColor.a = 1;  // make sure it is opaque; don't let diffuse variable scale alpha.

}
