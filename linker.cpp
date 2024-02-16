// TARUN SHARMA
// ts5098

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <stdexcept> 
#include <regex> 

#include <iomanip>

// Variables to track line offsets and numbers for error reporting
int templineoffset = 0;
int templineno = 0;
int prevtokenlen = 0;
bool first_time = true;

#include <array>


// Defines the structure for the symbol table
template<typename T>
class TwoValueContainer {
private:
    std::array<T, 2> values;
    int index = 0;

public:
    void push(const T& newValue) {
        values[index % 2] = newValue;
        index++;
    }

    T getNewest() const {
        return values[(index - 1) % 2];
    }

    T getOldest() const {
        return values[index % 2];
    }
};

TwoValueContainer<int> containeroff;
TwoValueContainer<int> containerline;

std::vector<int> moduleList;

class SymbolTable {
// private:
    
public:

struct SymbolInfo {
        std::string symbol; // symbol name
        int value;          // The value of the symbol
        int address;        // The global address of the symbol
        int moduleno;       // The module number where the symbol is defined
        bool definedMultipleTimes = false;  // Flag for multiple definitions
        bool usedButNotDefined = false;     // Flag for symbols used but not defined
        bool definedButNotUsed = true; // // Flag for symbols defined but not used. Assume true initially, mark false when used

        SymbolInfo(const std::string& sym, int val, int addr, int mno)
            : symbol(sym), value(val), address(addr), moduleno(mno) {}
    };

    std::vector<SymbolInfo> symbols;

    // // Helper to find a symbol in the vector. Constructor to initialize a symbol information
    std::vector<SymbolInfo>::iterator findSymbol(const std::string& symbol) {
        for (auto it = symbols.begin(); it != symbols.end(); ++it) {
            if (it->symbol == symbol) {
                return it;
            }
        }
        return symbols.end();
    }

    const SymbolInfo& getSymbolInfo(int i) const {
        if (i >= 0 && i < symbols.size()) {
            return symbols[i];
        } else {
            throw std::out_of_range("Index out of range");
        }
    }
    // Add a symbol to the symbol table
    void addSymbol(const std::string& symbol, int value, int address, int baseAddress, int module) {
        auto it = findSymbol(symbol);
        if (it != symbols.end()) {
            // Symbol is already defined
            std:: cout << "Warning: Module " << module << ": " << symbol << " redefinition ignored"<<std::endl;
            it->definedMultipleTimes = true;
        } else {
            symbols.push_back(SymbolInfo(symbol, value, address + baseAddress, module));
        }
    }

    // Get the address of a symbol, returns -1 if the symbol does not exist
    int getAddress(const std::string& symbol) {
        auto it = findSymbol(symbol);
        if (it == symbols.end()) {
            return -1; // Per assignment specs, use 0 for undefined symbols
        }
        it->definedButNotUsed = false; // Mark as used
        return it->address;
    }

    // Print the symbol table
    void print() {
        std::cout << "Symbol Table" << std::endl;
        for (const auto& entry : symbols) {
            std::cout << entry.symbol << "=" << entry.address;
            if (entry.definedMultipleTimes) {
                std::cout << " Error: This variable is multiple times defined; first value used";
            }
            std::cout << std::endl;
        }
        // Iterate again for usedButNotDefined and definedButNotUsed errors and warnings
        for (const auto& entry : symbols) {
            if (entry.usedButNotDefined) {
                std::cout << "Error: " << entry.symbol << " is not defined; zero used" << std::endl;
            }
        }
    }
};


// Utility function to convert an integer to a string with padding
std::string intToStringWithPadding(int value) {
    std::ostringstream oss;
    oss << std::setw(3) << std::setfill('0') << value;
    return oss.str();
}

std::string intToStringWithPadding4digit(int value) {
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << value;
    return oss.str();
}


