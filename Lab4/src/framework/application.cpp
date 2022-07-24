#include "application.h"
#include "utils.h"
#include "image.h"
#include "camera.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "light.h"
#include "material.h"
#include "random"

Camera* camera = NULL;
Mesh* mesh = NULL;
Matrix44 model_matrix;
Shader* shader = NULL;
Texture* texture = NULL;
Texture* normalmap = NULL;
Light* light = NULL;
Vector3 ambient_light(0.1,0.2,0.3);
Material* material = NULL;
Material* material1 = NULL; //Task 4
Material* material2 = NULL; //Task 4
float task; //Per canviar de task

float Application::r_number(void){
	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	return r;
}

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	// initialize attributes
	// Warning: DO NOT CREATE STUFF HERE, USE THE INIT 
	// things create here cannot access opengl
	int w,h;
	SDL_GetWindowSize(window,&w,&h);

	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(NULL);
}

//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;
	
	//here we create a global camera and set a position and projection properties
	camera = new Camera();
	camera->lookAt(Vector3(0,20,20),Vector3(0,10,0),Vector3(0,1,0));
	camera->setPerspective(60,window_width / window_height,0.1,10000);

	//then we load a mesh
	mesh = new Mesh();
	mesh->loadOBJ("../res/meshes/lee.obj");

	//load the texture
	texture = new Texture();
	if(!texture->load("../res/textures/lee_color_specular.tga"))
	{
		std::cout << "Texture not found" << std::endl;
		exit(1);
	}
	normalmap = new Texture();
	if(!normalmap->load("../res/textures/lee_normal.tga"))
	{
		std::cout << "Texture not found" << std::endl;
		exit(1);
	}

	//we load a shader
	shader = Shader::Get("../res/shaders/texture.vs","../res/shaders/texture.fs");

	//load whatever you need here
	Shader* phong = Shader::Get("../res/shaders/phong.vs","../res/shaders/phong.fs");
	shader = phong;

	light = new Light();
	light->position.set(5, 190, 77);
	light->diffuse_color.set(1,1,1);
	light->specular_color = light->diffuse_color;
	
	material = new Material();
	material->shininess = 1; //Més alt comencen a sortir les taques negres
	material->ambient.set(1,1,1);
	material->diffuse.set(1,1,1);

	task = 1;

	material1 = new Material();
	material2 = new Material();
	material1->shininess = 1; //Més alt comencen a sortir les taques negres
	material1->ambient.set(r_number(),r_number(),r_number());
	material1->diffuse.set(r_number(),r_number(),r_number());
	material2->shininess = 1; //Més alt comencen a sortir les taques negres
	material2->ambient.set(r_number(),r_number(),r_number());
	material2->diffuse.set(r_number(),r_number(),r_number());
}

