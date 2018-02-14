#include "Macros.h"
#include <vector>
#ifdef WINDOWS
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include "Perceptron.h"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

class Geometry{
	private:
	SDL_Window* _window;
	SDL_Renderer* _renderer;
	double _scale;
	double _offset_x;
	double _offset_y;
	
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
	void DrawSamples(std::vector<TraningSample>& ts, SDL_Event* e );
	void HandleMouse(SDL_Event* e, int c_x, int c_y, int r, TraningSample& ts);
	
	void scale(double scale);
	SDL_Renderer* GetRenderer() {return _renderer;}

};
