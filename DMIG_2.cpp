#include <fstream>
#include<queue> 
#include <iostream>
#include <string>
//#include <sstream>
#include <climits> // INT_MAX
#include <map>
#include <algorithm>
#include <vector>
using namespace std;
class Cut2;
int k;
int LUT_count;
long long int merge_id;
int max_level;

std::string input_file;
std::map<std::string, Cut2> TO_order;
queue<std::string> all_nodes;
queue<std::string> PI_PO;
std::vector<std::string> PO;

std::string model_info;
//std::string input_info;
//std::string output_info;
std::vector<std::string> input_info;
std::vector<std::string> output_info;

class Cut2{
public:
	std::vector<std::vector<int> > table;
	std::vector<std::string> innodes;
	std::vector<std::string> outnodes;
	std::string name;
	int gate_type;
	int level;
	bool vis1;
	bool visit;
	Cut2(){
		this->level=INT_MAX;
		this->visit=false;
		this->vis1=false;
	};
};

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

bool compare_level(std::string i1, std::string i2)
{
    return (TO_order[i1].level > TO_order[i2].level);
}

bool tag(char& first){
	bool ntag;
	if(first!='.'){ntag=true;}else{ntag=false;}
	return ntag;
}

void process_PO(ifstream& inFile, std::string& readline){
	//output_info=readline;
	std::string tmp; //.outputs	
	bool next_line=true;
	bool first=true;
	int start;
	//Using PO_PI

	while(next_line){
		output_info.push_back(readline);
		if(first){
			start=9;
			first=false;
		}else{
			start=0;
		}
		for(int i=start; i<readline.size(); i++){ //output nodes start from index 8
			if(readline[i]!=' '){
				tmp.push_back(readline[i]);
			}else{
				if(tmp!=""){
					PO.push_back(tmp);
					//cout << tmp << endl;
					tmp.clear();
				}					
			}
		}
		if(tmp!="" && tmp!="\\"){
			PO.push_back(tmp);
			//cout << tmp << endl;
			tmp.clear();
			next_line=false;
		}else if(tmp=="\\"){
			tmp.clear();
			getline(inFile, readline);
		}
	}	
}

void process_PI(ifstream& inFile, std::string& readline){
	std::string tmp;
	//input_info=readline;
	bool next_line=true;
	bool first=true;
	int start;
	while(next_line){
		input_info.push_back(readline);
		if(first){
			start=8;
			first=false;
		}else{
			start=0;
		}
		for(int i=start; i<readline.size(); i++){ //input nodes start from index 8
			if(readline[i]!=' '){
				tmp.push_back(readline[i]);
			}else{
				if(tmp!=""){
					TO_order[tmp].name=tmp;
					TO_order[tmp].gate_type=-1; //Because it's primary input
					TO_order[tmp].level=0;
					TO_order[tmp].visit=true;
					TO_order[tmp].vis1=true;
					all_nodes.push(tmp);

					PI_PO.push(tmp);
					//cout << tmp << endl;
					tmp.clear();
				}					
			}
		}
		if(tmp!="" && tmp!="\\"){
			TO_order[tmp].name=tmp;
			TO_order[tmp].gate_type=-1;
			TO_order[tmp].level=0;
			TO_order[tmp].visit=true;
			TO_order[tmp].vis1=true;
			all_nodes.push(tmp);
			PI_PO.push(tmp);
			//cout << tmp << endl;
			tmp.clear();
			next_line=false;
		}else if(tmp=="\\"){
			tmp.clear();
			getline(inFile, readline);
		}
	}
	
}

int Gate_type(std::string& readline, bool ntag){
	int type; //0:and 1:or 2:not 3:constant_0 4:constant_1 
	if(!ntag){
		type=3;
		return type;
	}else{
		int size=readline.size();
		if(size==1){ //constant
			if(readline[0]==1){type=4;}else{type=5;}
			return type;
		}else{
			for(int i=0; i<size; i++){
				if(readline[i]!=' '){;
					if(readline[i]=='-'){
						type=1;
						return type;
					}else if(readline[i]=='1'){
						type=0;
					}else{ //=0
						type=2;
						return type;
					}				
				}			
			}
			return type;
		}		
	}
}

