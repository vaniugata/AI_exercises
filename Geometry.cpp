#include "Geometry.h"
#include <iostream>

Geometry::Geometry() : 
	_scale(0.5f)
{

}

Geometry::~Geometry() {

}

void Geometry::InitSDL() {
	_window = SDL_CreateWindow("Ai", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if( !_window ) {
		std::cerr << SDL_GetError() <<"\n";
		return;
	}
	_renderer = SDL_CreateRenderer(_window, 1, SDL_RENDERER_PRESENTVSYNC);
	if( !_renderer ){
		std::cerr << SDL_GetError() << "\n";
	}
}

double Geometry::CalcCoordinate(std::vector<double> weights, double x) {
	if(weights.empty()) return (-1);
	return ( (-weights[1]*x - weights[0]) / weights[2] );
}

double Geometry::ScaleCoordiate( double value, double min_in, double min_out, double max_in, double max_out ) {
	return ((value/std::abs(min_out-min_in) - min_in)*std::abs(max_out-max_in) + max_in);
}

void Geometry::Bresenham_line(int x1, int y1, int x2, int y2, bool dotted) {
	int delta_x = x2 - x1;
	int delta_y = y2 - y1;	
	
	int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;
	if(delta_x > 0) dx1 = dx2 = 1; else if(delta_x < 0) dx1 = dx2 = (-1);
	if(delta_y > 0) dy1 = 1; else if(delta_y < 0) dy1 = (-1);

	int greater_dist = std::abs( delta_x );
	int shorter_dist = std::abs( delta_y );
	if(greater_dist <= shorter_dist){
		std::swap( greater_dist, shorter_dist );
		dx2 = 0;
		if(delta_y > 0) dy2 = 1; else if(delta_y < 0) dy2 = (-1);
	}

	int numerator = greater_dist / 2; 
	for(int i = 0; i <= greater_dist; i++){
		if( dotted ){
			if( i%2 ) SDL_RenderDrawPoint( _renderer, x1, y1 );
		} else {
			SDL_RenderDrawPoint( _renderer, x1, y1 );
		}
		numerator += shorter_dist;
		// Not steep slope (octants: 2, 3, 6, 7)
		if( numerator >= greater_dist ){
			numerator -= greater_dist;
			x1 += dx1;
			y1 += dy1;
		// Steep slope (octants: 1, 4, 5, 8)
		} else {
			x1 += dx2;
			y1 += dy2;
		}
	}
}

void Geometry::DrawLine( std::vector<double> weights ) {
	double offset_x = ( SCREEN_WIDTH - SCREEN_WIDTH*_scale ) / 2;
	double offset_y = ( SCREEN_HEIGHT - SCREEN_HEIGHT*_scale ) / 2;
	
	double raw_x1 = -offset_x / SCREEN_WIDTH;
	double raw_x2 = 1 + offset_x / SCREEN_WIDTH;
	raw_x1 = -offset_x; raw_x2 = SCREEN_WIDTH;

	double raw_y1 =  CalcCoordinate( weights, raw_x1 );
	double raw_y2 =  CalcCoordinate( weights, raw_x2 );

	int y1 = (int) ( ScaleCoordiate(raw_y1, 0, 1, 0, SCREEN_HEIGHT*_scale) ) + offset_y;
	int y2 = (int) ( ScaleCoordiate(raw_y2, 0, 1, 0, SCREEN_HEIGHT*_scale) ) + offset_y;

	int x1 = (int) ScaleCoordiate(raw_x1, 0, 1, 0, SCREEN_WIDTH*_scale) + offset_x;
	int x2 = (int) ScaleCoordiate(raw_x2, 0, 1, 0, SCREEN_WIDTH*_scale) + offset_x;
	
	SDL_SetRenderDrawColor(_renderer, 255, 255, 0, 255); 
	//Bresenham_line(x1, SCREEN_HEIGHT-y1, x2 ,SCREEN_HEIGHT-y2);
	SDL_RenderDrawLine(_renderer, x1, SCREEN_HEIGHT-y1, x2, SCREEN_HEIGHT-y2);
}

void Geometry::DrawCircle(int x, int y, int r){
	for(double a = 0; a < 2 * M_PI; a += 0.1){
	   SDL_RenderDrawPoint( _renderer, x + r * std::cos(a), y +  r * std::sin(a) );
	   SDL_RenderDrawPoint( _renderer, x , y);
	}
}

void Geometry::UpdateScreen(){
	SDL_RenderPresent(_renderer);
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
	SDL_RenderClear(_renderer);
}

void Geometry::DrawSamples( std::vector<TraningSample>& ts, Perceptron* p  ){
	int offset_x = ( SCREEN_WIDTH - SCREEN_WIDTH*_scale ) / 2;
	int offset_y = ( SCREEN_HEIGHT- SCREEN_HEIGHT*_scale ) / 2;
	int r = 10;
	for(unsigned i = 0; i < ts.size(); ++i){
			int x = _scale * ScaleCoordiate((double)ts[i].samples[1], 0.0f, 1.0f, 0.0f, (double)SCREEN_WIDTH) + offset_x;
			int y = _scale * ScaleCoordiate((double)ts[i].samples[2], 0.0f, 1.0f, 0.0f, (double)SCREEN_HEIGHT) + offset_y;
			std::cout << ts[i].samples[1]<< " " << y << std::endl;
			if( ts[i].correct_output ){
				SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);
			} else {
				SDL_SetRenderDrawColor(_renderer, 0, 255, 0, 255);
			}
			DrawCircle( x, (SCREEN_HEIGHT-y), r*_scale );
	}
	//show coord sys axes
	SDL_SetRenderDrawColor( _renderer, 255, 255, 255, 255 );
	Bresenham_line( offset_x, 0, offset_x, SCREEN_HEIGHT );
	Bresenham_line( 0, SCREEN_HEIGHT - offset_y, SCREEN_WIDTH, SCREEN_HEIGHT-offset_y );
}

void Geometry::scale(double scale){
	this->_scale += scale;
}
