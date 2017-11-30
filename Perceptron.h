#ifndef PERCEPTRON_H
#define PERCEPTRON_H

#include <vector>
#include <SDL2/SDL.h>

struct TraningSample {
	std::vector<float> samples;
	bool correct_output;

	TraningSample( float a, float b, bool output ) {
		samples = std::vector<float>(3);
		samples = {1, a , b};

		correct_output = output;
	}
};

class Perceptron {
private:
	std::vector<double> _weights;
	double _sum;
	bool _training_finished;
public:
	Perceptron();
	virtual ~Perceptron();

private:	
	void SumInputs( std::vector<float>& inputs );

public:
	void Input( std::vector<float>& inputs );
	bool Output();
	void Train( std::vector<TraningSample>& samples, unsigned cycle_cnt, double lr );
	void Reset();

	const std::vector<double>& GetWeights() const; 

	void SetWeights(std::vector<double> weights);
};
#endif
