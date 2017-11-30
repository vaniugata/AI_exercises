#ifndef PERCEPTRON_H
#define PERCEPTRON_H

#include <vector>
#include <SDL2/SDL.h>

struct TraningSample {
	std::vector<bool> samples;
	bool correct_output;

	TraningSample( bool a, bool b, bool output ) {
		samples = std::vector<bool>(3);
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

	void InitWeights( unsigned input_size );

private:	
	void SumInputs( std::vector<bool>& inputs );

public:
	void Input( std::vector<bool>& inputs );
	bool Output();
	void Train( std::vector<TraningSample>& samples, unsigned cycle_cnt, double lr );
	void Train( std::vector<int>& input, bool target_output, double lr );
	
	const std::vector<double>& GetWeights() const; 

	void SetWeights(std::vector<double> weights);
};
#endif