// Function to report parsing errors with specific codes
void parseError(int errcode, int lineoffset, int linenum) {
    static const char* errstr[] = {
        "NUM_EXPECTED", // Number expected
        "SYM_EXPECTED", // Symbol Expected
        "MARIE_EXPECTED", // Addressing Expected which is A/E/I/R
        "SYM_TOO_LONG", // Symbol Name is too long
        "TOO_MANY_DEF_IN_MODULE", // > 16
        "TOO_MANY_USE_IN_MODULE", // > 16
        "TOO_MANY_INSTR" // total num_instr exceeds memory size (512)
    };

    std::string message = "Parse Error line " + std::to_string(linenum) + " offset " + std::to_string(lineoffset) + ": " + errstr[errcode];
    std::cout << message;
    exit(0);
}

// Function to process tokens, primarily for debugging and verification purposes
void processToken(const std::string& token, int lineoffset, int linenum) {
    std::cout << "token=<" << token << "> linenum=" << linenum << ", position=" << lineoffset << std::endl;
}

// Utility function to print the endings of lines, used for debugging line and offset calculations
void printLineEndings(std::ifstream& inputFile) {
    std::string line;
    int lineNum = 0;
    int offset = 0; // Assuming offset is the position before the newline at the end of each line

    while (std::getline(inputFile, line)) {
        lineNum++; // Increment line number
        offset += line.length(); // Update offset to current position before newline
        
        // Print the current line's end information
        std::cout << "Line " << lineNum << " ends at offset " << offset << std::endl;
        
        offset += 1; // Account for the newline character itself
    }
}

// Tokenizer function to extract tokens from input
bool tokenizer(std::ifstream& inputFile, int& linenum, int& curr, std::string& token) {
    static std::string line;
    static std::istringstream iss;
    static bool newLine = true;

    if (!first_time){

    containeroff.push(curr);
    containerline.push(linenum);
    }
    first_time = false;
    if (newLine) {
        if (!std::getline(inputFile, line)) {
            return false; // No more lines to read
        }
        iss.clear(); // Clear any error flags
        iss.str(line); // Assign new string
        newLine = false;
        curr = 0; // Reset current position for new line
    }

    if (!(iss >> token)) {
        newLine = true; // Prepare to read a new line on next call
        linenum++; // Increment line number for next read
        return tokenizer(inputFile, linenum,curr,  token); // Try getting next token from new line
    }

    // Calculate the offset of the current token within the line
    int tokenPos = static_cast<int>(iss.tellg()) - static_cast<int>(token.length());
    if (tokenPos < 0) { // When the token is the last one in the line, tellg() returns -1
        tokenPos = static_cast<int>(line.length()) - static_cast<int>(token.length());
    }
    curr += tokenPos; // Update current position globally
    // processToken(token, curr+1, linenum+1);
    templineoffset = curr + 1;
    templineno = linenum + 1;
    prevtokenlen = token.length();

    return true; // Token was successfully read
}

// Corrected readInt and readSym functions
int readInt(std::ifstream& inputFile, int& lineoffset, int linenum) {

    std::string token;
    if (tokenizer(inputFile, lineoffset, linenum, token)) {
        // Convert token to int and return
        // return std::stoi(token); // Make sure to add error handling for non-integer tokens
        try {
            // Attempt to convert token to int and return
            return std::stoi(token);
        } catch (const std::invalid_argument& e) {
            // Handle the case where conversion fails due to invalid argument (non-integer token)
            parseError(0, templineoffset, templineno); // Use appropriate error code for "NUM_EXPECTED"
            exit(1); // Or handle more gracefully if desired
        } catch (const std::out_of_range& e) {
            // Handle the case where the number is out of range for int
            parseError(0, templineoffset, templineno); // "NUM_EXPECTED" or consider another error message for out of range
            exit(1); // Or handle more gracefully if desired
        }
    } 
    return -1;
}

