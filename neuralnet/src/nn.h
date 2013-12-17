#ifndef nn_h
#define nn_h

#include "vector"

class NeuralNetwork;
class Pattern;
class Layer;

class NeuralNetwork {
public:
	NeuralNetwork(int,int,const std::vector<int>); 		//initializes a Network with given inputs, outputs, and hidden layer sizes
	void train(std::vector<Pattern*>);					//runs trainPattern on a list of patterns
	void trainPattern(Pattern*);						//uses a pattern to train the network
	std::vector<double> run(std::vector<double>*);		//propogates given inputs through network and returns the network output
	void findError(std::vector<double>*);				//use a set of outputs for the output layer and propagates back
	void adjustWeights();								//uses errors and deltas found in findError to adjust edge weights
	void addLayer(int);									//appends a new layer of size int to the end of the existing network
	void printNetwork(); 								//print all layer adj mats to stdout
	std::vector<Layer*> layers; 						//vector of pointers to Layers
	double learningRate = 0.3;
	double momentum = 0.1;
};

class Layer {
public:
	Layer(int);											//initializes a Layer with a given number of nodes
	void propagate(std::vector<double>&);				//takes a set of incoming values and calculates outputs
	void adjustWeights(double,double,std::vector<double>&);	//uses learningRate and incoming outputs from the prev layer to adjust weights in backWeights
	void findOutputLayerError(std::vector<double>&);	//finds error and delta for each node in this output layer
	void findHiddenLayerError(std::vector<double>&);	//finds the error and delta for each node in this hidden Layer
														//note, neither of the above methods will be called on the input layer
	void printFrontWeights();							//prints the frontWeights matrix to stdout
	double sigmoid(double);								//sigmoid function
	int size;
	std::vector<double> outputs;
	std::vector<double> errors;
	std::vector<double> deltas;
	std::vector< std::vector<double> > changes;			//memory of edge adjustment
	std::vector< std::vector<double> > *backWeights;	//pointer to back edges
	std::vector< std::vector<double> > *frontWeights;	//pointer to front edges
};

class Pattern {
public:
	std::vector<double> *inputs;						//a pointer to a vector of inputs
	std::vector<double> *outputs;						//a pointer to a vector of outputs
};

#endif

