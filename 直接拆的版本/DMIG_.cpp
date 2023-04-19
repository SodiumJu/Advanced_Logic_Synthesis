#include "TecMap.h"
/*#include <fstream>
#include<queue> 
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
using namespace std;*/

class Circuit;
class Cut2;

int k;
std::string input_file;
Circuit C1;
std::string model_info;
std::string input_info;
std::string output_info;
int and_or_gate(std::vector<int> line){
	//and:0 or:1 
	int type=0;
	if(line[0]==1){
		for(int i=1; i< line.size(); i++){
			if(line[i]==2||line[i]==0){
				type=1;
				break;
			}
		}
	}else{
		type=1;
	}

	return type;
}

Circuit::Circuit(){
			this->inter_count=0;
		};

void Circuit::input_PI(std::string node){
	inputs.push_back(node);
}
void Circuit::input_PO(std::string node){
	outputs.push_back(node);
}
void Circuit::new_gate(int type,std::vector<std::vector<int> >& table){
	std::vector <int> tmp_vec;
	if(type==0){ // AND 11 1
		tmp_vec.push_back(1);
		tmp_vec.push_back(1);
		tmp_vec.push_back(1);
		table.push_back(tmp_vec);
	}else{ // OR 1- 1 
		   //    -1 1
		tmp_vec.push_back(1);
		tmp_vec.push_back(2);
		tmp_vec.push_back(1);
		table.push_back(tmp_vec);
		tmp_vec.clear();
		tmp_vec.push_back(1);
		tmp_vec.push_back(1);
		tmp_vec.push_back(2);
		table.push_back(tmp_vec);
	}
}

void Circuit::to_2_inputs(Cut2 input_cut){
	int type;
	if(input_cut.nodes.size()>1){
		type=and_or_gate(input_cut.table[0]);
	}
	while(input_cut.nodes.size()>2){
		Cut2 tmp_cut;		
		std::string node1=input_cut.nodes.back();
		input_cut.nodes.pop_back();
		
		std::string node2=input_cut.nodes.back();
		input_cut.nodes.pop_back();

		tmp_cut.nodes.push_back(node2);
		tmp_cut.nodes.push_back(node1);

		for(int i=0; i<input_cut.table.size(); i++){
			
			if(type==0){ //and gate
				input_cut.table[i].pop_back();

			}else{ //type==1 //or gate
				int val1=input_cut.table[i].back();
				input_cut.table[i].pop_back();
				int val2=input_cut.table[i].back();
				input_cut.table[i].pop_back();

				if(val1==1 || val2==1){
					input_cut.table[i].push_back(1);
				}else{
					input_cut.table[i].push_back(2);
				}
			}
		}
		new_gate(type,tmp_cut.table);
		this->inter_count++;
		std::string new_node="["+to_string(this->inter_count)+"]";
		input_cut.nodes.push_back(new_node); //input back into nodes name
		tmp_cut.output=new_node;
		
		this->circuit2.push(tmp_cut);
	}

	if(input_cut.nodes.size()==2){
		input_cut.table.clear();
		this->new_gate(type,input_cut.table);
		this->circuit2.push(input_cut);
	}else{
		this->circuit2.push(input_cut);
	}
	
//cout << "input 1 cut" << endl;	
}

void input_arg(int argc, char* argv[])
{	
	std::string str1;
	std::string str2;
	std::string str3;

	if(argc==4){
		str1=argv[1];
		str2=argv[2];
		str3=argv[3];

		if(str1=="-k"){
			k=std::stoi(str2);
			input_file=str3;	
		}else if(str2=="-k"){
			k=std::stoi(str3);
			input_file=str1;	
		}else{
			cout << "The number of parameters is incorrect." << endl;
		}
    }else{
		cout << "The number of parameters is incorrect." << endl;
	}
}



bool tag(char& first){
	bool ntag;
	if(first!='.'){ntag=true;}else{ntag=false;}
	return ntag;
}

