#include <SDL2/SDL.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Perceptron.h"
#include "Geometry.h"

constexpr double LEARNING_RATE = .1f;
constexpr unsigned POINTS_CNT = 20;

int main(){
	std::srand( std::time(0) );
	Geometry* geom = new Geometry;
	geom->InitSDL();

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
				case SDLK_a : geom->scale(0.01); break;
				case SDLK_d : geom->scale(-0.01); break; 
			}
		}
		if( time + period < SDL_GetTicks()){
				p->Train(*ptrTs, 1, LEARNING_RATE);
				time = SDL_GetTicks();
		}
		geom->DrawLine( p->GetWeights() );
		
		geom->DrawSamples( *ptrTs, e );
		geom->UpdateScreen();
	}
	delete p;
	delete e;
	delete geom;
	return 0;
}
