#include <SDL2/SDL.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Perceptron.h"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 800;
constexpr double SCALE = 0.3f;
constexpr double LEARNING_RATE = 0.1f;
constexpr unsigned POINTS_CNT = 100;

SDL_Renderer* renderer = nullptr;
SDL_Window* window = nullptr;

enum Pane { pane_1, pane_2 };

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
	Pane pane = pane_1;

	// Init training samples 
	std::vector<TraningSample> ts;
	ts.push_back(TraningSample(0, 0, false));
	ts.push_back(TraningSample(0, 1, false));
	ts.push_back(TraningSample(1, 0, false));
	ts.push_back(TraningSample(1, 1, !false));

	std::vector<std::pair<SDL_Point, bool> > training_set;

	for(unsigned i = 0; i < POINTS_CNT; i++){
		SDL_Point pt;
		pt.x = rand() % SCREEN_WIDTH;
		pt.y = rand() % SCREEN_HEIGHT;
		bool is_above_the_line = pt.x > pt.y;
		training_set.push_back( std::pair<SDL_Point, bool>( pt, is_above_the_line ) );		
	}


	Perceptron* p = new Perceptron;

	unsigned time = SDL_GetTicks();
	unsigned period = 50;

	std::vector<int> input;
	p->InitWeights(3);
	input.clear();
	unsigned i = 0;

	while(!q) {
		while(SDL_PollEvent(e)) {
			if( e->type == SDL_QUIT ) q = true;
			
			switch(e->key.keysym.sym){
				case SDLK_q : q = true; break;
				case SDLK_1 : pane = pane_1; break;
				case SDLK_2 : pane = pane_2; break;
			}
		}

		switch( pane ){
			case pane_1 :
				if( time + period < SDL_GetTicks()){
					p->Train(ts, 1, LEARNING_RATE);
					time = SDL_GetTicks();
				}
				DrawLine( p->GetWeights() );

				for(unsigned i = 0; i < ts.size(); ++i){
						int x = (int)ScaleCoordiate(ts[i].samples[1], 0 , 1, 0, SCREEN_WIDTH);
						int y = (int)ScaleCoordiate(ts[i].samples[2], 0 , 1, 0, SCREEN_HEIGHT);
						SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
						DrawCircle( x, y, 10 );
					}
			break;
			case pane_2 :
				if( time + period < SDL_GetTicks() ){
					if( i >= training_set.size()-1 ) i = 0;
					time = SDL_GetTicks();
					input.push_back(1);
					input.push_back( training_set[i].first.x );
					input.push_back( training_set[i].first.y );
					p->Train( input, training_set[i].second, LEARNING_RATE );
					input.clear();
					i++;
				}
				double x1_val = (-1);
				double x2_val = 1;
				double y1_val = CalcCoordinate( p->GetWeights(), x1_val );
				double y2_val = CalcCoordinate( p->GetWeights(), x2_val );

				int x1 = (int) ScaleCoordiate(x1_val, 0, 1, 0, SCREEN_WIDTH) + SCREEN_WIDTH/2;
				int x2 = (int) ScaleCoordiate(x2_val, 0, 1, 0, SCREEN_WIDTH) + SCREEN_WIDTH/2;
				int y1 = (int) ScaleCoordiate(y1_val, 0, 1, 0, SCREEN_WIDTH) + SCREEN_HEIGHT/2;
				int y2 = (int) ScaleCoordiate(y2_val, 0, 1, 0, SCREEN_WIDTH) + SCREEN_HEIGHT/2;

				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				Bresenham_line(x1, y1, x2, y2);
		
				unsigned time2 = SDL_GetTicks();
				if( time2 + period < SDL_GetTicks() ) {
					std::cerr<<"x1:"<<x1<<" y1:"<<y1<<" x2:"<<x2<<" y2:"<<y2<<"\n";
					time = SDL_GetTicks();
				}
				// Draw samples
				for(auto i : training_set){
					if( i.second ){
						SDL_SetRenderDrawColor(renderer, 100, 255, 100, 255);
					} else {
						SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
					}
					DrawCircle(i.first.x, i.first.y, 5);
				}
			
				SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255);
				Bresenham_line(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, true);
			break;
		}
		UpdateScreen();
	}
	delete p;
	delete e;
	return 0;
}

void DrawLine(std::vector<double> weights){
	//double raw_x1 = -SCREEN_WIDTH * SCALE, raw_x2 = SCREEN_WIDTH - SCREEN_WIDTH * SCALE;
	double raw_x1 = 0, raw_x2 = 1;
	
	double raw_y1 = SCALE * CalcCoordinate( weights, raw_x1 );
	double raw_y2 = SCALE * CalcCoordinate( weights, raw_x2 );

	int y1 = (int) ( SCREEN_HEIGHT - ScaleCoordiate(raw_y1, 0, 1, 200, SCREEN_HEIGHT) );
	int y2 = (int) ( SCREEN_HEIGHT - ScaleCoordiate(raw_y2, 0, 1, 200, SCREEN_HEIGHT) );

	int x1 = (int) ScaleCoordiate(raw_x1, 0, 1, 200, SCREEN_WIDTH);
	int x2 = (int) ScaleCoordiate(raw_x2, 0, 1, 200, SCREEN_WIDTH);
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); 
	Bresenham_line(x1, y1, x2 ,y2);
	std::cerr << x1 << " " << y1 << " " << x2 << " " << y2 << "\n";
}

double CalcCoordinate(std::vector<double> weights, double x){
	if(weights.empty()) return (-1);
	return (-weights[1] * x - weights[0]) / weights[2]; 
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
