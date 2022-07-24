//global variables from the CPU
uniform mat4 model;
uniform mat4 viewprojection;

//vars to pass to the pixel shader
varying vec3 v_wPos;
varying vec3 v_wNormal;
//vars to pass to the pixel shader
varying vec2 v_coord; //from texture.vs


//here create uniforms for all the data we need here

void main()
{	
	//convert local coordinate to world coordinates
	vec3 wPos = (model * vec4( gl_Vertex.xyz, 1.0)).xyz;
	//convert local normal to world coordinates
	vec3 wNormal = (model * vec4( gl_Normal.xyz, 0.0)).xyz;

	//pass them to the pixel shader interpolated
	v_wPos = wPos;
	v_wNormal = wNormal;
	
	//get the texture coordinates (per vertex) and pass them to the pixel shader
	v_coord = gl_MultiTexCoord0.xy; //slides

	//project the vertex by the model view projection 
	gl_Position = viewprojection * vec4(wPos,1.0); //output of the vertex shader
}