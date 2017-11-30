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

void Perceptron::InitWeights( unsigned input_size ){
	if( _weights.empty() ){
		for(unsigned i = 0; i < input_size; i++){
			char sign = rand() % 2 ? 1 : (-1);
			_weights.push_back( sign * (double)(rand() % 101) / 100.0f  );
		}
	}
}

void Perceptron::SumInputs( std::vector<bool> & inputs ) { 
	for(unsigned char i = 0; i < _weights.size(); ++i) {
		_sum += inputs[i] * _weights[i];
	}
	std::cout << "sum: " << _sum << "\n";
}

void Perceptron::Input( std::vector<bool> & inputs ) {
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
	if( _training_finished ) std::cerr << "Training finished.\n";  return;	
	bool has_error = false;
	for(unsigned i = 0; i < cycle_cnt; ++i){
		for(unsigned char j = 0; j < samples.size(); ++j){
			Input( samples[j].samples );
			double error = samples[j].correct_output - Output();
			std::cerr << "error: "<< error << "\n";
			has_error |= error != 0;
			for(unsigned char k = 0; k < _weights.size(); ++k){
				_weights[k] += lr * error * samples[j].samples[k]; 
			}
		}
		if(has_error){
			has_error = false;
			_training_finished = false;
		} else {
			std::cerr<<std::boolalpha<<has_error<<"\n";  
			_training_finished = true;
		}
	}
}

void Perceptron::Train( std::vector<int>& input, bool target_output, double lr ){
	for(unsigned i = 0; i < input.size(); i++){
		if( i < _weights.size() ){
			_sum += _weights[i] * input[i];
		}	
	}
	char error = target_output - Output();
			std::cerr << "error: "<< (int)error << "\n";
	if(error != 0){
		for(unsigned i = 0; i < _weights.size(); i++){
			_weights[i] += error * input[i] * lr;	
		}
	}
}

const std::vector<double>& Perceptron::GetWeights() const {
	return _weights;
}

void Perceptron::SetWeights(std::vector<double> weights){
	_weights = weights;
}
