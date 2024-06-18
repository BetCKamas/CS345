/*
 * This is my soloution to translating an asm file into binary for NAND to Tetris Project 06
 * Needs to be compiled with the asm file as a command line arg
 * By Betsy Kamas
 * November 18 2023
 */


#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <vector>
#include <algorithm>
#include <regex>

using namespace std;


vector<string> symbolNames = { "R0", "R1", "R2", "R3", "R4", "R5", "R6",  "R7",
								"R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15",
								"SCREEN", "KBD", "SP", "LCL", "ARG", "THIS", "THAT"};
	
vector<string> symbolBinary = { "0000000000000000", "0000000000000001", "0000000000000010", "0000000000000011",
								"0000000000000100", "0000000000000101", "0000000000000110", "0000000000000111",
								"0000000000001000", "0000000000001001", "0000000000001010", "0000000000001011",
								"0000000000001100", "0000000000001101", "0000000000001110", "0000000000001111",
								"0100000000000000", "0110000000000000", "0000000000000000", "0000000000000001",
								"0000000000000010", "0000000000000011", "0000000000000100" };
 																		
vector<string> cState = { "0", "1", "-1", "D", "A", "!D", "!A", "-D", "-A", "D+1",
							"A+1", "D-1", "A-1", "D+A", "D-A", "A-D", "D&A", "D|A",
							"M", "!M", "-M", "M+1", "M-1", "D+M", "D-M", "M-D", "D&M", "D|M"};

vector<string> cBinary = { "0101010", "0111111", "0111010", "0001100", "0110000", "0S001101",
						   "0110001", "0001111", "0110011", "0011111", "0110111", "0001110",
						   "0110010", "0000010", "0010011", "0000111", "0000000", "0010101",
						   "1110000", "1110001", "1110011", "1110111", "1110010", "1000010",
						   "1010011", "1000111", "1000000", "1010101"};

vector<string> dState = {"M", "D", "DM", "A", "AM", "AD", "ADM"};
vector<string> jState = {"JGT", "JEQ", "JGE", "JLT", "JNE", "JLE", "JMP"};
vector<string> djBinary = {"001", "010", "011", "100", "101", "110", "111"};


string convertToBinarySixteen(int value){
	/* function takes in an int and converts it to a 16 bit long binary cast as a string*/
	bitset<16> bits(value);
	return bits.to_string();	
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
				[](std::string const& s) { return s.empty(); }),
					 instructions.end());
	instructions.erase(
				remove_if( instructions.begin(), instructions.end(), 
				[](std::string const& s) { return s.find("//") == 0; }),
					 instructions.end());

}

string removeParen(const string& str){
    /* function removes parantheses from lables */
	const auto strBegin = str.find_first_not_of('(');
    if (strBegin == string::npos)
        return ""; // no content
    const auto strEnd = str.find_last_not_of(')');
    const auto strRange = strEnd - strBegin + 1;
    return str.substr(strBegin, strRange);
}

void recordLabel(vector<string>instructions){
	/* finds the labels in the code and adds them to the vector of all the other predefined items and records its binary value*/
	int val = 0;
	for (vector<string>::iterator it = instructions.begin(); it != instructions.end(); ++it) {
		string code = *it;
		if(code[0] == '('){
			code = removeParen(code);
			symbolNames.push_back(code);
			symbolBinary.push_back(convertToBinarySixteen(val+1));
		} else
			val++;
	}

}

bool checkForLetters(string code){
	/* true if digits false if letter */
	return all_of(code.begin(), code.end(), ::isdigit);
}

void recordSymbols(vector<string>instructions){
	/* records value locations of variables by first occurance*/
	int customNum = 0;
	for (vector<string>::iterator it = instructions.begin(); it != instructions.end(); ++it) {
		string code = *it;
		if(code[0] == '@'){
			code.erase(0,1);
			if(!checkForLetters(code)){ // checks if number or symbol
				bool matchPredefined = false;
				for(auto a: symbolNames) {		// checks if predefined
					if(code == a){
						matchPredefined = true;			// is predefined no need to add
					}
				}
				if(!matchPredefined){	// not predefined, needs to be added
					symbolNames.push_back(code);
					symbolBinary.push_back(convertToBinarySixteen(customNum+16));
					customNum++;
				}
			} else {
				symbolNames.push_back(code);
				symbolBinary.push_back(convertToBinarySixteen(stoi(code)));
				customNum++;
			}
		}
	}
}

