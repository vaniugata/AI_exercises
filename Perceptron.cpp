#include "Perceptron.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

Perceptron::Perceptron() :
	_sum(0), _training_finished(false)
{

}

Perceptron::~Perceptron() {

}

void Perceptron::SumInputs( std::vector<float> & inputs ) { 
	for(unsigned i = 0; i < _weights.size(); ++i) {
		_sum += inputs[i] * _weights[i];
	}
//	std::cout << "sum: " << _sum << "\n";
}

void Perceptron::Input( std::vector<float> & inputs ) {
	if( _weights.empty() ) {
		_weights = std::vector<double>(inputs.size(), 1);
	}
	_sum = 0;
	SumInputs(inputs);
}

bool Perceptron::Output() {
    return (_sum > 0);
}

void Perceptron::Train(std::vector<TraningSample>& samples, unsigned cycle_cnt, double lr) {
	if( _training_finished ) {
		std::cerr << "Training finished.\n";
		return;	
	}
	bool has_error = false;
	for(unsigned i = 0; i < cycle_cnt; ++i){
		// set weights and sum
		for(unsigned j = 0; j < samples.size(); ++j){
			Input( samples[j].samples );
			double error = samples[j].correct_output - Output();
			has_error |= (error != 0);
			//adjust weights
			for(unsigned k = 0; k < _weights.size(); ++k){
				_weights[k] += lr * error * samples[j].samples[k]; 
			}
		}
		if(has_error){
			has_error = false;
			_training_finished = false;
		} else {
			_training_finished = true;
		}
	}
}

void Perceptron::Reset(){
	this->_training_finished = false;
}

const std::vector<double>& Perceptron::GetWeights() const {
	return _weights;
}

void Perceptron::SetWeights(std::vector<double> weights){
	_weights = weights;
}
