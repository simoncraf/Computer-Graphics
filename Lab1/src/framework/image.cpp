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

void Image::drawLine(int x1, int y1, int x2, int y2, Color color) {
	if (abs(x1-x2) <=1 && abs(y1 - y2) <= 1) {
		setPixelSafe(x1, y1, color);
		setPixelSafe(x2, y2, color);
	}
	else {
		int k = (x1 - x2) / 2;
		int k2 = (y1 - y2) / 2;
		drawLine(x1, y1, x2 + k, y2 + k2, color);
		drawLine(x1 - k, y1 - k2, x2, y2, color);
	}
}
void Image::drawRectangle(int x, int y, int w, int h, Color color, bool fill){
	if (fill == true){
		for (int i = x; i < (x + w)+1; ++i) {
			drawLine(i, y, i, y + h, color);
		}
	}
	else {
		drawLine(x, y, x+w, y, color);
		drawLine(x, y, x, y+h, color);
		drawLine(x+w, y+h, x, y + h, color);
		drawLine(x + w, y + h, x + w, y, color);
	}
}
/*void Image::drawCircle(int x, int y, int r, Color color, bool fill) {
	for (int i = x - r; i < x + r + 1; ++i) {
		for (int j = y - r; j < y + r + 1; ++j) {
			if (fill == false) {
				if (int(std::sqrt(pow(i - x, 2) + pow(j - y, 2))) == r) { setPixelSafe(i, j, color); }
			}
			else { if (int(std::sqrt(pow(i - x, 2) + pow(j - y, 2))) <= r) { setPixelSafe(i, j, color); } }
		}
	}
}*/
void Image::drawCircle(int xc, int yc, int x, int y, Color c)
{
    setPixelSafe(xc+x, yc+y, c);
    setPixelSafe(xc-x, yc+y, c);
    setPixelSafe(xc+x, yc-y, c);
    setPixelSafe(xc-x, yc-y, c);
    setPixelSafe(xc+y, yc+x, c);
    setPixelSafe(xc-y, yc+x, c);
    setPixelSafe(xc+y, yc-x, c);
    setPixelSafe(xc-y, yc-x, c);
}
void Image::gradientBtoR() {
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			float f = x / (float)width;
			f = f * 255;
			setPixelSafe(x, y, Color(f, 0, 255 - f));
		}
	}
}
void Image::chesslike(int square) {
	for (int x = 0; x < width; x++){
		for (int y = 0; y < height; y++){
			float f = ((x/square) % 2) ^ ((y/square) % 2);
			f = f * 255;
			setPixelSafe(x, y, Color(f, f, f));
		}
	}

}
void Image::invert() {
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			Color c = getPixel(x, y);
			setPixelSafe(x, y, Color(255 - c.r, 255 - c.g , 255 - c.b));
		}
	}
}
void Image::threshold() {
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			Color c = getPixel(x, y);
			if (c.r + c.b + c.g >= 383) { setPixelSafe(x, y, Color(255, 255, 255)); }
			else { setPixelSafe(x, y, Color(0, 0, 0)); }
		}
	}
}
Image Image::getArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
{
	Image result(width, height);
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
		{
			if( (x + start_x) < this->width && (y + start_y) < this->height) 
				result.setPixelSafe( x, y, getPixel(x + start_x,y + start_y) );
		}
	return result;
}

void Image::flipX()
{
	for(unsigned int x = 0; x < width * 0.5; ++x)
		for(unsigned int y = 0; y < height; ++y)
		{
			Color temp = getPixel(width - x - 1, y);
			setPixelSafe( width - x - 1, y, getPixel(x,y));
			setPixelSafe( x, y, temp );
		}
}

void Image::flipY()
{
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height * 0.5; ++y)
		{
			Color temp = getPixel(x, height - y - 1);
			setPixelSafe( x, height - y - 1, getPixel(x,y) );
			setPixelSafe( x, y, temp );
		}
}

int sgn(int x)
{
	if(x >= 0)
	{
		return 1;
	}
	return -1;
}

void Image::drawLineDDA(int x1, int y1 , int x2 , int y2 ,Color c) 
{
	float d, x, y;
	float dx = (x2-x1);
	float dy = (y2-y1);
	if(fabs(dx) >= fabs(dy))
		{
			d = fabs(dx);
		}
	else
		{
			d = fabs(dy);
		}
	float vx = dx / d;
	float vy = dy / d;
	x = x1 + sgn(x1) * 0.5;
	y = y1 + sgn(y1) * 0.5;
	for (int i = 0; i <= d; i++)
	{
		setPixelSafe(floor(x),floor(y), c);
		x = x + vx;
		y = y + vy;
	}
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

void Image::drawCircleBresenham(int xc, int yc, int r, Color c, bool fill)
{
	int x, y, v;
	x = 0;
	y = r;
	v = 1-r;
	setPixelSafe(xc,yc,c);

	while(y > x){
		if(v < 0){
			v = v + 2 * x + 3;
			x++;
		}
		else {
			v = v + 2 * (x - y) + 5;
			x++;
			y--;
		}
		drawCircle(xc,yc,x,y,c);

		if(fill){
			for(int i = -r; i <= r; i++){
				for(int k = -r; k <= r; k++){
					if(pow(k,2) + pow(i,2) <= pow(r,2)){
						setPixelSafe(xc + k, yc + i, c);
					}
				}
			}
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
		drawLineDDA(x1,y1,x2,y2,c);
		drawLineDDA(x2,y2,x3,y3,c);
		drawLineDDA(x1,y1,x3,y3,c);
	}
}

Color Image::barycentricCoordinates(int px, int py, int x1,int y1, int x2, int y2, int x3, int y3, Color c1, Color c2, Color c3)
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
	

	Color c = c1 * u + c2 * v + c3 * w;
	return c;
}

void Image::drawTriangleInterpolated(int x1,int y1, int x2, int y2, int x3, int y3, Color c1, Color c2, Color c3)
{
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
					Color c = barycentricCoordinates(k,i,x1,y1,x2,y2,x3,y3,c1,c2,c3);
					setPixelSafe(k,i,c);
				}
			}
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
			this->setPixelSafe(x , height - y - 1, Color( tgainfo->data[pos+2], tgainfo->data[pos+1], tgainfo->data[pos]) );
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

#ifndef IGNORE_LAMBDAS

//you can apply and algorithm for two images and store the result in the first one
//forEachPixel( img, img2, [](Color a, Color b) { return a + b; } );
template <typename F>
void forEachPixel(Image& img, const Image& img2, F f) {
	for(unsigned int pos = 0; pos < img.width * img.height; ++pos)
		img.pixels[pos] = f( img.pixels[pos], img2.pixels[pos] );
}

#endif