//render one frame
void Application::render(void)
{
	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable( GL_DEPTH_TEST );

	//Get the viewprojection
	camera->aspect = window_width / window_height;
	Matrix44 viewprojection = camera->getViewProjectionMatrix();
	
	//enable the shader
	shader->enable();
	shader->setMatrix44("model", model_matrix); //upload info to the shader
	shader->setMatrix44("viewprojection", viewprojection); //upload info to the shader

	shader->setTexture("color_texture", texture, 0); //set texture in slot equivalent to the task
	shader->setTexture("normal_texture", normalmap, 1);
	shader->setVector3("light_pos", light->position);
	shader->setVector3("light_spc", light->specular_color);
	shader->setVector3("light_dif", light->diffuse_color);
	shader->setVector3("material_amb", material->ambient);
	shader->setVector3("material_dif",material->diffuse);
	shader->setVector3("material_spc", material->specular);
	shader->setFloat("material_shin", material->shininess);
	shader->setFloat("task",task);

	//render the data
	glDisable(GL_BLEND);

	if(task == 5){ //Aixo era per renderitzar mes figures nomes en el cas de la task 4, s ha de canviar la condicio del if per != 4
		mesh->render(GL_TRIANGLES);
	}

	if(task != 5){ //entra sempre en el loop, sino ho hem de canviar per == 4
			camera->eye.z = 70;
			for (int index = 0; index < 3; index++){
			if(index<2){
				model_matrix.m[12] = index * 25.0; //Distance between the faces
			} else{
					model_matrix.m[12] = (index-1) * -25.0; //Per fer un a la esquerra del original
			}
			shader->setMatrix44("model", model_matrix); //upload the transform matrix to the shader
			mesh->render(GL_TRIANGLES);
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		shader->setVector3("material_amb", material1->ambient);
		shader->setVector3("material_dif",material1->diffuse);
		shader->setVector3("material_spc", material1->specular);

		mesh->render(GL_TRIANGLES);

		shader->setVector3("material_amb", material2->ambient);
		shader->setVector3("material_dif",material2->diffuse);
		shader->setVector3("material_spc", material2->specular);

		mesh->render(GL_TRIANGLES);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
	}

	

	/*if(task == 4){ //Generacio de meshes amb diferents materials per la task 4
		camera->eye.z = 164;
		for(int j = 0; j < 6; j++){
			for (int index = 0; index < 6; index++){
			//Generacio materials
			material1 = new Material();
			material2 = new Material();
			material1->shininess = 1; //Més alt comencen a sortir les taques negres
			material1->ambient.set(r_number(),r_number(),r_number());
			material1->diffuse.set(r_number(),r_number(),r_number());
			material2->shininess = 1; //Més alt comencen a sortir les taques negres
			material2->ambient.set(r_number(),r_number(),r_number());
			material2->diffuse.set(r_number(),r_number(),r_number());


			model_matrix.m[12] = index * 25.0; //Distancia de les cares a la dreta
			shader->setMatrix44("model", model_matrix); //upload the transform matrix to the shader
			shader->setVector3("material_amb", material1->ambient);
			shader->setVector3("material_dif",material1->diffuse);
			shader->setVector3("material_spc", material1->specular);
			mesh->render(GL_TRIANGLES);


			model_matrix.m[12] = index * -25.0; //Distancia de les cares a la esquerra
			shader->setMatrix44("model", model_matrix); //upload the transform matrix to the shader
			shader->setVector3("material_amb", material2->ambient);
			shader->setVector3("material_dif",material2->diffuse);
			shader->setVector3("material_spc", material2->specular);
			mesh->render(GL_TRIANGLES);
		}
		if(j < 3){model_matrix.m[13] = j * 25.0;} //Construir amunt
		else{model_matrix.m[13] = (j-3) * -25.0;} //Construir abaix
		}
	}*/

	//disable shader
	shader->disable();

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

//called after render
void Application::update(double seconds_elapsed)
{
	
	if(keystate[SDL_SCANCODE_1]){
		light->position.x++;
		//std::cout<< light->position.x<<"\n";
	}
	if(keystate[SDL_SCANCODE_2]){
		light->position.x--;
		//std::cout<< light->position.x<<"\n";
	}
	if(keystate[SDL_SCANCODE_3]){
		light->position.y++;
		//std::cout<< light->position.y<<"\n";
	}
	if(keystate[SDL_SCANCODE_4]){
		light->position.y++;
		//std::cout<< light->position.y<<"\n";
	}
	if(keystate[SDL_SCANCODE_5]){
		light->position.z++;
		//std::cout<< light->position.z<<"\n";
	}
	if(keystate[SDL_SCANCODE_6]){
		light->position.z--;
		//std::cout<< light->position.z<<"\n";
	}
	if(keystate[SDL_SCANCODE_7]){
		material->shininess++;
		//std::cout<< material->shininess<<"\n";
	}
	if(keystate[SDL_SCANCODE_8]){
		material->shininess--;
		//std::cout<< material->shininess<<"\n";
	}

	if (keystate[SDL_SCANCODE_SPACE])
	{
		model_matrix.rotateLocal(seconds_elapsed,Vector3(0,1,0));
		//std::cout<<r_number()<<"\n";
	}

	if (keystate[SDL_SCANCODE_RIGHT])
		camera->eye = camera->eye + Vector3(1, 0, 0) * seconds_elapsed * 10.0;
	else if (keystate[SDL_SCANCODE_LEFT])
		camera->eye = camera->eye + Vector3(-1, 0, 0) * seconds_elapsed * 10.0;
	if (keystate[SDL_SCANCODE_UP])
		camera->eye = camera->eye + Vector3(0, 1, 0) * seconds_elapsed * 10.0;
	else if (keystate[SDL_SCANCODE_DOWN])
		camera->eye = camera->eye + Vector3(0, -1, 0) * seconds_elapsed * 10.0;

	
	if (keystate[SDL_SCANCODE_W])
		camera->eye = camera->eye + Vector3(0, 0, -1) * seconds_elapsed * 10.0;
	else if (keystate[SDL_SCANCODE_S])
		camera->eye = camera->eye + Vector3(0, 0, 1) * seconds_elapsed * 100.0;
		//std::cout<<camera->eye.z<<"\n";
		//std::cout<<seconds_elapsed<<"\n";

}

//keyboard press event 
void Application::onKeyPressed( SDL_KeyboardEvent event )
{
	if(keystate[SDL_SCANCODE_T]){
		switch (int(task))
		{
		case 1: task = 2; std::cout<<"Task: "<<task<<"\n"; break; //Canvi a Task 1
		case 2: task = 3; std::cout<<"Task: "<<task<<"\n"; break; //Canvi a task 2
		case 3: task = 4; std::cout<<"Task: "<<task<<"\n"; break; //Canvi a task 3
		case 4: task = 1; std::cout<<"Task: "<<task<<"\n"; break; //Canvi a task 4
		}
		update(1); //Perque al apretar no canvii d'estat massa rapid
	}
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	switch (event.keysym.scancode){
		//case SDL_SCANCODE_D: material->shininess--;
		case SDL_SCANCODE_R: Shader::ReloadAll(); break;
        case SDL_SCANCODE_ESCAPE: exit(0); break; //ESC key, kill the app
	}

}

//mouse button event
void Application::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse pressed
	{

	}
}

void Application::onMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse unpressed
	{

	}
}

//when the app starts
void Application::start()
{
	std::cout << "launching loop..." << std::endl;
	launchLoop(this);
}
