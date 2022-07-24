#include "image.h"


Image::Image() {
	width = 0; height = 0;
	pixels = NULL;
}

Image::Image(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new Color[width*height];
	memset(pixels, 0, width * height * sizeof(Color));
}

//copy constructor
Image::Image(const Image& c) {
	pixels = NULL;

	width = c.width;
	height = c.height;
	if(c.pixels)
	{
		pixels = new Color[width*height];
		memcpy(pixels, c.pixels, width*height*sizeof(Color));
	}
}

//assign operator
Image& Image::operator = (const Image& c)
{
	if(pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if(c.pixels)
	{
		pixels = new Color[width*height*sizeof(Color)];
		memcpy(pixels, c.pixels, width*height*sizeof(Color));
	}
	return *this;
}

Image::~Image()
{
	if(pixels) 
		delete pixels;
}



//change image size (the old one will remain in the top-left corner)
void Image::resize(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for(unsigned int x = 0; x < min_width; ++x)
		for(unsigned int y = 0; y < min_height; ++y)
			new_pixels[ y * width + x ] = getPixel(x,y);

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

//change image size and scale the content
void Image::scale(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];

	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
			new_pixels[ y * width + x ] = getPixel((unsigned int)(this->width * (x / (float)width)), (unsigned int)(this->height * (y / (float)height)) );

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

Image Image::getArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
{
	Image result(width, height);
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++x)
		{
			if( (x + start_x) < this->width && (y + start_y) < this->height) 
				result.setPixel( x, y, getPixel(x + start_x,y + start_y) );
		}
	return result;
}

void Image::flipX()
{
	for(unsigned int x = 0; x < width * 0.5; ++x)
		for(unsigned int y = 0; y < height; ++y)
		{
			Color temp = getPixel(width - x - 1, y);
			setPixel( width - x - 1, y, getPixel(x,y));
			setPixel( x, y, temp );
		}
}

void Image::flipY()
{
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height * 0.5; ++y)
		{
			Color temp = getPixel(x, height - y - 1);
			setPixel( x, height - y - 1, getPixel(x,y) );
			setPixel( x, y, temp );
		}
}


//Loads an image from a TGA file
bool Image::loadTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char TGAcompare[12];
	unsigned char header[6];
	unsigned int bytesPerPixel;
	unsigned int imageSize;

	FILE * file = fopen(filename, "rb");
   	if ( file == NULL || fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||
		fread(header, 1, sizeof(header), file) != sizeof(header))
	{
		std::cerr << "File not found: " << filename << std::endl;
		if (file == NULL)
			return NULL;
		else
		{
			fclose(file);
			return NULL;
		}
	}

	TGAInfo* tgainfo = new TGAInfo;
    
	tgainfo->width = header[1] * 256 + header[0];
	tgainfo->height = header[3] * 256 + header[2];
    
	if (tgainfo->width <= 0 || tgainfo->height <= 0 || (header[4] != 24 && header[4] != 32))
	{
		std::cerr << "TGA file seems to have errors or it is compressed, only uncompressed TGAs supported" << std::endl;
		fclose(file);
		delete tgainfo;
		return NULL;
	}
    
	tgainfo->bpp = header[4];
	bytesPerPixel = tgainfo->bpp / 8;
	imageSize = tgainfo->width * tgainfo->height * bytesPerPixel;
    
	tgainfo->data = new unsigned char[imageSize];
    
	if (tgainfo->data == NULL || fread(tgainfo->data, 1, imageSize, file) != imageSize)
	{
		if (tgainfo->data != NULL)
			delete tgainfo->data;
            
		fclose(file);
		delete tgainfo;
		return false;
	}

	fclose(file);

	//save info in image
	if(pixels)
		delete pixels;

	width = tgainfo->width;
	height = tgainfo->height;
	pixels = new Color[width*height];

	//convert to float all pixels
	for(unsigned int y = 0; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			unsigned int pos = y * width * bytesPerPixel + x * bytesPerPixel;
			this->setPixel(x , height - y - 1, Color( tgainfo->data[pos+2], tgainfo->data[pos+1], tgainfo->data[pos]) );
		}

	delete tgainfo->data;
	delete tgainfo;

	return true;
}

