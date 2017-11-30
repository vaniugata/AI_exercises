#include <SDL2/SDL.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Perceptron.h"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 800;
static double SCALE = .1f;
constexpr double LEARNING_RATE = .1f;
constexpr unsigned POINTS_CNT = 320;

SDL_Renderer* renderer = nullptr;
SDL_Window* window = nullptr;

void DrawLine( std::vector<double> weights );

double CalcCoordinate(std::vector<double> weights, double x);

double ScaleCoordiate(double, double, double, double, double);

void Bresenham_line(int x1, int y1, int x2, int y2, bool dotted = false);

void DrawCircle(int x, int y, int r);

void UpdateScreen();

int main(){
	// Init SDL
	window = SDL_CreateWindow("Ai", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, 1, SDL_RENDERER_PRESENTVSYNC);
	
	std::srand( std::time(0) );

	SDL_Event* e = new SDL_Event;
	bool q = false;	

	// Init training samples 
	std::vector<TraningSample> ts0;
	ts0.push_back(TraningSample(0, 0, false));
	ts0.push_back(TraningSample(0, 1, false));
	ts0.push_back(TraningSample(1, 0, false));
	ts0.push_back(TraningSample(1, 1, !false));

	std::vector<TraningSample> ts1;
	for(unsigned i = 0; i < POINTS_CNT; i++){
		float x = (rand() % 100) / 100.0f;
		float y = (rand() % 100) / 100.0f ;
		ts1.push_back( TraningSample(x, y,( x > y ) ) );
	}

	std::vector<TraningSample>* ptrTs = &ts0;

	Perceptron* p = new Perceptron;

	unsigned time = SDL_GetTicks();
	unsigned period = 50;

	while(!q) {
		while(SDL_PollEvent(e)) {
			if( e->type == SDL_QUIT ) q = true;
			
			switch(e->key.keysym.sym){
				case SDLK_q : q = true; break;
				case SDLK_1 : 
					ptrTs = &ts0;
					p->Reset();		  
				break;
				case SDLK_2 : 
					ptrTs = &ts1;
					p->Reset();
					break;
				case SDLK_a : SCALE += 0.05; break;
				case SDLK_d : SCALE -= 0.05; break; 
			}
		}
		if( time + period < SDL_GetTicks()){
				p->Train(*ptrTs, 1, LEARNING_RATE);
				time = SDL_GetTicks();
		}
		
		int offset_x = ( SCREEN_WIDTH - SCREEN_WIDTH*SCALE ) / 2;
		int offset_y = ( SCREEN_HEIGHT- SCREEN_HEIGHT*SCALE ) / 2;
		int r = 10;
		for(unsigned i = 0; i < ptrTs->size(); ++i){
				int x = SCALE * (int)ScaleCoordiate(ptrTs->at(i).samples[1], 0 , 1, 0, SCREEN_WIDTH) + offset_x;
				int y = SCALE * (int)ScaleCoordiate(ptrTs->at(i).samples[2], 0 , 1, 0, SCREEN_HEIGHT) + offset_y;
				if( ptrTs->at(i).correct_output ){
					SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
				} else {
					SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
				}
				DrawCircle( x, (SCREEN_HEIGHT-y), r*SCALE );
		}
		DrawLine( p->GetWeights() );
		SDL_SetRenderDrawColor(renderer,255,255,255,255);
		Bresenham_line( offset_x, 0, offset_x, SCREEN_HEIGHT );
		Bresenham_line( 0,SCREEN_HEIGHT - offset_y, SCREEN_WIDTH, SCREEN_HEIGHT-offset_y );

		UpdateScreen();
	}
	delete p;
	delete e;
	return 0;
}

void DrawLine(std::vector<double> weights){

	double offset_x = ( SCREEN_WIDTH - SCREEN_WIDTH*SCALE ) / 2;
	double offset_y = ( SCREEN_HEIGHT - SCREEN_HEIGHT*SCALE ) / 2;
	
	double raw_x1 = -offset_x / SCREEN_WIDTH;
	double raw_x2 = 1 + offset_x / SCREEN_WIDTH;
	raw_x1 = -offset_x; raw_x2 = SCREEN_WIDTH;

	double raw_y1 =  CalcCoordinate( weights, raw_x1 );
	double raw_y2 =  CalcCoordinate( weights, raw_x2 );

	int y1 = (int) ( ScaleCoordiate(raw_y1, 0, 1, 0, SCREEN_HEIGHT*SCALE) ) + offset_y;
	int y2 = (int) ( ScaleCoordiate(raw_y2, 0, 1, 0, SCREEN_HEIGHT*SCALE) ) + offset_y;

	int x1 = (int) ScaleCoordiate(raw_x1, 0, 1, 0, SCREEN_WIDTH*SCALE) + offset_x;
	int x2 = (int) ScaleCoordiate(raw_x2, 0, 1, 0, SCREEN_WIDTH*SCALE) + offset_x;
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); 
	//Bresenham_line(x1, SCREEN_HEIGHT-y1, x2 ,SCREEN_HEIGHT-y2);
	SDL_RenderDrawLine(renderer, x1, SCREEN_HEIGHT-y1, x2, SCREEN_HEIGHT-y2);
	std::cerr << x1 << " " << y1 << " " << x2 << " " << y2 << "\n";
}

double CalcCoordinate(std::vector<double> weights, double x){
	if(weights.empty()) return (-1);
	return ( (-weights[1]*x - weights[0]) / weights[2] );
}

double ScaleCoordiate(double value, double min_in, double max_in, double min_out, double max_out){
	double x = value / std::abs( max_in - min_in);
	double result = min_out + std::abs(max_out - min_out) * x;

	return result;
}

void DrawCircle(int x, int y, int r){
	for(double a = 0; a < 2 * M_PI; a += 0.1){
	   SDL_RenderDrawPoint(renderer, x + r * std::cos(a), y +  r * std::sin(a) );
	   SDL_RenderDrawPoint(renderer, x , y);
	}
}

void Bresenham_line(int x1, int y1, int x2, int y2, bool dotted){
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
			if( i%2 ) SDL_RenderDrawPoint( renderer, x1, y1 );
		} else {
			SDL_RenderDrawPoint( renderer, x1, y1 );
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

void UpdateScreen(){
	SDL_RenderPresent(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
}
