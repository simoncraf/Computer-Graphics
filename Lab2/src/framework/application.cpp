#include "application.h"
#include "utils.h"
#include "image.h"
#include "mesh.h"


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

	state = EMPTY_MESH;
	zbuffer.resize(w, h);
	framebuffer.resize(w, h);

	
}

//Here we have already GL working, so we can create meshes and textures
//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;
	
	//here we create a global camera and set a position and projection properties
	camera = new Camera();
	camera->lookAt(Vector3(0,10,20),Vector3(0,10,0),Vector3(0,1,0)); //define eye,center,up
	camera->perspective(60, window_width / (float)window_height, 0.1, 10000); //define fov,aspect,near,far

	//load a mesh
	mesh = new Mesh();
	if( !mesh->loadOBJ("lee.obj") )
		std::cout << "FILE Lee.obj NOT FOUND" << std::endl;

	//load the texture
	texture = new Image();
	texture->loadTGA("color.tga");
}

//this function fills the triangle by computing the bounding box of the triangle in screen space and using the barycentric interpolation
//to check which pixels are inside the triangle. It is slow for big triangles, but faster for small triangles 
void Application::fillTriangle(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector2& texcoord, const Vector2& ucoordinates, const Vector2& ucoordinates2)
{
	//compute triangle bounding box in screen space

	//clamp to screen area

	//loop all pixels inside bounding box
	// for()

		//we must compute the barycentrinc interpolation coefficients, weights of pixel P(x,y)

		//check if pixel is inside or outside the triangle

		//here add your code to test occlusions based on the Z of the vertices and the pixel (TASK 5)

		//here add your code to compute the color of the pixel (barycentric interpolation) (TASK 4)

		//draw the pixels in the colorbuffer x,y 
		//framebuffer.setPixel(x, y, COMPUTED COLOR);
}

//render one frame
void Application::render(Image& framebuffer)
{
	framebuffer.fill(Color(40, 45, 60)); //clear
	zbuffer.fill(99999999.0); //Initialize zbuffer to huge numbers
	std::cout<<"Z in Zbuffer of pixel (x,y): (277,778) is z = "<<zbuffer.getPixel(277,778)<<"\n";
	//for every point of the mesh (to draw triangles take three points each time and connect the points between them (1,2,3,   4,5,6,   ...)

		for (int i = 0; i  < mesh->vertices.size(); i = i+3)
	{
		Vector3 vertex = mesh->vertices[i]; //extract vertex from mesh
		Vector3 vertex2 = mesh->vertices[i+1];
		Vector3 vertex3 = mesh->vertices[i+2];
		
		//Textures
		Vector2 texcoord = mesh->uvs[i]; //texture coordinate of the vertex (they are normalized, from 0,0 to 1,1)
		Vector2 texcoord2 = mesh->uvs[i+1];
		Vector2 texcoord3 = mesh->uvs[i+2];

		//Normalize the vectors form [0,1] to windows values
		texcoord.x = texcoord.x * texture->width;
        texcoord.y = texcoord.y * texture->height;
        texcoord2.x = texcoord2.x * texture->width;
        texcoord2.y = texcoord2.y * texture->height;
        texcoord3.x = texcoord3.x * texture->width;
        texcoord3.y = texcoord3.y * texture->height;
		

		//project every point in the mesh to normalized coordinates using the viewprojection_matrix inside camera
		Vector3 normalized_point = camera->projectVector(vertex);
		Vector3 normalized_point2 = camera->projectVector(vertex2);
		Vector3 normalized_point3 = camera->projectVector(vertex3);

		//convert from normalized (-1 to +1) to framebuffer coordinates (0,W)
		//Projected vertex
		Vector3 coordinates = Vector3((normalized_point.x * window_width/2) + window_width/2,(normalized_point.y * window_height/2) + window_height/2, normalized_point.z);
		Vector3 coordinates2 = Vector3((normalized_point2.x * window_width / 2) + window_width / 2, (normalized_point2.y * window_height / 2) + window_height / 2, normalized_point2.z);
		Vector3 coordinates3 = Vector3((normalized_point3.x * window_width / 2) + window_width / 2, (normalized_point3.y * window_height / 2) + window_height / 2,  normalized_point3.z);

		//paint points in framebuffer (using your drawTriangle function or the fillTriangle function)
		//framebuffer.drawTriangle(coordinates.x,coordinates.y,coordinates2.x,coordinates2.y,coordinates3.x,coordinates3.y,Color::WHITE,false);
		//framebuffer.drawTriangleInterpolated(coordinates,coordinates2,coordinates3,interpolate1, interpolate2, interpolate3, zbuffer);
		//Recycling code from Interpolated triangle
		if(state == INTERPOLATION || state == TEXTURIZED){
		std::vector<Image::sCelda> table;
		table.resize(this->window_height);
		for(int i = 0; i < table.size();i++){
			table[i].minx = 1000000;
			table[i].maxX = -1000000;
		}
		framebuffer.DDAwTable(coordinates.x,coordinates.y,coordinates2.x,coordinates2.y,table);
		framebuffer.DDAwTable(coordinates2.x,coordinates2.y,coordinates3.x,coordinates3.y,table);
		framebuffer.DDAwTable(coordinates.x,coordinates.y,coordinates3.x,coordinates3.y,table);
		for(int y = 0; y < window_height; y++){
			if(table[y].minx < table[y].maxX){
				for(int x = table[y].minx; x < table[y].maxX; x++){
					//Calculate barycentric interpolation of projected vertes to obtain u,v,w
					Vector3 barycentric = framebuffer.barycentricCoordinates(x,y,coordinates.x,coordinates.y,coordinates2.x,coordinates2.y,coordinates3.x,coordinates3.y,interpolate1,interpolate2,interpolate3);
					//Now we can find the value of z in the projected vertex
					float z_pixel = coordinates.z * barycentric.x + coordinates2.z * barycentric.y + coordinates3.z * barycentric.z;
					//Interpolate the color ans in lab2
					Color c = interpolate1 * barycentric.x + interpolate2 * barycentric.y + interpolate3 * barycentric.z;
					//Find interpolation of the texture with the interpolation coefficients and the coordinates of the texture
					Vector2 texture_interpolated = texcoord * barycentric.x + texcoord2 * barycentric.y + texcoord3 * barycentric.z;
					//Check if pixel is in the view
					/*if(pixel.z < 0 || pixel.x < 0 || pixel.x >= window_width || pixel.y < 0 || pixel.y >= window_height){
						continue;
					}
					float z_pixel = zbuffer.getPixel(pixel.x,pixel.y);*/
					//std::cout<< "La z de zbuffer original es: "<< z_pixel<<"\n";
					//std::cout<< "La z del pixel es: "<< pixel.z<<"\n";
					//std::cout<< zbuffer.getPixel(400,300);
					if(z_pixel <= zbuffer.getPixel(x,y)){
						zbuffer.setPixel(x,y,z_pixel);
						//std::cout<< "La nova z del zbuffer es: "<< zbuffer.getPixel(pixel.x,pixel.y);
						if(state == TEXTURIZED){
						framebuffer.setPixel(x,y,texture->getPixel(texture_interpolated.x,texture_interpolated.y));
						}
						else framebuffer.setPixel(x,y,c);
					} 
				}
			}
		}
		}

		else{
		Color c = Color::WHITE;
		framebuffer.drawLineBresenham(coordinates.x,coordinates.y,coordinates2.x,coordinates2.y,c);
		framebuffer.drawLineBresenham(coordinates.x,coordinates.y,coordinates3.x,coordinates3.y,c);
		framebuffer.drawLineBresenham(coordinates3.x,coordinates3.y,coordinates2.x,coordinates2.y,c);
	}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}


	
	//std::cout<<"Z in Zbuffer of pixel (x,y): (277,778) is z = "<<zbuffer.getPixel(277,778)<<"\n";
	/*for (int i = 0; i < window_height; i++)
	{
		for (int j = 0; j < window_width; j++)
		{
			std::cout<<"Z in Zbuffer of pixel (x,y): ("<<i<<","<<j<<") is z = "<<zbuffer.getPixel(i,j)<<"\n";
		}
		
	}*/
	
}

