#include "nn.h"
#include "iostream"
#include "ctime"
#include <random>
#include "cmath"
#include "sstream"

#define RANGE 0.4
#define DEBUG false

using namespace std;

//forward declars for util functions located at bottom of this file
void log(string);
double randomReal();
vector<vector<double> >* getRandomWeightArray(int,int);
void printVec(vector<double>*);
void print2dVec(vector<vector<double> > *vec);
double mse(vector<double>&);

//NeuralNetwork class

NeuralNetwork::NeuralNetwork(int inputs, int outputs, vector<int> 	hidden) {
	srand(time(0));
	Layer *inputL = new Layer(inputs);
	this->layers.push_back(inputL);
	for(int i=0;i<hidden.size();i++) {
		addLayer(hidden[i]);
	}
	addLayer(outputs);
}

void NeuralNetwork::addLayer(int nodes) {
	Layer *lastLayer = layers.back();
	Layer *newLayer = new Layer(nodes);
	vector<vector<double> > *adjMat = getRandomWeightArray(lastLayer->size,newLayer->size);
	lastLayer->frontWeights = adjMat;
	newLayer->backWeights = adjMat;
	this->layers.push_back(newLayer);
}

void NeuralNetwork::train(vector<Pattern*> patterns) {
	log("Training pattern");

	int i = 0;
	int numPats = patterns.size();
	double error = 1.0;
	double sum = 0.0;
	while(i<maxIter && error > errorThresh) {
		sum = 0.0;
		for(int i=0;i<numPats;i++) {
			sum += this->trainPattern(patterns[i]);
		}
		error = sum / (double) numPats;
		i++;
	}
	cout << "Error: " << error << endl;
	cout << "Iterations: " << i << endl;
}

double NeuralNetwork::trainPattern(Pattern *pat) {
	//run network normally
	run(pat->inputs);
	//backpropagation
	log("Expected result");
	if(DEBUG) printVec(pat->outputs);
	double err = findError(pat->outputs);
	adjustWeights();

	return err;
}

vector<double> NeuralNetwork::run(vector<double> *inputs) {
	log("Beginning propagation.");

	this->layers[0]->outputs = *inputs;
	for(int i=1;i<layers.size();i++) {
		layers[i]->propagate(layers[i-1]->outputs);
	}

	log("Propagation complete. Result:");
	if(DEBUG) printVec(&(layers.back()->outputs));
	return layers.back()->outputs;
}

double NeuralNetwork::findError(vector<double> *outputs) {
	log("Beginning error computation.");
	//run findOutputLayerError on last layer using outputs
	//run findHiddenLayerError on all other layers, except for input, using the predecessor's delta vector

	for (int i = layers.size()-1; i >= 1; i--) {
		if(DEBUG) cout << "Computing error for layer " << i+1 << endl;

		Layer& layer1 = *(layers[i]);
		if(i==layers.size()-1) {
			layer1.findOutputLayerError(*outputs);
		} else {
			Layer& layer2 = *(layers[i+1]);
			layer1.findHiddenLayerError(layer2.deltas);
		}
	}

	return mse(layers.back()->outputs);
}

void NeuralNetwork::adjustWeights() {
	//use each output as incoming values over every edge
	//adjust each edge according to:
	//change = learningRate*delta*incoming[k]+momentum*change

	log("Adjusting edge weights");
	for (int i = 1; i < layers.size(); i++) {
		if(DEBUG) cout << "Adjusting layer "<<i+1 << endl;

		Layer& layer1 = *(layers[i-1]);
		Layer& layer2 = *(layers[i]);
		layer2.adjustWeights(learningRate,momentum,layer1.outputs);
	}
}

void NeuralNetwork::printNetwork() {
	for (int i = 0; i < layers.size()-1; ++i) {
		cout << "Layer: "<< i << endl;
		layers[i]->printFrontWeights();
	}
}

//Layer class

Layer::Layer(int s) {
	this->size=s;
	this->outputs.resize(s);
	this->errors.resize(s);
	this->deltas.resize(s);
	for (int i = 0; i < s; ++i) biases.push_back(randomReal()*RANGE-RANGE/2);
	this->changes.resize(s);
}

double Layer::sigmoid(double in) {
	//sigmoid(in) = 1/(1+e^(-in))
	return 1.0/(1.0+exp(-in));
}

