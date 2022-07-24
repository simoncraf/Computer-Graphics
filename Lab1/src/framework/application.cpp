#define NOMINMAX
#include "application.h"
#include "utils.h"
#include "image.h"

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

	framebuffer.resize(w, h);
	state = DDA;
	triangle_state = third;
	framebuffer.drawRectangle(0,595,800,5,painting,true);
	interpolate1 = Color::RED;
	interpolate2 = Color::GREEN;
	interpolate3 = Color::BLUE;
	interpolation = RGB;
	
}

//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;
}

//render one frame
void Application::render( Image& framebuffer )
{
	
}

//called after render
void Application::update(double seconds_elapsed)
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	if (keystate[SDL_SCANCODE_SPACE]) //if key space is pressed
	{
		//...
		
	}
	

	//to read mouse position use mouse_position
}

//keyboard press event 
void Application::onKeyDown( SDL_KeyboardEvent event )
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	switch(event.keysym.scancode)
	{
		case SDL_SCANCODE_1:
			state = DDA;
			break;
		
		case SDL_SCANCODE_2:
			state = Bresenham;
			break;

		case SDL_SCANCODE_3:
			state = Circles;
			break;

		case SDL_SCANCODE_4:
			state = Triangles;
			break;

		case SDL_SCANCODE_5:
			state = Triangles_Interpolated;
			break;

		case SDL_SCANCODE_SPACE:
			if(painting_color == WHITE){
				painting_color = RED;
				painting = Color::RED;
				framebuffer.drawRectangle(0,595,800,5,painting,true);
			}
			else if(painting_color == RED){
				painting_color = GREEN;
				painting = Color::GREEN;
				framebuffer.drawRectangle(0,595,800,5,painting,true);
			}
			else if(painting_color == GREEN){
				painting_color = CYAN;
				painting = Color::CYAN;
				framebuffer.drawRectangle(0,595,800,5,painting,true);
			}
			else if(painting_color == CYAN){
				painting_color = WHITE;
				painting = Color::WHITE;
				framebuffer.drawRectangle(0,595,800,5,painting,true);
			}
			break;

		case SDL_SCANCODE_I:
			if(interpolation == RGB){
				interpolation = GYP;
				interpolate1 = Color::CYAN;
				interpolate2 = Color::YELLOW;
				interpolate3 = Color::PURPLE;
			}
			else if(interpolation == GYP){
				interpolation = APB;
				interpolate1 = Color::GREEN;
				interpolate2 = Color::YELLOW;
				interpolate3 = Color::PURPLE;
			} 
			else if(interpolation == APB){
				interpolation = RGB;
				interpolate1 = Color::RED;
				interpolate2 = Color::GREEN;
				interpolate3 = Color::BLUE;
			}
			break;

		case SDL_SCANCODE_F:
			if(state == Circles){
				if(fill){
				framebuffer.drawCircleBresenham(circle_center.x,circle_center.y,radius,Color::BLACK,fill);
				fill = false;

			} else{
				fill = true;
			}
				framebuffer.drawCircleBresenham(circle_center.x,circle_center.y,radius,painting,fill);
			}

			if(state == Triangles){
				if(fill){
				framebuffer.drawTriangle(vertex_triangle.x,vertex_triangle.y,vertex_triangle2.x,vertex_triangle2.y,vertex_triangle3.x,vertex_triangle3.y, Color::BLACK, fill);
				fill = false;

			} else{
				fill = true;
			}
			framebuffer.drawTriangle(vertex_triangle.x,vertex_triangle.y,vertex_triangle2.x,vertex_triangle2.y,vertex_triangle3.x,vertex_triangle3.y, painting, fill);	
			}
			
			break;

		case SDL_SCANCODE_R:
			framebuffer.fill(Color::BLACK);
			break;

		case SDL_SCANCODE_D:
			if(state == DDA){
				framebuffer.drawLineDDA(vertex_line1.x,vertex_line1.y,vertex_line2.x,vertex_line2.y,background);
			}
			else if (state == Bresenham)
			{
				framebuffer.drawLineBresenham(vertex_line1.x,vertex_line1.y,vertex_line2.x,vertex_line2.y,background);
			}
			else if(state == Circles)
			{
				framebuffer.drawCircleBresenham(circle_center.x,circle_center.y,radius,background,fill);
			}
			else if(state == Triangles){
				framebuffer.drawTriangle(vertex_triangle.x,vertex_triangle.y,vertex_triangle2.x,vertex_triangle2.y,vertex_triangle3.x,vertex_triangle3.y, background, fill);
			}
			else
			{
				framebuffer.drawTriangle(vertex_triangle.x,vertex_triangle.y,vertex_triangle2.x,vertex_triangle2.y,vertex_triangle3.x,vertex_triangle3.y, background, fill);
			}
			break;

		case SDL_SCANCODE_ESCAPE:
			exit(0); 
			break; //ESC key, kill the app
	}
}

//keyboard key up event 
void Application::onKeyUp(SDL_KeyboardEvent event)
{
	//...
}