//called after render
void Application::update(double seconds_elapsed)
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	/*if (keystate[SDL_SCANCODE_SPACE])
	{

		state = INTERPOLATION;
		render(framebuffer);
	}*/

	//example to move eye
	if (keystate[SDL_SCANCODE_LEFT])
		camera->eye.x -= 1 * seconds_elapsed;
	if (keystate[SDL_SCANCODE_RIGHT])
		camera->eye.x += 1 * seconds_elapsed;
	if (keystate[SDL_SCANCODE_UP])
		camera->eye.y += 1 * seconds_elapsed;
	if (keystate[SDL_SCANCODE_DOWN])
		camera->eye.y -= 1 * seconds_elapsed;
	if (keystate[SDL_SCANCODE_A])
		camera->center.x -= 1 * seconds_elapsed;
	if (keystate[SDL_SCANCODE_D])
		camera->center.x += 1 * seconds_elapsed;
	if (keystate[SDL_SCANCODE_W])
		camera->center.y += 1 * seconds_elapsed;
	if (keystate[SDL_SCANCODE_S])
		camera->center.y -= 1 * seconds_elapsed;
	if (keystate[SDL_SCANCODE_F])
		camera->fov += 1 * seconds_elapsed;
	if (keystate[SDL_SCANCODE_G])
		camera->fov -= 1 * seconds_elapsed;

	//if we modify the camera fields, then update matrices
	camera->updateViewMatrix();
	camera->updateProjectionMatrix();
}

//keyboard press event 
void Application::onKeyDown( SDL_KeyboardEvent event )
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	switch (event.keysym.scancode)
	{
		case SDL_SCANCODE_1: state = EMPTY_MESH; break;
		case SDL_SCANCODE_2: state = INTERPOLATION; break;
		case SDL_SCANCODE_3: state = TEXTURIZED; break;
	}
	render(framebuffer);
}

//keyboard released event 
void Application::onKeyUp(SDL_KeyboardEvent event)
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	switch (event.keysym.scancode)
	{
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