void Process_Cut(ifstream& inFile, std::string& readline){
	std::string tmp;
	std::vector<std::string> tmp_nodes;
	std::string out_name;
	int g_type;
	bool ntag;
	for(int i=7; i<readline.size(); i++){ //.names node1 node2 01234567 
				if(readline[i]!=' '){
					tmp.push_back(readline[i]);
				}else{
					if(tmp!=""){
						tmp_nodes.push_back(tmp);
						tmp.clear();
					}					
				}
			}
	if(tmp!=""){		
		out_name=tmp;
		tmp.clear();
	}

	getline(inFile, readline);
	ntag=tag(readline[0]);
	g_type=Gate_type(readline, ntag);
	//cout << g_type << endl;
	if(g_type==3 || g_type ==4){
		TO_order[out_name].level=0;
		all_nodes.push(out_name);
		PI_PO.push(out_name);
		TO_order[out_name].gate_type=g_type;
	}else{

		TO_order[out_name].gate_type=g_type;
		TO_order[out_name].innodes=tmp_nodes;
		TO_order[out_name].name=out_name;

		std::string tmp_in_name;
		for(int i=0; i<tmp_nodes.size(); i++){
			tmp_in_name=tmp_nodes[i];
			TO_order[tmp_in_name].outnodes.push_back(out_name);
		}

	}
	while(ntag){
		getline(inFile, readline);
		ntag=tag(readline[0]);
	}
}

void read_file(){
	cout << "Reading file: " << input_file << endl;
	ifstream inFile(input_file);
	std::string readline;

	int size;
	getline(inFile, readline);
	bool ntag=true;
	ntag=tag(readline[0]);

	while(ntag || readline[1]!='e'){

		if(!ntag){ // This is tags
			if(readline[1]=='m'){
				model_info=readline;
			}else if(readline[1]=='i'){
				process_PI(inFile, readline);
			}else if(readline[1]=='o'){
				process_PO(inFile, readline);
			}
		}

		if(readline[1]=='n'){
			Process_Cut(inFile, readline);
		}else{
			getline(inFile, readline);
			ntag=tag(readline[0]);
		} 		
	}
	inFile.close();
}

void Split_nodes(Cut2* cut_t){
	//std::stringstream ss;
	int max_id;
	std::string small_1;
	std::string small_2;
	std::string merge_name;
	int next_level;
	while(cut_t->innodes.size()>2){
		std::sort(cut_t->innodes.begin(),cut_t->innodes.end(), compare_level);
		small_1=cut_t->innodes.back();
		cut_t->innodes.pop_back();
		small_2=cut_t->innodes.back();
		cut_t->innodes.pop_back();
		//ss << merge_id;
		//merge_name="["+ss.str()+"]";
		merge_name="["+std::to_string(merge_id)+"]";
		merge_id++;
		TO_order[merge_name].name=merge_name;
		TO_order[merge_name].innodes.push_back(small_1);
		TO_order[merge_name].innodes.push_back(small_2);
		TO_order[small_1].outnodes.push_back(merge_name);
		TO_order[small_2].outnodes.push_back(merge_name);
		TO_order[merge_name].gate_type=cut_t->gate_type;
		TO_order[merge_name].level=TO_order[small_1].level+1;
		TO_order[merge_name].outnodes.push_back(cut_t->name);

		cut_t->innodes.push_back(merge_name);
	}
	//std::sort(cut_t->innodes.begin(),cut_t->innodes.end(), compare_level);
	next_level=TO_order[cut_t->innodes[0]].level+1;
	//cout << next_level << endl;

	cut_t->level=next_level;
	if(next_level>max_level){
		max_level=next_level;
	}
}