bool isValidSymbol(const std::string& token) {
    if (token.empty() || !std::isalpha(token[0])) {
        return false; // Symbol must start with an alpha character
    }

    for (size_t i = 1; i < token.size(); ++i) {
        if (!std::isalnum(token[i])) {
            return false; // Subsequent characters must be alphanumeric
        }
    }

    return true;
}

std::string readSym(std::ifstream& inputFile, int& lineoffset, int linenum) {
    // Symbols always begin with alpha characters followed by optional alphanumerical characters, 
    // i.e.[a-Z][a-Z0-9]*. Valid symbols can be up to 16 characters.
    std::string token;

    int prevlinenum = linenum;
    int prevlineoffset = lineoffset;
    if (tokenizer(inputFile, lineoffset, linenum, token)) {
        if (token.size() > 16) {
            parseError(3, linenum+1, lineoffset+1); // Symbol too long error
            return "";
        } else if (!isValidSymbol(token)) {
            if (templineoffset == containeroff.getOldest()+1 & templineno ==containerline.getOldest()+1){
            parseError(1, containeroff.getOldest()+1 + prevtokenlen, containerline.getOldest()+1);
        }
        else{
            parseError(1, templineoffset, containerline.getOldest()+1); // Invalid symbol format error
        }
            return "";
        }
        return token;
    } 
    else {
        if (templineoffset == containeroff.getOldest()+1 & templineno == containerline.getOldest()+1){
            parseError(1, containeroff.getOldest()+1 + prevtokenlen, containerline.getOldest()+1);
        }
        else{
            parseError(1, templineoffset, containerline.getOldest()+1); // Invalid symbol format error
        }
        return "";
    }
}

char readIEAR(std::ifstream& inputFile, int& lineoffset, int linenum) {
    std::string token;
    if (tokenizer(inputFile, lineoffset, linenum, token) && !token.empty()) {
        char firstChar = token[0];
        // Check if the first character of the token is one of the allowed characters
        if (firstChar == 'M' || firstChar == 'A' || firstChar == 'R' || firstChar == 'I' || firstChar == 'E') {
            return firstChar;
        } else {
            // If the token's first character is not one of the specified characters, throw a parse error 
            parseError(2, templineoffset, templineno); // Use the appropriate error code for this specific error
            exit(1); // Or handle the error as appropriate for your application
        }
    } else {
        parseError(2, templineoffset + prevtokenlen, templineno); // Error code for token not found or EOF reached unexpectedly
        exit(1); // Or handle the error as appropriate for your application
    }
    // This return statement is redundant due to the exits above but is necessary to avoid compiler warnings
    return 0;
}

int getOp(std::ifstream& inputFile, int& lineoffset, int linenum){
    std::string token;
    // int lineoffset;
    tokenizer(inputFile, lineoffset, linenum, token);
    int num = std::stoi(token);
    return num;
}

void operatorM (SymbolTable& symbolTable,
             int instruction, int i, int baseAddress, int module){
    // (M) operand is the number of a valid module and is replaced with the base address of that module.
    int opcode = instruction / 1000;
    int operand = instruction % 1000;

    if (operand > moduleList.size()-1){
        int repaceModule = moduleList[0];
        std::string ans = intToStringWithPadding4digit(instruction + repaceModule - operand);
        std:: cout << intToStringWithPadding(i + baseAddress) << ": " << ans << 
        " Error: Illegal module operand ; treated as module=0" << std::endl;   
    }
    else{
        int repaceModule = moduleList[operand];
        std::string ans = intToStringWithPadding4digit(instruction - operand + repaceModule);
        std:: cout << intToStringWithPadding(i + baseAddress) << ": " << ans << std::endl;
    }

}