//mouse button event
void Application::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse pressed
	{
		if(state == DDA)
		{
			if (line_completed == true)
		{
			vertex_line1 = mouse_position;
			framebuffer.setPixel(vertex_line1.x,vertex_line1.y,painting);
			line_completed = false;
		}
		else {
			vertex_line2 = mouse_position;
			framebuffer.setPixel(vertex_line2.x,vertex_line2.y,painting);
			framebuffer.drawLineDDA(vertex_line1.x,vertex_line1.y,vertex_line2.x,vertex_line2.y,painting);
			line_completed = true;
		}
		}

		if(state == Bresenham)
		{
			if (line_completed == true)
			{
			vertex_line1 = mouse_position;
			framebuffer.setPixel(vertex_line1.x,vertex_line1.y,painting);
			line_completed = false;
			}
		else {
			vertex_line2 = mouse_position;
			framebuffer.setPixel(vertex_line2.x,vertex_line2.y,painting);
			framebuffer.drawLineBresenham(vertex_line1.x,vertex_line1.y,vertex_line2.x,vertex_line2.y,painting);
			line_completed = true;
		}
		}

		if(state == Circles)
		{
			if (circle_completed == true)
		{
			circle_center = mouse_position;
			framebuffer.setPixel(circle_center.x,circle_center.y,painting);
			circle_completed = false;
		}
		else {
			circle_radius = mouse_position;
			framebuffer.setPixel(circle_radius.x,circle_radius.y,painting);
			int radius_x = pow(abs(int(circle_radius.x) - int(circle_center.x)),2);
			int radius_y = pow(abs(int(circle_radius.y) - int(circle_center.y)),2);
			radius = sqrt(radius_x + radius_y);
			std::cout << "circle radius x: " << circle_radius.x <<"; circle center x: " << circle_center.x << "; circle radius y: " << circle_radius.y << "; circle center y: " << circle_center.y << " radius: " << radius << "\n";
			//int radius = 50;
			framebuffer.drawCircleBresenham(circle_center.x,circle_center.y,radius,painting,fill);
			circle_completed = true;
		}

		}
		if(state == Triangles)
		{
			if (triangle_state == third)
			{
			vertex_triangle = mouse_position;
			framebuffer.setPixel(vertex_triangle.x,vertex_triangle.y,painting);
			triangle_state = first;
			}

			else if(triangle_state == first){
			vertex_triangle2 = mouse_position;
			framebuffer.setPixel(vertex_triangle2.x,vertex_triangle2.y,painting);
			triangle_state = second;
			}

		else {
			vertex_triangle3 = mouse_position;
			framebuffer.setPixel(vertex_triangle3.x,vertex_triangle3.y,painting);
			framebuffer.drawTriangle(vertex_triangle.x,vertex_triangle.y,vertex_triangle2.x,vertex_triangle2.y,vertex_triangle3.x,vertex_triangle3.y, painting, fill);
			std::cout << vertex_triangle.x <<" "<<vertex_triangle2.x<<" "<<vertex_triangle3.x<<"\n";
			triangle_state = third;
		}
		}
		if(state == Triangles_Interpolated)
		{
			if (triangle_state == third)
			{
			vertex_triangle = mouse_position;
			framebuffer.setPixel(vertex_triangle.x,vertex_triangle.y,painting);
			triangle_state = first;
			}

			else if(triangle_state == first){
			vertex_triangle2 = mouse_position;
			framebuffer.setPixel(vertex_triangle2.x,vertex_triangle2.y,painting);
			triangle_state = second;
			}

		else {
			vertex_triangle3 = mouse_position;
			framebuffer.setPixel(vertex_triangle3.x,vertex_triangle3.y,painting);
			framebuffer.drawTriangleInterpolated(vertex_triangle.x,vertex_triangle.y,vertex_triangle2.x,vertex_triangle2.y,vertex_triangle3.x,vertex_triangle3.y, interpolate1, interpolate2, interpolate3);
			std::cout << vertex_triangle.x <<" "<<vertex_triangle2.x<<" "<<vertex_triangle3.x<<"\n";
			triangle_state = third;
		}
		}
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
void Application::rotate(Image& framebuffer, int deg) {
	Image rotation;
	float angle = (deg * M_PI) / 180;
	int max_x = 0;
	int min_x = 4 * std::max(framebuffer.width, framebuffer.height);
	int max_y = 0;
	int min_y = 4 * std::max(framebuffer.width, framebuffer.height);
	rotation.resize(4 * std::max(framebuffer.width, framebuffer.height), 4 * std::max(framebuffer.width, framebuffer.height));
	for (int x = 0; x < framebuffer.width; ++x) {
		for (int y = 0; y < framebuffer.height; ++y) {
			float x2 = (x * cos(angle)) - (y * sin(angle));
			float y2 = (x * sin(angle)) + (y * cos(angle));
			Color col = framebuffer.getPixel(x, y);
			x2 += (rotation.width) / 2;
			y2 += (rotation.width) / 2;
			//if (x2 - int(x2) >= 0.5) { ++x2; }
			//if (y2 - int(y2) >= 0.5) { ++y2; }
			if ((col.r == 0) && (col.g == 0) && (col.b == 0)){}
			else {
				if (int(x2) > max_x) { max_x = int(x2); }
				if (int(y2) > max_y) { max_y = int(y2); }
				if (int(x2) < min_x) { min_x = int(x2); }
				if (int(y2) < min_y) { min_y = int(y2); }
			}
			rotation.setPixel(int(x2), int(y2), col);
		}
	}
	for (int x = min_x; x < max_x+1; ++x) {
		for (int y = min_y; y < max_y+1; ++y) {
			Color ch = rotation.getPixel(x, y);
			rotation.setPixel(x, y, Color::BLACK);
			rotation.setPixel(x - min_x, y - min_y, ch);
		}

	}
	rotation.resize(max_x - min_x + 1, max_y - min_y + 1);
	framebuffer = rotation;
}