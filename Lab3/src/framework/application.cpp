#include "application.h"
#include "utils.h"
#include "includes.h"
#include "utils.h"

#include "image.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "material.h"
#include "light.h"

Camera* camera = NULL;
Mesh* mesh = NULL;
Shader* shader = NULL;

//might be useful...
Material* material = NULL;
Light* light = NULL;
Light* second_light = NULL;
Light* third_light = NULL;
Shader* phong_shader = NULL;
Shader* gouraud_shader = NULL;

Vector3 ambient_light(0.1,0.2,0.3); //here we can store the global ambient light of the scene

float angle = 0;
int shader_selection = 1;
int faces = 5;

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
	if( !mesh->loadOBJ( "../res/meshes/lee.obj" ) )
		std::cout << "FILE Lee.obj NOT FOUND " << std::endl;

	//we load one or several shaders...
	phong_shader = Shader::Get("../res/shaders/gouraud.vs", "../res/shaders/gouraud.fs");
	gouraud_shader = Shader::Get("../res/shaders/phong.vs", "../res/shaders/phong.fs");
	
	
	//load your Gouraud and Phong shaders here and stored them in some global variables
	//gouraud_shader = Shader::Get("../res/shaders/gouraud.vs", "../res/shaders/gouraud.fs");
	//phong_shader = Shader::Get("../res/shaders/phong.vs", "../res/shaders/phong.fs");

	//CODE HERE:
	//create a light (or several) and and some materials
	light = new Light();
	light->position.set(0, 0, 0);
	light->diffuse_color.set(20.0,255.0,20.0);
	light->specular_color = light->diffuse_color;

	second_light = new Light();
	second_light->position.set(10, 10, 10);
	second_light->diffuse_color.set(255,0,0);
	second_light->specular_color = second_light->diffuse_color;

	third_light = new Light();
	third_light->position.set(5, 5, 5);
	third_light->diffuse_color.set(250,250,250);
	third_light->specular_color = third_light->diffuse_color;


	material = new Material();
	material->shininess = 5;
}

//render one frame
void Application::render(void)
{
	//update the aspect of the camera acording to the window size
	camera->aspect = window_width / window_height;
	camera->updateProjectionMatrix();
	//Get the viewprojection matrix from our camera
	Matrix44 viewprojection = camera->getViewProjectionMatrix();

	//set the clear color of the colorbuffer as the ambient light so it matches
	glClearColor(ambient_light.x, ambient_light.y, ambient_light.z, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear framebuffer and depth buffer 
	glEnable( GL_DEPTH_TEST ); //enable depth testing for occlusions
	glDepthFunc(GL_LEQUAL); //Z LESS or EQUAL 

	//choose a shader and enable it
	switch (shader_selection) {
		case 1: shader = gouraud_shader; break;
		case 2: shader = phong_shader; break;
	}

	shader->enable();

	Matrix44 model_matrix;
	model_matrix.setIdentity();
	model_matrix.translate(0,0,0); //example of translation
	model_matrix.rotate(angle, Vector3(0, 1, 0));
	shader->setMatrix44("model", model_matrix); //upload the transform matrix to the shader
	shader->setMatrix44("viewprojection", viewprojection); //upload viewprojection info to the shader

	//CODE HERE: pass all the info needed by the shader to do the computations
	//send the material and light uniforms to the shader


		shader->setVector3("light_pos", light->position);
		shader->setVector3("light_spc", light->specular_color);
		shader->setVector3("light_dif", light->diffuse_color);
		shader->setVector3("material_amb", material->ambient);
		shader->setVector3("material_dif",material->diffuse);
		shader->setVector3("material_spc", material->specular);
		shader->setFloat("material_shin", material->shininess);
	
	/**/

	//do the draw call into the GPU
	glDisable(GL_BLEND);
	if (faces == 0)
	{
		mesh->render(GL_TRIANGLES);
	}

	for (int index = 0; index < faces; index++){
		model_matrix.m[12] = index * 25.0; //Distance between the faces
		shader->setMatrix44("model", model_matrix); //upload the transform matrix to the shader
		mesh->render(GL_TRIANGLES);
	}
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	
		shader->setVector3("light_pos", second_light->position);
		shader->setVector3("light_spc", second_light->specular_color);
		shader->setVector3("light_dif", second_light->diffuse_color);
		shader->setVector3("material_amb", material->ambient);
		shader->setVector3("material_dif",material->diffuse);
		shader->setVector3("material_spc", material->specular);
		shader->setFloat("material_shin", material->shininess);

	mesh->render(GL_TRIANGLES);

	
		shader->setVector3("light_pos", third_light->position);
		shader->setVector3("light_spc", third_light->specular_color);
		shader->setVector3("light_dif", third_light->diffuse_color);
		shader->setVector3("material_amb", material->ambient);
		shader->setVector3("material_dif",material->diffuse);
		shader->setVector3("material_spc", material->specular);
		shader->setFloat("material_shin", material->shininess);
	

	mesh->render(GL_TRIANGLES);

	//disable shader when we do not need it any more
	shader->disable();

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

//called after render
void Application::update(double seconds_elapsed)
{
	if (keystate[SDL_SCANCODE_SPACE])
		angle += seconds_elapsed;

	if (keystate[SDL_SCANCODE_RIGHT])
		camera->eye = camera->eye + Vector3(1, 0, 0) * seconds_elapsed * 10.0;
	else if (keystate[SDL_SCANCODE_LEFT])
		camera->eye = camera->eye + Vector3(-1, 0, 0) * seconds_elapsed * 10.0;
	if (keystate[SDL_SCANCODE_UP])
		camera->eye = camera->eye + Vector3(0, 1, 0) * seconds_elapsed * 10.0;
	else if (keystate[SDL_SCANCODE_DOWN])
		camera->eye = camera->eye + Vector3(0, -1, 0) * seconds_elapsed * 10.0;

	
}

//keyboard press event 
void Application::onKeyPressed( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: exit(0); break; //ESC key, kill the app
		case SDLK_1: shader_selection = 1;break;
		case SDLK_2: shader_selection = 2;break;
		case SDLK_PLUS: faces++; break;
		case SDLK_MINUS: faces--; break;
		case SDLK_r: 
			Shader::ReloadAll();
			break; //ESC key, kill the app
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