void operatorA (SymbolTable& symbolTable,
             int instruction, int i, int baseAddress, int module){
    // (A) operand is an absolute address which will never be changed in 
    // pass2; however it can’t be “>=” the machine size (512);

    int opcode = instruction / 1000;
    int operand = instruction % 1000;

    if (operand >= 512){
        std::string ans = intToStringWithPadding4digit(instruction - operand);
        std:: cout << intToStringWithPadding(i + baseAddress) << ": " << ans << " "
        << "Error: Absolute address exceeds machine size; zero used" << std:: endl;
    }
    else{
        std::string ans = intToStringWithPadding4digit(instruction);
        std:: cout << intToStringWithPadding(i + baseAddress) << ": " << ans << std::endl;
    }
}

void operatorR (SymbolTable& symbolTable, 
             int instruction, int i, int baseAddress, int module, int instcount){
    // (R) operand is a relative address in the module which is relocated by replacing the 
    // relative address with the absolute address of that relative address after the module’s 
    // global address has been determined (absolute_addr = module_base+relative_addr).
    // (I) an immediate operand is unchanged, but must be less than 900.
    // std:: cout << "This is R Address" << operand << std::endl;
    int opcode = instruction / 1000;
    int operand = instruction % 1000; // Assuming this gives the direct index in symbols

    if (operand >= instcount){
        std::string ans = intToStringWithPadding4digit(instruction - operand + baseAddress);
        std:: cout << intToStringWithPadding(i + baseAddress) << ": " << ans << 
        " Error: Relative address exceeds module size; relative zero used" << std::endl;
    }
    else{
        std::string ans = intToStringWithPadding4digit(instruction + baseAddress);
        std:: cout << intToStringWithPadding(i + baseAddress) << ": " << ans << std::endl;
    }
}

void operatorI (SymbolTable& symbolTable,
             int instruction, int i, int baseAddress, int module){
    // (I) an immediate operand is unchanged, but must be less than 900.
    int opcode = instruction / 1000;
    int operand = instruction % 1000; 

    if (operand >= 900){
        // std:: cout << "MAAKI" << instruction<< std::endl;
        std::string ans = intToStringWithPadding4digit(instruction - operand + 999);
        std:: cout << intToStringWithPadding(i + baseAddress) << ": " << ans << 
        " Error: Illegal immediate operand; treated as 999" << std::endl;
    }
    else{
        std::string ans = intToStringWithPadding4digit(instruction);
        std:: cout << intToStringWithPadding(i + baseAddress) << ": " << ans<< std::endl;
    }
}


void operatorE (SymbolTable& symbolTable, std::vector<std::pair<std::string, bool>>& useListValues,
                int instruction, int i, int baseAddress, int module) {
    // (E) operand is an external address which is represented as an index into the uselist. 
    // For example, a reference in the program text with operand K represents the Kth symbol in the 
    // use list, using 0-based counting, e.g., if the use list is ‘‘2 f g’’, then an instruction 
    // ‘‘E 7000’’ refers to f, and an instruction ‘‘E 5001’’ refers to g. You must identify to which 
    // global address the symbol is assigned and then replace the operand with that global address.
    int opcode = instruction / 1000;
    int operand = instruction % 1000; // Assuming this gives the direct index in symbols
    
    // std:: cout << "YOYOYO This is newaddreess speaking" << newaddress <<std::endl;
    if (operand >= useListValues.size()){
        // int temp = (instruction - operand);
        std::cout << intToStringWithPadding(i + baseAddress) << ": " <<  intToStringWithPadding4digit(instruction - operand + baseAddress) << 
        " Error: External operand exceeds length of uselist; treated as relative=0" << std::endl;
        return;
    }
    std:: string uselisttoken = useListValues[operand].first;
    // std:: cout<< "Imma back bitches "<<  useListValues.size() <<std::endl;
        
    int newaddress = symbolTable.getAddress(uselisttoken);
    if (newaddress < 0){
        
        int temp = (instruction - operand);
        useListValues[operand].second = true;
        std::cout << intToStringWithPadding(i + baseAddress) << ": " << intToStringWithPadding4digit(temp) << 
        " Error: " << useListValues[operand].first << " is not defined; zero used" << std::endl;
    }
    else {
        useListValues[operand].second = true;
        int temp = (instruction - operand) + newaddress;
        std::cout << intToStringWithPadding(i + baseAddress) << ": " << intToStringWithPadding4digit(temp) << std::endl;
    }
    
}