// Saves the image to a TGA file
bool Image::saveTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	FILE *file = fopen(filename, "wb");
	if ( file == NULL )
	{
		fclose(file);
		return false;
	}

	unsigned short header_short[3];
	header_short[0] = width;
	header_short[1] = height;
	unsigned char* header = (unsigned char*)header_short;
	header[4] = 24;
	header[5] = 0;

	//tgainfo->width = header[1] * 256 + header[0];
	//tgainfo->height = header[3] * 256 + header[2];

	fwrite(TGAheader, 1, sizeof(TGAheader), file);
	fwrite(header, 1, 6, file);

	//convert pixels to unsigned char
	unsigned char* bytes = new unsigned char[width*height*3];
	for(unsigned int y = 0; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			Color c = pixels[(height-y-1)*width+x];
			unsigned int pos = (y*width+x)*3;
			bytes[pos+2] = c.r;
			bytes[pos+1] = c.g;
			bytes[pos] = c.b;
		}

	fwrite(bytes, 1, width*height*3, file);
	fclose(file);
	return true;
}


FloatImage::FloatImage(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new float[width*height];
	memset(pixels, 0, width * height * sizeof(float));
}

//copy constructor
FloatImage::FloatImage(const FloatImage& c) {
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width*height];
		memcpy(pixels, c.pixels, width*height * sizeof(float));
	}
}

//assign operator
FloatImage& FloatImage::operator = (const FloatImage& c)
{
	if (pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width*height * sizeof(float)];
		memcpy(pixels, c.pixels, width*height * sizeof(float));
	}
	return *this;
}

FloatImage::~FloatImage()
{
	if (pixels)
		delete pixels;
}

int sgn(int x)
{
	if(x >= 0)
	{
		return 1;
	}
	return -1;
}

void Image::drawLineBresenham(int x1, int y1 , int x2 , int y2 ,Color c)
{
	int dx,dy,sx,sy,err,e2;
	dx = abs(x2-x1);
	sx = sgn(x2-x1); //To verify if x2>x1
	dy = -abs(y2-y1);
	sy = sgn(y2-y1); //To verify if y2>y1
	err = dx+dy;
	while(true){
		setPixelSafe(x1,y1,c);
		if(x1 == x2 && y1 == y2) {break;}
		e2 = err * 2;
		if(e2 >= dy){
			if(x1 == x2){break;}
			err += dy;
			x1 += sx;
		}
		if(e2 <= dx){
			if(y1 == y2){break;}
			err += dx;
			y1 += sy;
		}
	}
}

void Image::DDAwTable(int x1, int y1, int x2, int y2, std::vector<sCelda> &table)
{
	float d, x, y;
	float dx = (x2-x1);
	float dy = (y2-y1);
	if(fabs(dx) >= fabs(dy)){
		d = fabs(dx);
	} else{
		d = fabs(dy);
	}
	float vx = dx / d;
	float vy = dy / d;
	x = x1 + sgn(x1) * 0.5;
	y = y1 + sgn(y1) * 0.5;

	for(int i = 0; i <= d; i++)
	{
		if ((int)floor(x) < table[(int)floor(y)].minx)
		{
			table[(int)floor(y)].minx = (int)floor(x);
		}
		if (table[(int)floor(y)].maxX < (int)floor(x))
		{
			table[(int)floor(y)].maxX = (int)floor(x);
		}

		x = vx + x;
		y = vy + y;
	}
}

void Image::drawTriangle(int x1,int y1, int x2, int y2, int x3, int y3, Color c, bool fill)
{
	if(fill){
		std::vector<sCelda> table;
		table.resize(this->height);

		for(int i = 0; i < table.size();i++){
			table[i].minx = 1000000;
			table[i].maxX = -1000000;
		}
		DDAwTable(x1,y1,x2,y2,table);
		DDAwTable(x2,y2,x3,y3,table);
		DDAwTable(x1,y1,x3,y3,table);
		for(int i = 0; i < height; i++){
			if(table[i].minx < table[i].maxX){
				for(int k = table[i].minx; k < table[i].maxX; k++){
					setPixelSafe(k,i,c);
				}
			}
		}
	}

	else{
		drawLineBresenham(x1,y1,x2,y2,c);
		drawLineBresenham(x2,y2,x3,y3,c);
		drawLineBresenham(x1,y1,x3,y3,c);
	}
}

