/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#ifndef APPLICATION_H
#define APPLICATION_H

#include "includes.h"
#include "framework.h"
#include "image.h"

class Application
{
public:
	//window
	SDL_Window* window;
	float window_width;
	float window_height;
	Image fruit;
	Image toolbar;
	Image framebuffer;
	enum states {
		DDA,
		Bresenham,
		Circles,
		Triangles,
		Triangles_Interpolated
	} state;
	float time;
	Color background = Color::BLACK;
	Color painting = Color::WHITE;
	enum painting_colors{
		WHITE,
		RED,
		GREEN,
		CYAN,
	}painting_color;

	enum interpolations{
		RGB,
		GYP,
		APB
	} interpolation;
	Color interpolate1;
	Color interpolate2;
	Color interpolate3;
	Vector2 vertex_line1 = Vector2(0,0);
	Vector2 vertex_line2 = Vector2(0,0);
	bool line_completed = true;
	Vector2 circle_center = Vector2(0,0);
	Vector2 circle_radius = Vector2(0,0);
	int radius;
	bool circle_completed = true;
	bool fill = true;
	Vector2 vertex_triangle;
	Vector2 vertex_triangle2;
	Vector2 vertex_triangle3;
	enum traingle_states{
		first,
		second,
		third
	}triangle_state;
	//keyboard state
	const Uint8* keystate;

	//mouse state
	int mouse_state; //tells which buttons are pressed
	Vector2 mouse_position; //last mouse position
	Vector2 mouse_delta; //mouse movement in the last frame
	Color paint_color;

	//constructor
	Application(const char* caption, int width, int height);

	//main methods
	void init( void );
	void render( Image& framebuffer );
	void update( double dt );

	//methods for events
	void onKeyDown( SDL_KeyboardEvent event );
	void onKeyUp(SDL_KeyboardEvent event);
	void onMouseButtonDown( SDL_MouseButtonEvent event );
	void onMouseButtonUp( SDL_MouseButtonEvent event );

	//other methods to control the app
	void setWindowSize(int width, int height) {
		glViewport( 0,0, width, height );
		this->window_width = width;
		this->window_height = height;
		framebuffer.resize(width,height);
	}

	Vector2 getWindowSize()
	{
		int w,h;
		SDL_GetWindowSize(window,&w,&h);
		return Vector2(w,h);
	}

	void start();
	void rotate(Image& f,int deg);
	void draw(Image& framebuffer);
};


#endif 