// The `operate` function dispatches the operation based on the instruction's addressing mode.
// It takes as input the symbol table, memory map, a list of use list values (symbols and a flag indicating if used),
// a list of base addresses for each module, the addressing mode of the current instruction, the instruction itself,
// the instruction's index within the current module, the base address of the current module, the module number, and the instruction count of the current module.
// This function directs the instruction to the appropriate operator function (M, A, R, I, or E) based on its addressing mode.

void operate (SymbolTable& symbolTable, std::vector<std::pair<std::string, bool>>& useListValues,
             char addressmode, int instruction, int i, int baseAddress, int module, int instcount){
    
    if (addressmode == 'M') operatorM(symbolTable, instruction, i, baseAddress, module);
    else if (addressmode == 'A') operatorA(symbolTable, instruction, i, baseAddress, module);
    else if (addressmode == 'R') operatorR(symbolTable, instruction, i, baseAddress, module, instcount);
    else if (addressmode == 'I') operatorI(symbolTable, instruction, i, baseAddress, module);
    else if (addressmode == 'E') operatorE(symbolTable, useListValues, instruction, i, baseAddress, module);
}

// First pass function to build the symbol table and prepare for address resolution
// The `firstPass` function performs the first pass of the two-pass linker process.
// It reads the input file to build the symbol table and calculates the base address for each module.
// This setup is necessary for the address resolution that occurs in the second pass.
// The function iterates through each module in the input file, processing definition lists, use lists, and program text.

void firstPass(std::ifstream& inputFile, SymbolTable& symbolTable) {
    int module = 0;
    int linenum = 0;
    int curr = 0;
    int baseAddress = 0;

    int totaldefcount = 0;
    int totalusecount = 0;
    int totalinstcount = 0;

    while (!inputFile.eof()) {
        std::string token;
        int lineoffset;

        int defcount = readInt(inputFile, curr, linenum); //return negative to indicate no more tokens
        if (defcount == -1) break;
        totaldefcount += defcount;
  
        if (defcount > 16) parseError(4, templineoffset, templineno);
        for (int i=0; i<defcount; i++){
            std::string sym;
            // std::cout << curr << std::endl;
            sym = readSym(inputFile, curr, linenum);
            int val = readInt(inputFile, curr, linenum);
            symbolTable.addSymbol(sym, val, val, baseAddress, module); // Add symbol to the table
        }

        int usecount = readInt(inputFile, curr, linenum);
        totalusecount += usecount;
        if (usecount > 16) parseError(5, templineoffset, templineno);
        for (int i=0; i<usecount; i++){
            std::string sym;
            sym = readSym(inputFile, curr, linenum);
        }
        moduleList.push_back(baseAddress);
 
        int instcount = readInt(inputFile, curr, linenum);
        totalinstcount += instcount;
        if (totalinstcount > 512) parseError(6, templineoffset, templineno);
        for (int i=0; i<instcount; i++){
            char addressmode = readIEAR(inputFile, curr, linenum);
            int operand = getOp(inputFile, curr, linenum);
        }
        
        for (auto& symboltemp : symbolTable.symbols) {
            if (symboltemp.moduleno == module){
                if (symboltemp.value > instcount - 1){
                    std::cout << "Warning: Module " << module << ": " << symboltemp.symbol << "="
                    << symboltemp.value<<" valid=[0.."<<instcount - 1<<"] assume zero relative" << std::endl;
                    symboltemp.address = baseAddress;
                }
            }
        }
        
        baseAddress += instcount;
        module ++;
        
    }
 
    symbolTable.print();
    std::cout<<"\n";
}