void read_file()
{
	cout << "reading file:" << input_file << endl;
	ifstream inFile(input_file);
	std::string readline;
	std::string tmp;
	//std::string tmp_readout;
	std::vector<int> tmp_line;
	std::vector<std::vector<int> > tmp_table;
	std::vector<std::string> tmp_nodes;
	//std::vector<int> tmp_out_values;
	std::string tmp_out_node;

	//Cut2 tmp_cut;

	int size;
	//inFile >> tmp_readout;
	getline(inFile, readline);
	bool ntag=true;

	//if(readline[0]!='.'){ntag=false;}
	ntag=tag(readline[0]);

	while(ntag || readline[1]!='e'){

		if(!ntag){ // This is tags
			if(readline[1]=='m'){
				model_info=readline;
			}else if(readline[1]=='i'){
				input_info=readline;
			}else if(readline[1]=='o'){
				output_info=readline;
			}
		}

		//cout << readline << endl;
		if(readline[1]=='n'){
			Cut2 tmp_cut; //warning
			for(int i=7; i<readline.size(); i++){ //.names node1 node2 01234567 
				if(readline[i]!=' '){
					tmp.push_back(readline[i]);
				}else{
					if(tmp!=""){
						tmp_nodes.push_back(tmp);
						//cout << tmp << endl;
						tmp.clear();
					}					
				}
			}
			if(tmp!=""){
				//cout << tmp << endl;
				tmp_out_node=tmp;
				tmp.clear();
			}
			reverse(tmp_nodes.begin(), tmp_nodes.end());
			tmp_cut.nodes=tmp_nodes;
			tmp_nodes.clear();
			tmp_cut.output=tmp_out_node;
			//read .names all nodes
			getline(inFile, readline);
			ntag=tag(readline[0]);
			//if(readline[0]!='.'){ntag=true;}else{ntag=false;}
			while(ntag){
				
				//table
				size=readline.size();
				for(int i=0; i<size; i++){
					if(readline[i]!=' '){
						//cout << readline[i];
						if(readline[i]=='-'){
							tmp_line.push_back(2);
						}else if(readline[i]=='1'){
							tmp_line.push_back(1);
						}else{
							tmp_line.push_back(0);
						}				
					}			
				}
				//cout << endl;
				reverse(tmp_line.begin(), tmp_line.end());
				tmp_table.push_back(tmp_line);
				tmp_line.clear();
				getline(inFile, readline);
				ntag=tag(readline[0]);
				//if(readline[0]!='.'){ntag=true;}else{ntag=false;}
			}
			tmp_cut.table=tmp_table;
			C1.to_2_inputs(tmp_cut);
			tmp_table.clear();
			//cout << "here" << endl;
			//getline(inFile, readline);
		}else{
			getline(inFile, readline);
			ntag=tag(readline[0]);
			//if(readline[0]!='.'){ntag=true;}else{ntag=false;}
		} 		
	}
	inFile.close();
}

char str(std::vector<int>::reverse_iterator it){
	if(*it==0){
		return '0' ;
	}else if(*it==1){
		return '1' ;
	}else if(*it==2){
		return '-' ;
	}else{
		return '?' ;
	}
	
}

void write_blif(){
	std::string outblif_name="out2_"+input_file;
	ofstream outfile;
	outfile.open(outblif_name, ios::out);
	std::vector<int>::reverse_iterator it;
	
	cout << "The number of LUTs is " << C1.circuit2.size() << "." << endl;
	outfile << model_info << endl;
	outfile << input_info << endl;
	outfile << output_info << endl;

	while (!C1.circuit2.empty())
  	{	
  		outfile << ".names";
  		Cut2 cutobj=C1.circuit2.front();
  		while(!cutobj.nodes.empty()){
  			outfile << " " << cutobj.nodes.back();
  			cutobj.nodes.pop_back();
  		}
  		outfile << " "<< cutobj.output;
  		outfile << endl;
  		for(int i=0; i<cutobj.table.size(); i++){
  			for(it = cutobj.table[i].rbegin(); it != (cutobj.table[i].rend()-1); ++it){
  				outfile << str(it);				
  			}
  			outfile << ' ';
  			//it++;
  			outfile << str(it);
  			outfile << endl;
  		}
  		C1.circuit2.pop();
  	}
  	outfile << ".end" << endl;
	outfile.close();
}

int DMIG(int argc, char* argv[]){
	k=2;
	input_arg(argc, argv);
	read_file();
	write_blif();
	return k;
}

/*int main(int argc, char* argv[])
{	
	
	input_arg(argc, argv);
	read_file();
	write_blif();
	return 0;
}*/