void Layer::adjustWeights(double learningRate, double momentum, vector<double> &incoming) {
	//use each incoming value to adjust each edge
	//adjust each edge according to:
	//change[edge] = learningRate*delta*incoming[node]+momentum*change[edge]

	//for each node in this layer
	for (int i = 0; i < size; i++) {
		//for each node in the previous layer
		for (int j = 0; j < incoming.size(); j++) {

			if(DEBUG) cout << "Adjusting node "<< i+1 << " edge "<<j+1 << endl;

			//initialize the changes table
			if(changes[i].size() <= j) changes[i].push_back(0); 

			double change = learningRate*deltas[i]*incoming[j]+momentum*changes[i][j];
			
			(*backWeights)[j][i] += change;
			changes[i][j] = change;

			if(DEBUG) cout << "change: " << change << "=" << learningRate << " * " << deltas[i] << " * " << incoming[i] << endl;
		}

		biases[i]+=learningRate*deltas[i];
	}

	if(DEBUG) print2dVec(&changes);
}

void Layer::findHiddenLayerError(vector<double> &next_deltas) {
	//calculate error and delta for every node
	//error for hidden nodes = w_ij*delj + w_ik*delk ...

	//for each node in this layer
	for (int fLi = 0; fLi < outputs.size(); fLi++) {
		double error = 0;
		double output = outputs[fLi];

		//for each node in the next layer
		for (int sLj = 0; sLj < next_deltas.size(); sLj++) {
			error += next_deltas[sLj]*(*frontWeights)[fLi][sLj];
		}
		errors[fLi]=error;
		deltas[fLi]=error*output*(1-output);

		if(DEBUG) cout << "Error/delta for node " << fLi+1 << " " << error << "/" << deltas[fLi] << endl;
	}
}

void Layer::findOutputLayerError(vector<double> &target) {
	//calculate error and delta for every node
	//error for output nodes = output[node] - target[node]

	//for every output node
	for (int i = 0; i < outputs.size(); ++i) {
		double output = outputs[i];
		double error = output-target[i];
		errors[i]=error;
		deltas[i]=error*output*(1-output);

		if(DEBUG) cout << "Error/delta for node " << i+1 << " " << error << "/" << deltas[i] << endl;
	}
}

void Layer::printFrontWeights() {
	// for (int i=0; i < frontWeights->size() ; i++) {
	// 	printVec(&(frontWeights->at(i)));
	// }
	print2dVec(frontWeights);
}

void Layer::propagate(vector<double> &incoming) {
	if(DEBUG) {
		cout << "Incoming: ";
		printVec(&incoming);
	}

	for(int i=0;i<outputs.size();i++) {
		outputs[i] = biases[i];
		for(int j=0;j<incoming.size();j++) {
			double o = incoming[j] * (*backWeights)[j][i];
			outputs[i] += o;
			// cout << "Calc: "<< o << " = " << incoming[j] << " * " << (*backWeights)[j][i] << endl;
		}
		outputs[i]=sigmoid(outputs[i]);
	}

	if(DEBUG) {
		cout << "Outgoing: ";
		printVec(&outputs);
	}
}

//Util functions

void log(string line) {
	if(DEBUG) cout << line << endl;
}

double randomReal() {
	return static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
}

vector<vector<double> >* getRandomWeightArray(int r,int c) {
	vector<vector<double> > *arr = new vector<vector<double> >();
	for(int i=0;i<r;i++) {
		vector<double> vec;
		arr->push_back(vec);
		for (int j = 0; j < c; j++) {
			(*arr)[i].push_back(randomReal()*RANGE-RANGE/2);
		}
	}
	return arr;
}

void printVec(vector<double> *vec) {
	for (int i = 0; i < (*vec).size(); ++i)
	{
		cout<<(*vec)[i]<<" ";
	}
	cout<<endl;
}

void print2dVec(vector<vector<double> > *vec) {
	for (int i = 0; i < (*vec).size(); ++i)
	{
		printVec(&((*vec)[i]));
	}
}

double mse(vector<double> &errors) {
	double err = 0.0;
	for (int i = 0; i < errors.size(); ++i)
	{
		err += pow(errors[i],2.0);
	}
	return err;
}