// Second pass function to resolve addresses and generate the final memory map output
// The `secondPass` function performs the second pass of the linker process.
// Using the symbol table and base addresses computed in the first pass, it resolves external references and computes the final memory addresses for each instruction.
// The function iterates through each module, now focusing on modifying the instructions based on their addressing mode and the previously built symbol table.

void secondPass(std::ifstream& inputFile, SymbolTable& symbolTable) {

    // Implement second pass logic here
    int module = 0;
    int linenum = 0;
    int curr = 0;
    int baseAddress = 0;

    int totalinstcount = 0;

    std::cout << "Memory Map"<< std::endl;

    while (!inputFile.eof()) {
        std::string token;

        std::vector<std::pair<std::string, bool>> useListValues;

        int lineoffset;

        int defcount = readInt(inputFile, curr, linenum); //return negative to indicate no more tokens
        if (defcount == -1) break;

        if (defcount > 16) parseError(4, linenum+1, curr+1);
        for (int i=0; i<defcount; i++){
            std::string sym;

            sym = readSym(inputFile, curr, linenum);
            int val = readInt(inputFile, curr, linenum);
            // symbolTable.addSymbol(sym, val, baseAddress); // Add symbol to the table
        }

        int usecount = readInt(inputFile, curr, linenum);
        if (usecount > 16) parseError(5, linenum+1, curr+1);
        for (int i=0; i<usecount; i++){
            std::string sym;
            sym = readSym(inputFile, curr, linenum);
            
            useListValues.push_back(std::make_pair(sym, false)); 
        }
        int instcount = readInt(inputFile, curr, linenum);
        
        totalinstcount += instcount;
        if (totalinstcount > 512) parseError(6, linenum+1, curr+1);
        for (int i=0; i<instcount; i++){
            char addressmode = readIEAR(inputFile, curr, linenum);
            int instruction = getOp(inputFile, curr, linenum);
 
            int opcode = instruction / 1000;
            int operand = instruction % 1000;
                if (opcode >= 10){
        
                std:: cout << intToStringWithPadding(i + baseAddress) << ": " << 9999 << 
                " Error: Illegal opcode; treated as 9999" << std::endl;
            } 
            else{
                operate(symbolTable, useListValues, addressmode, instruction, i, baseAddress, module, instcount);
            }
            
        }

        for (size_t i = 0; i < useListValues.size(); ++i) {
        // Check if the bool part of the pair is false
        if (!useListValues[i].second) {
            // Print the index and the string part
            std::cout << "Warning: Module " << module<< ": uselist[" << i <<"]=" <<  
            useListValues[i].first <<" was not used" <<std::endl;
        }
    }
        baseAddress += instcount;
        module ++;
        
    }

    std:: cout<<"\n";
    for (const auto& symbolInfo : symbolTable.symbols) {
        // Check if the symbol is defined but not used
        if (symbolInfo.definedButNotUsed) {
            // Print a warning message for each symbol that is defined but not used
            std::cout << "Warning: Module " << symbolInfo.moduleno << ": " 
                      << symbolInfo.symbol << " was defined but never used" << std::endl;
        }
    }
}

// The `parseInputFile` function orchestrates the overall linking process by calling the first and second pass functions.
// It opens the input file, initializes the symbol table and memory map, and then calls `firstPass` and `secondPass` to perform the linking.
// Afterward, it closes the input file.

void parseInputFile(const std::string& fileName) {
    std::ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return;
    }
    SymbolTable symbolTable;

    firstPass(inputFile, symbolTable);
    inputFile.clear();
    inputFile.seekg(0);
    secondPass(inputFile, symbolTable);

    inputFile.close();
}

// Main function to parse the input file and orchestrate the two-pass linking process
int main(int argc, char* argv[]) {
    // Main function remains mostly unchanged
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <inputfile>" << std::endl;
        return 1;
    }

    std::string inputFileName = argv[1];
    parseInputFile(inputFileName);
    std::cout<<"\n";
    std::cout<<"\n";

    return 0;
}
