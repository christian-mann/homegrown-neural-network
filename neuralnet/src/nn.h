#ifndef nn_h
#define nn_h

#include "vector"

using namespace std;

class NeuralNetwork;
class Pattern;
class Layer;

class NeuralNetwork {
public:
	NeuralNetwork(int,int,const vector<int>); 		//initializes a Network with given inputs, outputs, and hidden layer sizes
	void train(vector<Pattern*>);					//runs trainPattern on a list of patterns
	void trainPattern(Pattern*);						//uses a pattern to train the network
	vector<double> run(vector<double>*);		//propogates given inputs through network and returns the network output
	void findError(vector<double>*);				//use a set of outputs for the output layer and propagates back
	void adjustWeights();								//uses errors and deltas found in findError to adjust edge weights
	void addLayer(int);									//appends a new layer of size int to the end of the existing network
	void printNetwork(); 								//print all layer adj mats to stdout
	vector<Layer*> layers; 						//vector of pointers to Layers
	double learningRate = 0.8;
	double momentum = 0.3;
};

class Layer {
public:
	Layer(int);											//initializes a Layer with a given number of nodes
	void propagate(vector<double>&);				//takes a set of incoming values and calculates outputs
	void adjustWeights(double,double,vector<double>&);	//uses learningRate and incoming outputs from the prev layer to adjust weights in backWeights
	void findOutputLayerError(vector<double>&);	//finds error and delta for each node in this output layer
	void findHiddenLayerError(vector<double>&);	//finds the error and delta for each node in this hidden Layer
														//note, neither of the above methods will be called on the input layer
	void printFrontWeights();							//prints the frontWeights matrix to stdout
	double sigmoid(double);								//sigmoid function
	int size;
	vector<double> outputs;
	vector<double> errors;
	vector<double> deltas;
	vector<double> biases;
	vector< vector<double> > changes;			//memory of edge adjustment
	vector< vector<double> > *backWeights;	//pointer to back edges
	vector< vector<double> > *frontWeights;	//pointer to front edges
};

class Pattern {
public:
	vector<double> *inputs;						//a pointer to a vector of inputs
	vector<double> *outputs;						//a pointer to a vector of outputs
};

#endif