void DMIG(){
	std::string node_t;
	std::string tmp_node_nm;
	Cut2* t_cut;
	int this_lb;
	//int next_lb;
	bool vis1;
	bool next_vis1;
	int input_num;
	while(!all_nodes.empty()){
		node_t=all_nodes.front();
		//cout << node_t << endl;
		
		all_nodes.pop();
		t_cut=&TO_order[node_t];
		this_lb = t_cut->level;
		vis1 = t_cut->vis1;
		//cout << t_cut->level << endl;
		if(this_lb!=0){ //unvisited node input node label=0
		//this_lb==-1	

			if(t_cut->innodes.size()>2){
				Split_nodes(t_cut);
			}else{
				std::sort(t_cut->innodes.begin(),t_cut->innodes.end(), compare_level);
				t_cut->level=TO_order[t_cut->innodes[0]].level+1;
				if(t_cut->level>max_level){
					max_level=t_cut->level;
				}
			}	
		}

		for(int i=0; i<t_cut->outnodes.size(); i++){
			tmp_node_nm=t_cut->outnodes[i];
			//next_lb=TO_order.at(tmp_node_nm).level;
			next_vis1=TO_order[tmp_node_nm].vis1;
			if(next_vis1==false){
				all_nodes.push(tmp_node_nm);
				TO_order[tmp_node_nm].vis1=true;
			}	
		} 
	}
}
void print_2_truth_table(ofstream& outfile, int gate_type, int size){
	//0:and 1:or 2:not 3:constant_0 4:constant_1
	if(gate_type==0){
		if(size==1){
			outfile << "1 1" << endl;
		}else{
			outfile << "11 1" << endl;
		}
	}else if(gate_type==1){
		outfile << "1- 1" << endl;
		outfile << "-1 1" << endl;
	}else if(gate_type==2){
		outfile << "0 1" << endl;
	}else if(gate_type==3){

	}else if(gate_type==4){
		outfile << "1" << endl;
	}else{

	}
}

void write_blif(){
	std::string outblif_name="Out2_"+input_file;
	std::string travel_node;
	ofstream outfile;
	Cut2* travel_cut;
	outfile.open(outblif_name, ios::out);
	int size;
	int gate_type;

	outfile << model_info << endl;
	//outfile << input_info << endl;
	//outfile << output_info << endl;

	for(int i=0; i<input_info.size(); i++){
		outfile << input_info[i] << endl;
	}
	for(int i=0; i<output_info.size(); i++){
		outfile << output_info[i] << endl;
	}

	while (!PI_PO.empty())
  	{	
  		travel_node=PI_PO.front();
  		//cout << travel_node << endl;
  		PI_PO.pop();
  		travel_cut=&TO_order[travel_node];
  		gate_type=travel_cut->gate_type;

  		if(travel_cut->level!=0){ //print line .names
  			LUT_count++;
  			outfile << ".names";
  			size=travel_cut->innodes.size();
  			for(int i=0;i<size;i++){		
  				outfile << " " << travel_cut->innodes[i];
  			}
  			outfile << " " << travel_cut->name;
  			outfile << endl;
  			print_2_truth_table(outfile, gate_type, size);
  		}
  		  		
  		for (int i=0;i<travel_cut->outnodes.size();i++){
  			if(TO_order[travel_cut->outnodes[i]].visit==false){
  				PI_PO.push(travel_cut->outnodes[i]);
  				TO_order[travel_cut->outnodes[i]].visit=true;
  			}		
  		}
  	}

  	outfile << ".end" << endl;
	outfile.close();

	for(int j=0; j<PO.size(); j++){

	}

	cout << "The circuit level is " << max_level << "." << endl;
	cout << "The number of LUTs is " << LUT_count << "." << endl;
}

int main(int argc, char* argv[])
{	
	merge_id=1;
	LUT_count=0;
	max_level=0;
	input_arg(argc, argv);
	if(k==2){
		read_file();
		DMIG();
		write_blif();
	}else{
		cout << "Sorry! DAG is still unfinished. It only can process the input with k=2" << endl;
	}
	
	return 0;
}