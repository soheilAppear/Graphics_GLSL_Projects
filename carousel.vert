#version 150 // Specify which version of GLSL we are using.

uniform mat4 ModelView;
uniform mat4 Projection;
uniform mat4 GeomTransform;
uniform mat3 NormalMat;

vec3 in_light = vec3(13.4, 250, -3.7);

in vec3 in_Position; // vertex position (object coordinates)
in vec3 in_Normal;   // normal vector   (object coordinates)

out vec4 out_VertexPos; // vertex position (camera coordinates)  
out vec3 out_Normal;	// normal vector   (camera coordinates)

out vec4 light_position_in_camera; // position of the light in camera coordinates

void main()
{
	vec4 pos = vec4(in_Position.x, in_Position.y, in_Position.z, 1.0);
	mat3 NormalMat = mat3(transpose(inverse(ModelView * GeomTransform)));   // is this order correct for GeomTransform Matrix?
	out_Normal = normalize(NormalMat * in_Normal);

	light_position_in_camera = (ModelView * GeomTransform) * vec4(in_light,1);
	out_VertexPos = (ModelView * GeomTransform) * vec4(in_Position, 1);
	gl_Position = Projection * (ModelView * GeomTransform) * pos; // object -> unhomogenized NDC
}