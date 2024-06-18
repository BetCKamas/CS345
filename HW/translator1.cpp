#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <cctype>
#include <bitset>
#include <vector>
#include <algorithm>

using namespace std;

/*
	const string R0 = "0000000000000000";
	const string R1 = "0000000000000001";
	const string R2 = "0000000000000010";
	const string R3 = "0000000000000011";
	const string R4 = "0000000000000100";
	const string R5 = "0000000000000101";
	const string R6 = "0000000000000110";
	const string R7 = "0000000000000111";
	const string R8 = "0000000000001000";
	const string R9 = "0000000000001001";
	const string R10 = "0000000000001010";
	const string R11 = "0000000000001011";
	const string R12 = "0000000000001100";
	const string R13 = "0000000000001101";
	const string R14 = "0000000000001110";
	const string R15 = "0000000000001111";
	const string SCREEN = "0100000000000000"; 
	const string KBD = "0110000000000000";
	const string SP = "0000000000000000";
	const string LCL = "0000000000000001";
	const string ARG = "0000000000000010";
	const string THIS = "0000000000000011";
	const string THAT = "0000000000000100";
*/

vector<string> predefinedSymbolName = { "R0", "R1", "R2", "R3", "R4", "R5", "R6",  "R7",
										"R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15",
										"SCREEN", "KBD", "SP", "LCL", "ARG", "THIS", "THAT"
										};
										
vector<string> predefinedSymbolValue = { "0000000000000000", "0000000000000001", "0000000000000010", "0000000000000011",
										"0000000000000100", "0000000000000101", "0000000000000110", "0000000000000111",
										"0000000000001000", "0000000000001001", "0000000000001010", "0000000000001011",
										"0000000000001100", "0000000000001101", "0000000000001110", "0000000000001111",
										"0100000000000000", "0110000000000000", "0000000000000000", "0000000000000001",
										"0000000000000010", "0000000000000011", "0000000000000100"
										};
 										

vector<string> additionalSymbols;
vector<string> symbolBinary;

vector<string> allInstructions;
vector<string> binaryAllInstructions;


bool empty(const string& str) {
    bool result;
	for (char c : str) {
        if (!isspace(c)) {
            result = false;
        }
    }
    result = true;
	return result;
}

string convertToBinary(int value){
	bitset<16> bits(value);
	return bits.to_string();	
}

int customNum = 0;

void recordSymbol(string instruct){
	instruct.erase(0, 1); // delete first char to getr rid of @
	bool matchPredefined = false;
	for(auto a: predefinedSymbolName) {		// checks if predefined
		if(instruct.compare(a) == 0){
			matchPredefined = true;			// is predefined no need to add
		}
	}
	if(!matchPredefined){	// not predefined, needs to be added
		additionalSymbols.push_back(instruct);
		symbolBinary.push_back(convertToBinary(customNum+16));
		customNum++;
	}
}

void recordLabel(string instruct, int val){
	instruct.erase(0,1); // remove '('
	instruct.pop_back(); // remove ')'
	
	additionalSymbols.push_back(instruct);
	symbolBinary.push_back(convertToBinary(val));
}

int main(int argc, char* argv[]){
	if (argc > 1) {
		string filename = argv[1];
		ifstream file(filename);
		if (file.is_open()) {
			int lineNum = 0;
			string instruct;
			vector<string> instructions;
			while (getline(file, instruct)) {
				allInstructions.push_back(instruct);
				if(!empty(instruct) && instruct[0] != '/'){					// checks if its an empty line or a comment and ignores
					switch(instruct[0]){
						case '(': recordLabel(instruct, lineNum+1); break;
						default: lineNum++; break;
					}
					allInstructions.push_back(instruct);
				}
			}
			file.close();
			for(auto a: allInstructions){
				cout << a << endl;;
			}
		} else {
			std::cerr << "Unable to open file: " << filename << std::endl;
		}
	} else {
		std::cerr << "No filename provided" << std::endl;
	}
	
	return 0;
}