string findSymbol(string &code){
	/* returns binary string of the symbol value based on @'s */
	string result;
	int position = 0;
	bool found = false;
	for (vector<string>::iterator it = symbolNames.begin(); it != symbolNames.end(); ++it) {
		string predefinedCode = *it;
		if(!found){
			if(code == predefinedCode){
				result = symbolBinary[position];
				found = true;
			}
			position++;
		}
	}
	return result;
}

void breakUpInstruction(string&code, string&dest, string&comp, string&jump){
	/* breaks up the c style instruction into its parts and removes whitespace in the middle*/
	code.erase(remove(code.begin(), code.end(), '\t'), code.end());
	code.erase(remove(code.begin(), code.end(), ' '), code.end());
	char equal = '='; // Delimiter character
	char semicolon = ';'; // Delimiter character
	size_t end = code.size();
    size_t start_indexD = 0;
    size_t start_indexC = 0;
	size_t locEqual = code.find(equal); // index of the equals
    size_t locSemicolon = code.find(semicolon); // index of the semicolon
	
	if (locEqual != string::npos) // gets string before =
        dest = code.substr(start_indexD, locEqual); 
	
	if (locSemicolon != string::npos) // gets string between = and ; if it exists
        comp = code.substr(locEqual+1, locSemicolon);
    else 
		comp = code.substr(locEqual+1, end);
		
	if(locSemicolon != string::npos) // gets string after ; if it exists
		jump = code.substr(locSemicolon+1, end); 

	//cout << dest << " = " << comp << " ; " << jump << endl;
}

string decodeC(string code){
	/* translating the c style comp into its binary with the correct a value*/
	string binary = "111";
	int num = 0;
	for (vector<string>::iterator it = cState.begin(); it != cState.end(); ++it) {
			string line = *it;
			if(code == line)
				binary += cBinary[num];
			num++;
		}
	return binary;
}

string decodeD(string code){
	/* translating the c style dest into its binary */
	string binary;
	int num = 0;
	if(!code.empty()){
		for (vector<string>::iterator it = dState.begin(); it != dState.end(); ++it) {
			string line = *it;
			if(code == line)
				binary = djBinary[num];
			num++;
		}
	} else 
		binary = "000";
	
	return binary;
}

string decodeJ(string code){
	/* translating the c style jump into its binary */
	string binary;
	int num = 0;
	if(!code.empty()){
		for (vector<string>::iterator it = jState.begin(); it != jState.end(); ++it) {
			string line = *it;
			if(code == line)
				binary = djBinary[num];
			num++;
		}
	} else 
		binary = "000";
	return binary;
}

string removeMidlineComment(string& code){
    /* function removes midline comment from lables */
	regex re("//.*");
    string commentFree = regex_replace(code, re, "");
	return commentFree;
}

void translateInstructions(vector<string> &instructions, vector<string> &instructionsBinary){
	/* translating all instructions into binary in order */
	string dest, comp, jump, all;
	for (vector<string>::iterator it = instructions.begin(); it != instructions.end(); ++it) {
		*it = removeMidlineComment(*it);
		string code = *it;

		if(!code.empty()){
			switch(code[0]){
				case '@': code.erase(0,1); instructionsBinary.push_back(findSymbol(code)); break;
				case '(': break;
				default:
				trim(code);
				breakUpInstruction(code, dest, comp, jump);
				all = decodeC(comp);
				all += decodeD(dest);
				all += decodeJ(jump);
				instructionsBinary.push_back(all);
				break;
			}
		}
	}
}

void sendToFile(vector<string> &instructionsBinary){
	ofstream myfile;
    myfile.open("Prog.hack");
	for(auto a: instructionsBinary)
		myfile << a << endl;
    myfile.close();
}

int main(int argc, char* argv[]){
	if (argc > 1) {
		string file = argv[1];
		vector<string> instructions;
		vector<string> instructionsBinary;
		int linesOfCode = 0;
		readFileToVector(file, instructions);
		removeNonCode(instructions);
		recordLabel(instructions);
		recordSymbols(instructions);
		translateInstructions(instructions, instructionsBinary);
		sendToFile(instructionsBinary);
	} else {
		cerr << "No filename provided" << endl;
	}
	return 0;
}