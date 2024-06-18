/*
 * This is my soloution to translating a vm file into an asm file for NAND to Tetris Project 07
 * Needs to be compiled with the vm file as a command line arg
 * By Betsy Kamas
 * November 29 2023
 */


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int labelCount = 0;

string removeFileExtension(string filename) {
    size_t lastdot = filename.find_last_of(".");
    if (lastdot == string::npos) {
        return filename;
    }
    return filename.substr(0, lastdot);
}

void readFileToVector(string filename, vector<string>& instructions){
	/* opens file and reads all lines into a vector*/
	ifstream file(filename);
	if (file.is_open()) {
		int lineNum = 0;
		string instruct;
		while (getline(file, instruct)) {
			instructions.push_back(instruct);
		}
		file.close();
	} else 
		cerr << "Unable to open file: " << filename << endl;

}

string trim(const string& str, const string& whitespace = " \t"){
	/* function removes leading / trailing whitespace from elements in vector*/
	const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == string::npos)
        return ""; // no content
    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;
    return str.substr(strBegin, strRange);
	
}

void removeNonCode(vector<string>&instructions){
	/* function removes extra whitespace along with comments and empty lines */
	for (vector<string>::iterator it = instructions.begin(); it != instructions.end(); ++it) {
		*it = trim(*it);
	}	
	instructions.erase(
				remove_if( instructions.begin(), instructions.end(), 
				[](string const& s) { return s.empty(); }),
					 instructions.end());
	instructions.erase(
				remove_if( instructions.begin(), instructions.end(), 
				[](string const& s) { return s.find("//") == 0; }),
					 instructions.end());

}

string getCmdArg(string str) {
    int i = 0;
    while (i < str.size() && str[i] == ' ') {
        i++;
    }
    int j = i;
    while (j < str.size() && str[j] != ' ') {
        j++;
    }
    return str.substr(i, j - i);
}

string getFirstArg(string str) {
    int i = 0;
    while (i < str.size() && str[i] == ' ') {
        i++;
    }
    int j = i;
    while (j < str.size() && str[j] != ' ') {
        j++;
    }
    while (j < str.size() && str[j] == ' ') {
        j++;
    }
    int k = j;
    while (k < str.size() && str[k] != ' ') {
        k++;
    }
    return str.substr(j, k - j);
}

string getIndex(string str) {
    int i = str.size() - 1;
    while (i >= 0 && str[i] == ' ') {
        i--;
    }
    int j = i;
    while (j >= 0 && str[j] != ' ') {
        j--;
    }
    return str.substr(j + 1, i - j);
}

inline string increaseSP(){
	string str = "@SP\nM=M+1";
	return str;
}

inline string decreaseSP(){
	string str = "@SP\nM=M-1\n";
	return str;
}

inline string loadSPToA(){
	string str = "@SP\nA=M\n";
	return str;
}

string popStackToD(){
	string str = decreaseSP();
	str += "A=M\nD=M\n";
	return str;
}

string pushDToStack(){
	string str = loadSPToA();
	str += "M=D\n";
	str += increaseSP();
	return str;
}

inline string loadSeg(string seg, string index){
	return "@" + seg + "\nD=M\n" + "@" + index + "\nA=D+A\n";
}

vector<string> instructionsASM;
string filename;

void writePop(string code, string loc, string index){
	string str;
	if(loc == "constant"){
		str = "@" + index + "\n";
	} else if (loc == "local"){
		str = loadSeg("LCL", index);
	} else if (loc == "argument"){
		str = loadSeg("ARG", index);
	} else if (loc == "this"){
		str = loadSeg("THIS", index);
	} else if (loc == "that"){
		str = loadSeg("THAT", index);
	} else if (loc == "pointer"){
		str = "@R" + to_string(3 + stoi(index));
	} else if (loc == "temp"){
		str = "@R" + to_string(5 + stoi(index));
	} else if (loc == "static"){
		str = "@" + filename + index + "\n";
	}
	str += "D=A\n@R13\nM=D\n";
	str += popStackToD();
	str += "@R13\nA=M\nM=D";
	instructionsASM.push_back(str);
}

void writePush(string code, string loc, string index){
	string str;
	if(loc == "constant"){
		str = "@" + index + "\n" + "D=A\n";
	} else if (loc == "local"){
		str = loadSeg("LCL", index);
		str	+= "D=M\n";
	} else if (loc == "argument"){
		str = loadSeg("ARG", index);
		str	+= "D=M\n";
	} else if (loc == "this"){
		str = loadSeg("THIS", index);
		str	+= "D=M\n";
	} else if (loc == "that"){
		str = loadSeg("THAT", index);
		str	+= "D=M\n";
	} else if (loc == "pointer"){
		str = "@R" + to_string(3 + stoi(index));
		str	+= "\nD=M\n";
	} else if (loc == "temp"){
		str = "@R" + to_string(5 + stoi(index));
		str	+= "\nD=M\n";
	} else if (loc == "static"){
		str = "@" + filename + index + "\n";
		str += "D=M\n";
	}
	str += pushDToStack();
	instructionsASM.push_back(str);
}


void translateALCode(string decode){
	string str;
	 if(decode == "add"){
		str = popStackToD();
		str += decreaseSP();
		str += loadSPToA();
		str += "M=D+M\n";
		str += increaseSP();
		instructionsASM.push_back(str);		
	} else if(decode == "sub") {
		str = popStackToD();
		str += decreaseSP();
		str += loadSPToA();
		str += "M=M-D\n";
		str += increaseSP();
		instructionsASM.push_back(str);
	} else if(decode == "and"){
		str = popStackToD();
		str += decreaseSP();
		str += loadSPToA();
		str += "M=D&M\n";
		str += increaseSP();
		instructionsASM.push_back(str);
	} else {
		instructionsASM.push_back("code line not translated ATM");
	}
	
}

void parseCode(vector<string> &instructions){
	for(auto a : instructions){
		string cmdType = getCmdArg(a);
		if(cmdType == "pop"){
			instructionsASM.push_back("// " + a);
			string loc = getFirstArg(a);
			string index = getIndex(a);
			writePop(a, loc, index);
		} else if(cmdType == "push"){
			instructionsASM.push_back("// " + a);
			string loc = getFirstArg(a);
			string index = getIndex(a);
			writePush(a, loc, index);
		} else
			translateALCode(a);
	}
}


void sendToFile(){
	ofstream myfile;
    myfile.open("Prog.asm");
	for(auto a: instructionsASM)
		myfile << a << endl;
    myfile.close();
}


int main(int argc, char* argv[]){
	if (argc > 1) {
		string file = argv[1];
		filename = removeFileExtension(file);
		vector<string> instructions;
		int linesOfCode = 0;
		readFileToVector(file, instructions); 	// reads code into vector
		removeNonCode(instructions);			// removes comments
		parseCode(instructions);
		sendToFile();			// sends to file output
	} else {
		cerr << "No filename provided" << endl;
	}
	return 0;
}