uniform vec3 light_pos;
uniform vec3 light_dif;
uniform vec3 light_spc;
uniform vec3 light_amb;
uniform vec3 material_dif;
uniform vec3 material_spc;
uniform vec3 material_amb;
uniform vec3 eye_pos;
uniform float material_shin;
uniform float task;

//this var comes from the vertex shader
//they are baricentric interpolated by pixel according to the distance to every vertex
varying vec3 v_wPos;
varying vec3 v_wNormal;
varying vec2 v_coord; //slides
uniform sampler2D color_texture; //slides
uniform sampler2D normal_texture; //Task 3

//here create uniforms for all the data we need here

void main()
{
	//Processo la task que toqui

	//vec4 color;
	//if(int(task) == 1){ks = material_spc;color = texture2D( color_texture, v_coord);}//Agafat de texture.fs //Task 1
	//else if(int(task) == 2){ks = color.xyz * color.w; color = texture2D(color_texture, v_coord);}//Task 2
	//else if(int(task) == 3){ks = color.xyz * color.w; color = texture2D(normal_texture, v_coord);}//Task 3
	
	vec3 ks;
	vec4 color = texture2D( color_texture, v_coord);//Agafat de texture.fs
	vec4 normal;
	if(int(task) == 1){ks = material_spc; normal = vec4(v_wNormal,0.0);}//Apartat 1
	else if(int(task) == 2){ks = color.xyz * color.w;normal = vec4(v_wNormal,0.0);}//Apartat 2
	else if(int(task) == 3){ks = color.xyz * color.w; normal = texture2D( normal_texture, v_coord);normal.xyz = clamp(normal.xyz,-1.0,1.0);} //Tornem a posar els valors de les normals entre -1 i 1 agafant les normals de normalmap
	//here we set up the normal as a color to see them as a debug
	//here write the computations for PHONG.
	//for GOURAUD you dont need to do anything here, just pass the color from the vertex shader
	vec3 L = normalize(light_pos - v_wPos); //vector (P a Light pos)
	vec3 N = normalize(normal.xyz); //La Normal del vector
	vec3 R = normalize(-reflect(L, N)); //L reflected en N
	vec3 V =  normalize(eye_pos - v_wPos); //Vector desde el punt a camara eye
	float LdotN = dot(L, N);
	LdotN = clamp(LdotN,0.0,1.0); //Arreglat amb el clamp, calia posar floats
	float RdotV = dot(R, V);
	RdotV = clamp(RdotV,0.0,1.0); //Arreglat amb el clamp, calia posar floats
	RdotV = pow(RdotV, material_shin); // (R.V)^a a és el factor de shininess del material 

	vec3 amb = material_amb * light_amb; //Ka * Ia 
	vec3 dif = material_dif * LdotN * light_dif; //Kd *(L.N) * Id 
	vec3 spc = ks * RdotV * light_spc; //Ks * (R.V)^a * Is 
	//compute color
	vec3 final_color = amb + dif + spc; //Ip
	final_color = final_color * color.xyz;//Apartat 1 & 2
	//set the ouput color por the pixel
	gl_FragColor = vec4(final_color,1.0);
}