Vector3 Image::barycentricCoordinates(int px, int py, int x1,int y1, int x2, int y2, int x3, int y3, Color c1, Color c2, Color c3)
{
	Vector2 v1,v2,v3;
	float d00, d01, d11, d20, d21, denominador, w, u, v;

	v1 = Vector2(x2-x1,y2-y1);
	v2 = Vector2(x3-x1,y3-y1);
	v3 = Vector2(px-x1,py-y1);

	d00 = v1.dot(v1);
	d01 = v1.dot(v2);
	d11 = v2.dot(v2);
	d20 = v3.dot(v1);
	d21 = v3.dot(v2);
	denominador = ((d00 * d11)- pow(d01,2));
	v = (d11 * d20 - d01 * d21) / denominador;
	w = (d00 * d21 - d01 * d20) / denominador;
	u = 1.0 - v - w;
	
	return Vector3 (u,v,w);
}

void Image::drawTriangleInterpolated(Vector3 v1, Vector3 v2, Vector3 v3, Color c1, Color c2, Color c3, FloatImage zbf)
{
	Vector3 pixel;
	int counter = 0;
	std::vector<sCelda> table;
		table.resize(this->height);

		for(int i = 0; i < table.size();i++){
			table[i].minx = 1000000;
			table[i].maxX = -1000000;
		}
		DDAwTable(v1.x,v1.y,v2.x,v2.y,table);
		DDAwTable(v2.x,v2.y,v3.x,v3.y,table);
		DDAwTable(v1.x,v1.y,v3.x,v3.y,table);
		for(int y = 0; y < height; y++){
			if(table[y].minx < table[y].maxX){
				for(int x = table[y].minx; x < table[y].maxX; x++){
					pixel.set(x,y,0);
					Vector3 barycentric = barycentricCoordinates(pixel.x,pixel.y,v1.x,v1.y,v2.x,v2.y,v3.x,v3.y,c1,c2,c3);
					pixel.z = v1.z * barycentric.x + v2.z * barycentric.y + v3.z * barycentric.z;
					Color c = c1 * barycentric.x + c2 * barycentric.y + c3 * barycentric.z;
					//Check if pixel is in the view
					if(pixel.z < 0 || pixel.x < 0 || pixel.x >= width || pixel.y < 0 || pixel.y >= height){
						continue;
					}

					float z_pixel = zbf.getPixel(pixel.x,pixel.y);
					//std::cout<< "La z de zbuffer original es: "<< z_pixel<<"\n";
					//std::cout<< "La z del pixel es: "<< pixel.z<<"\n";
					//std::cout<< zbf.getPixel(400,300);
					if(pixel.z < zbf.getPixel(pixel.x,pixel.y)){
						zbf.setPixel(pixel.x,pixel.y,pixel.z);
						//std::cout<< "La nova z del zbuffer es: "<< zbf.getPixel(pixel.x,pixel.y);
						counter++;
						setPixel(pixel.x,pixel.y,c);
					} else{
						//std::cout<< "El pixel esta per sota de l anterior"<<"x: " <<pixel.x<<" y: "<<pixel.y;
					}
				}
			}
		}
		//std::cout<<"Pixels pintats: "<<counter;
}

//change image size (the old one will remain in the top-left corner)
void FloatImage::resize(unsigned int width, unsigned int height)
{
	float* new_pixels = new float[width*height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for (unsigned int x = 0; x < min_width; ++x)
		for (unsigned int y = 0; y < min_height; ++y)
			new_pixels[y * width + x] = getPixel(x, y);

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}


#ifndef IGNORE_LAMBDAS

//you can apply and algorithm for two images and store the result in the first one
//forEachPixel( img, img2, [](Color a, Color b) { return a + b; } );
template <typename F>
void forEachPixel(Image& img, const Image& img2, F f) {
	for(unsigned int pos = 0; pos < img.width * img.height; ++pos)
		img.pixels[pos] = f( img.pixels[pos], img2.pixels[pos] );
}

#endif