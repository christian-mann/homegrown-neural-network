#include "nn.h"
#include "vector"
#include "iostream"
#include "fstream"
#include "string"
#include "sstream"

using namespace std;

const size_t MAX_SIZE = (size_t)-1;

//uses getline, but deletes anything after a hashmark #
void parseLine(ifstream &input_file,string &line) {
	if(getline(input_file,line)) {
		size_t s = line.find('#');
		if(s<MAX_SIZE)line.erase(line.begin()+s,line.end());
	}
}

void parsePatternFile(ifstream &pat_file,
					  int &inputs,
					  int &outputs,
					  vector<int> &hiddens,
					  vector<Pattern*> &trainPats,
					  vector<Pattern*> &validPats)
{
	string line;
	istringstream iss;
	int ntrains, nvalids;
	parseLine(pat_file,line);
	iss.str(line);
	iss.clear();

	//read in first line
	iss >> inputs >> outputs;
	int h;
	while(iss>>h) hiddens.push_back(h);
	parseLine(pat_file,line);
	iss.str(line);
	iss.clear();

	//read in second line
	iss >> ntrains >> nvalids;

	//read in training data
	for (int i = 0; i < ntrains; ++i) {
		Pattern *p = new Pattern();
		vector<double> *inputs = new vector<double>();
		vector<double> *outputs = new vector<double>();
		(*p).inputs = inputs;
		(*p).outputs = outputs;
		trainPats.push_back(p);

		parseLine(pat_file,line);
		iss.str(line);
		iss.clear();
		double d;
		while(iss >> d) {
			(*inputs).push_back(d);
		}
		parseLine(pat_file,line);
		iss.str(line);
		iss.clear();
		while(iss >> d) {
			(*outputs).push_back(d);
		}
	}

	//read in validation data
	for (int i = 0; i < nvalids; ++i) {
		Pattern *p = new Pattern();
		vector<double> *inputs = new vector<double>();
		vector<double> *outputs = new vector<double>();
		(*p).inputs = inputs;
		(*p).outputs = outputs;
		validPats.push_back(p);

		parseLine(pat_file,line);
		iss.str(line);
		iss.clear();
		double d;
		while(iss >> d) {
			(*inputs).push_back(d);
		}
		parseLine(pat_file,line);
		iss.str(line);
		iss.clear();
		while(iss >> d) {
			(*outputs).push_back(d);
		}
	}
}

int main(int argc, char const *argv[]) {
	if(argc!=2) {
		cout << "Supply name of pattern file." << endl;
		return 0;
	}

	string file_name = argv[1];
	ifstream input_file;
	input_file.open(file_name.c_str());

	if(!input_file.is_open()) {
		cout << "Couldn't open file "<< file_name << endl;
		return 0;
	}

	int inputs,outputs;
	vector<int> hiddens;
	vector<Pattern*> trainPats;
	vector<Pattern*> validPats;
	vector<vector<double> > results;

	parsePatternFile(input_file,inputs,outputs,hiddens,trainPats,validPats);

	input_file.close();

	NeuralNetwork network(inputs,outputs,hiddens);

	cout << "Weights before training:" << endl;
	network.printNetwork();

	cout << endl << "Begin Training" << endl;
	network.train(trainPats);

	cout << endl << "Weights after training:" << endl;
	network.printNetwork();

	double validErr = network.validation(validPats,results);
	cout << endl;
	cout << "Validation Error:" << validErr << endl;
	cout << "Outputs" << endl;

	print2dVec(&results);

	//HARD CODED XOR TEST

	// int inputs = 2;
	// int outputs = 1;
	// vector<int> hidden;
	// hidden.push_back(2);

	// NeuralNetwork network(inputs,outputs,hidden);
	// // network.printNetwork();

	// vector<Pattern*>& trainPats = *(new vector<Pattern*>());
	// Pattern *p1 = new Pattern();
	// vector<double> *i1 = new vector<double>({0.0,0.0});
	// vector<double> *o1 = new vector<double>({0.0});
	// Pattern *p2 = new Pattern();
	// vector<double> *i2 = new vector<double>({0.0,1.0});
	// vector<double> *o2 = new vector<double>({1.0});
	// Pattern *p3 = new Pattern();
	// vector<double> *i3 = new vector<double>({1.0,0.0});
	// vector<double> *o3 = new vector<double>({1.0});
	// Pattern *p4 = new Pattern();
	// vector<double> *i4 = new vector<double>({1.0,1.0});
	// vector<double> *o4 = new vector<double>({0.0});
	// p1->inputs=i1;
	// p2->inputs=i2;
	// p3->inputs=i3;
	// p4->inputs=i4;
	// p1->outputs=o1;
	// p2->outputs=o2;
	// p3->outputs=o3;
	// p4->outputs=o4;
	// trainPats.push_back(p1);
	// trainPats.push_back(p2);
	// trainPats.push_back(p3);
	// trainPats.push_back(p4);

	// network.train(trainPats);

	// network.printNetwork();
	// cout << network.run(i1)[0] << endl;
	// cout << network.run(i2)[0] << endl;
	// cout << network.run(i3)[0] << endl;
	// cout << network.run(i4)[0] << endl;

	return 0;
}
