#include <vector>
#include <SDL2/SDL.h>
#include "Perceptron.h"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 800;

class Geometry{
	private:
	SDL_Window* _window;
	SDL_Renderer* _renderer;
	double _scale;
	
	public:
	Geometry();
	~Geometry();

	void InitSDL();
	double CalcCoordinate(std::vector<double> weights, double x);
	double ScaleCoordiate(double, double, double, double, double);
	void Bresenham_line(int x1, int y1, int x2, int y2, bool dotted = false);
	void DrawLine( std::vector<double> weights );
	void DrawCircle(int x, int y, int r);
	void UpdateScreen();
	void DrawSamples(std::vector<TraningSample>& ts, Perceptron* p );
	
	void scale(double scale);
	SDL_Renderer* GetRenderer() {return _renderer;}

};
