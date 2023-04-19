#include <fstream>
#include<queue> 
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
using namespace std;

class Cut2{
public:
	std::vector<std::vector<int> > table;
	//std::vector<queue<int> > table;
	std::vector<std::string> nodes;

	std::string output;
	//std::vector<int> output_value;
	Cut2(){};
};

class Circuit{
	private:
		int inter_count;
	public:
		Circuit();

		queue<Cut2> circuit2;

		std::vector<std::string> inputs;
		std::vector<std::string> outputs;

		void input_PI(std::string node);
		void input_PO(std::string node);
		void new_gate(int type,std::vector<std::vector<int> >& table);
		void to_2_inputs(Cut2 input_cut);
};




#ifndef functions
#define functions

int and_or_gate(std::vector<int> line);
void input_arg(int argc, char* argv[]);
bool tag(char& first);
void read_file();
char str(std::vector<int>::reverse_iterator it);
void write_blif();
int DMIG(int argc, char* argv[]);


#endif