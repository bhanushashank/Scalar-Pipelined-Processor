#include <bits/stdc++.h>
using namespace std;

typedef vector<int> vi;
typedef queue<vector<int>> qv;

int instructionCount = 0;
int arithmeticInstructions = 0;
int logicalInstructions = 0;
int dataInstructions = 0;
int controlInstructions = 0;
int halts = 0;
int dataStalls = 0;
int controlStalls = 0;

int PC = 0;
int IR = 0;
int ALUoutput;
int LMD;							
int stalls = 0;

int registerFile[16];
bool valid[16];
int instructionCache[256];				
int dataCache[256];
bool stop = false;

queue<int> IFBuffer;
qv IDBuffer;
qv EXBuffer;
qv MEMBuffer;							
qv WBBuffer;

int controlHazard = 0;
int dataHazard = 0;
bool flag = true;
int clockCycles = 0;

int HexToNum(char);
void haltCheck();
void InstructionFetch();
void InstructionDecode();
void Execute();
void MemoryAccess();
void WriteBack();
void ControlUnit();

// Convert Hexadecimal character to integer
int HexToNum(char c){
    if('0' <= c && c <= '9'){
        return (c - '0');
    } else return (c - 'a') + 10;
}

// Convert Hexadecimal character to binary string
string HexToBin(char c){
    string output = "";
    int x = HexToNum(c);
    output += (char)('0' + x/8);
    x %= 8;
    output += (char)('0' + x/4);
    x %= 4;
    output += (char)('0' + x/2);
    x %= 2;
    output += (char)('0' + x);
    return output;
}

// Convert Hexadecimal number (as string) to integer
int HexToInt(string s){
   string s1 = HexToBin(s[0]);
   int x = 0;
   for(int i=1; i<4; i++)
       x += HexToNum(s1[i])*(1 << 7-i);

   x += HexToNum(s[1]);
   if(s1[0] == '1') x -= 128;
   return x;
}

// Convert Integer to Hexadecimal number (in string form)
string IntToHex(int a){
    string s = "";
    int b = a/16;
    int c = a % 16;
    if(b >= 0 && b <= 9)  s  += (char)('0' + b);
    else s += (char)('a' + (b - 10)); 
    
    if(c >=0 && c <= 9) s += (char)('0' + c);
    else s += (char)('a' + (c - 10)); 
    return s;
}

// Instruction Fetch stage of pipeline
void InstructionFetch(){	
    if(stop) return;					
	
    IR = (instructionCache[PC] << 8) + instructionCache[PC + 1];
    PC += 2;
    IFBuffer.push(IR);
    int MSB4 = IR >> 12;
    stop = (MSB4 == 15);
}

// Instruction Decade stage of pipeline
void InstructionDecode(){
    if(IFBuffer.empty() && IDBuffer.empty()) return;	
    
    vi opCode(4);				
	if(!IFBuffer.empty()){
		int inst = IFBuffer.front();					
		IFBuffer.pop();
		
        // Retrieve opCode % 16;
        opCode[3] = inst % 16;
        inst = inst >> 4;
        opCode[2] = inst % 16;
        inst = inst >> 4;
        opCode[1] = inst % 16;
        inst = inst >> 4;
        opCode[0] = inst % 16;
        inst = inst >> 4;
		
	    stop = (opCode[0] == 15);
        instructionCount++;
		IDBuffer.push(opCode);
	}
	if(!IDBuffer.empty())
    {
        vi op = IDBuffer.front();
        vi temp;
        switch(op[0]){
            case 0: 
                    if(!valid[op[3]] || !valid[op[2]]){
                        dataStalls++;
                        stalls++;
                        dataHazard = 1;
                    }
                    else{
                        valid[op[1]] = 0;
                        temp = IDBuffer.front();
                        temp.push_back(0);
                        EXBuffer.push(temp);
                        arithmeticInstructions++;
                        IDBuffer.pop();
                    }
                    break;
            case 1: 
                    if(!valid[op[3]] || !valid[op[2]]){
                        dataStalls++;
                        stalls++;
                        dataHazard = 1;
                    }
                    else{
                        valid[op[1]] = 0;
                        temp = IDBuffer.front();
                        temp.push_back(0);
                        EXBuffer.push(temp);
                        arithmeticInstructions++;
                        IDBuffer.pop();
                    }
                    break;
            case 2: 
                    if(!valid[op[3]] || !valid[op[2]]){
                        dataStalls++;
                        stalls++;
                        dataHazard = 1;
                    }
                    else{
                        valid[op[1]] = 0;
                        temp = IDBuffer.front();
                        temp.push_back(0);
                        EXBuffer.push(temp);
                        arithmeticInstructions++;
                        IDBuffer.pop();
                    }
                    break;
            case 4:
                    if(!valid[op[3]] || !valid[op[2]]){
                        dataStalls++;
                        stalls++;
                        dataHazard = 1;
                    }
                    else{
                        valid[op[1]] = 0;
                        temp = IDBuffer.front();
                        temp.push_back(0);
                        EXBuffer.push(temp);
                        logicalInstructions++;
                        IDBuffer.pop();
                    }
                    break;
            case 5: 
                    if(!valid[op[3]] || !valid[op[2]]){
                        dataStalls++;
                        stalls++;
                        dataHazard = 1;
                    }
                    else{
                        valid[op[1]] = 0;
                        temp = IDBuffer.front();
                        temp.push_back(0);
                        EXBuffer.push(temp);
                        logicalInstructions++;
                        IDBuffer.pop();
                    }
                    break;
            case 7: 
                    if(valid[op[3]] && valid[op[2]]){
                        dataStalls++;
                        stalls++;
                        dataHazard = 1;
                    }
                    else{
                        valid[op[1]] = 0;
                        temp = IDBuffer.front();
                        temp.push_back(0);
                        EXBuffer.push(temp);
                        logicalInstructions++;
                        IDBuffer.pop();
                    }
                    break;
            case 6:
                    if(!valid[op[2]]){
                        dataStalls++;
                        stalls++;
                        dataHazard = 1;
                    }
                    else{
                        valid[op[1]] = 0;
                        temp = IDBuffer.front();
                        temp.push_back(0);
                        EXBuffer.push(temp);
                        logicalInstructions++;
                        IDBuffer.pop();
                    }
            case 8: 
                    if(!valid[op[2]]){
                        dataStalls++;
                        stalls++;
                        dataHazard = 1;
                    }
                    else{
                        valid[op[1]] = 0;
                        temp = IDBuffer.front();
                        temp.push_back(0);
                        EXBuffer.push(temp);
                        dataInstructions++;
                        IDBuffer.pop();
                    }
                    break;
            case 3: 
                    if(!valid[op[1]]){
                        dataStalls++;
                        stalls++;
                        dataHazard = 1;
                    }
                    else{
                        valid[op[1]] = 0;
                        temp = IDBuffer.front();
                        arithmeticInstructions++;
                        temp.push_back(0);
                        EXBuffer.push(temp);
                        if(!IDBuffer.empty()) IDBuffer.pop();
                    }
                    break;
            case 11:
                    if(!valid[op[1]]){
                        dataStalls++;
                        stalls++;
                        dataHazard = 1;
                    }
                    else{
                        temp = IDBuffer.front();
                        controlInstructions++;
                        controlStalls += 2;
                        stalls += 2;
                        controlHazard = 2;
                        if(registerFile[op[1]] == 0){
                            temp.push_back(1);
                            while((int)IDBuffer.size() > 0) IDBuffer.pop();
                            while((int)IFBuffer.size() > 0) IFBuffer.pop();
                            stop = false;
                        }
                        else temp.push_back(0);

                        EXBuffer.push(temp);
                        if((int)IDBuffer.size() > 0) IDBuffer.pop();
                    }
                    break;
            case 9: 
                    if(valid[op[2]] && valid[op[1]]){
                        dataInstructions++;
                        temp = IDBuffer.front();
                        temp.push_back(0);
                        EXBuffer.push(temp);
                        IDBuffer.pop();
                    }
                    else{
                        dataStalls++;
                        stalls++;
                        dataHazard = 1;
                    }
                    break;
            case 10:
                    temp = IDBuffer.front();
                    controlInstructions++;
                    controlStalls += 2;									
                    stalls += 2;
                    controlHazard = 2;
                    temp.push_back(1);
                    while(!IFBuffer.empty()) IFBuffer.pop();					
                    while(!IDBuffer.empty()) IDBuffer.pop();
                    stop = false;
                    EXBuffer.push(temp);
                    if(!IDBuffer.empty())IDBuffer.pop();
                    break;
            case 15:
                    temp = IDBuffer.front();
                    halts++;
                    temp.push_back(0);
                    EXBuffer.push(temp);
                    if(!IDBuffer.empty())IDBuffer.pop();
                    break;
            default: cerr << "Bad Input!" << '\n'; 
        }
    }
}

// Execute stage of the pipeline
void Execute(){
    if(EXBuffer.empty()) return;
   
    vector<int> currentEX = EXBuffer.front();
    EXBuffer.pop();
    vector<int> temp;
    int x;
    int value;

    // Choose operations based on the op-code
    int operationCode;  if(currentEX.size() > 0) operationCode = currentEX[0];
    int operand0;       if(currentEX.size() > 1) operand0 = currentEX[1];
    int operand1;       if(currentEX.size() > 2) operand1 = currentEX[2];
    int operand2;       if(currentEX.size() > 3) operand2 = currentEX[3];
    switch(operationCode){
        case 0: 
                // ADD
                ALUoutput = registerFile[operand1] + registerFile[operand2];
                temp.push_back(ALUoutput);
                temp.push_back(operand0);
                MEMBuffer.push(temp);
                break;
        case 1: 
                // SUB
                ALUoutput = registerFile[operand1] - registerFile[operand2];	
                temp.push_back(ALUoutput);
                temp.push_back(operand0);
                MEMBuffer.push(temp);
                break;
        case 2: 
                // MUL
                ALUoutput = registerFile[operand1] * registerFile[operand2];	
                temp.push_back(ALUoutput);
                temp.push_back(operand0);
                MEMBuffer.push(temp);
                break;
        case 3: 
                // INC
                ALUoutput = registerFile[operand0] + 1;						
                temp.push_back(ALUoutput);
                temp.push_back(operand0);
                MEMBuffer.push(temp);
                break;
        case 4: 
                // AND
                ALUoutput = registerFile[operand1] & registerFile[operand2];	
                temp.push_back(ALUoutput);
                temp.push_back(operand0);
                MEMBuffer.push(temp);
                break;
        case 5: 
                // OR
                ALUoutput = registerFile[operand1] | registerFile[operand2]; 	
                temp.push_back(ALUoutput);
                temp.push_back(operand0);
                MEMBuffer.push(temp);
                break;
        case 6: 
                // NOT
                ALUoutput = ~registerFile[operand1];							
                temp.push_back(ALUoutput);
                temp.push_back(operand0);
                MEMBuffer.push(temp);
                break;
        case 7: 
                // XOR
                ALUoutput = registerFile[operand1] ^ registerFile[operand2];	
                temp.push_back(ALUoutput);
                temp.push_back(operand0);
                MEMBuffer.push(temp);
                break;
        case 8: 
                // LOAD
                x = operand2 - ((operand2 & 8) << 4);
                ALUoutput = registerFile[operand1] + x;
                temp.push_back(ALUoutput);									
                temp.push_back(operand0);
                temp.push_back(operationCode - 8);
                MEMBuffer.push(temp);
                break;
        case 9: 
                // STORE
                x = operand2 - ((operand2 & 8) << 4);
                ALUoutput = registerFile[operand1] + x;
                temp.push_back(ALUoutput);									
                temp.push_back(operand0);
                temp.push_back(operationCode - 8);
                MEMBuffer.push(temp);
                break;
        case 10: 
                // JMP
                if(currentEX[4] == 1){
                    if(operationCode == 10)
                        value = (operand0 << 4) + operand1;
                    else
                        value = (operand1 << 4) + operand2;
                    if(value & 128) value -= 256;
                    PC += 2*value;
                }
                MEMBuffer.push(temp);
                break;
        case 11: 
                // BEQZ
                if(currentEX[4] == 1){
                    if(operationCode == 10)
                        value = (operand0 << 4) + operand1;
                    else
                        value = (operand1 << 4) + operand2;
                    if(value & 128) value -= 256;
                    PC += 2*value;
                }
                MEMBuffer.push(temp);
                break;
        case 15:
                // HLT
                MEMBuffer.push(temp);
                break;
        default: 
                cerr << "Bad Input!" << '\n'; 
                break;
    }

}

// Memory Access stage of pipeline
void MemoryAccess(){
    if(MEMBuffer.empty()) return;
	
    vector<int> memory = MEMBuffer.front();
    MEMBuffer.pop();

    vector<int> temp;
    // Push necessary data into the WB buffer
    if(memory.size() == 2)											
        temp = memory;
    else if(memory.size() == 3){
        if(memory[2] != 1){
            LMD = dataCache[memory[0]];
            temp.push_back(LMD);
            temp.push_back(memory[1]);
            temp.push_back(0);
        }
        else dataCache[memory[0]] = registerFile[memory[1]];															  
    }
    WBBuffer.push(temp);													
}

// Write Back stage of the pipeline
void WriteBack(){
    if(WBBuffer.empty()) return;
	
    // Extract the first entry in the WriteBack Buffer (queue)
    vi temp = WBBuffer.front();	
    WBBuffer.pop();

    // Write it to the memory
    if(temp.size() == 2 || temp.size() == 3){												
        registerFile[temp[1]] = temp[0];
        valid[temp[1]] = 1;
    }
}

// Control Unit, provides control path to the processor
void ControlUnit(){

    // Execute stages in reverse order (Pushing instruction forward in pipeline)
    WriteBack();
    MemoryAccess();
    Execute();
    InstructionDecode();

    if (controlHazard > 0) {controlHazard--; return;}
    if(dataHazard > 0){dataHazard--; return;}
    InstructionFetch();
    haltCheck();
}

// Check for program halt condition, set flag accordingly
void haltCheck(){
    if(!stop) return;
    flag = !(IFBuffer.empty() && IDBuffer.empty() && EXBuffer.empty() 
             && MEMBuffer.empty() && WBBuffer.empty());
}

// Main Function
int main()
{
    string temp;

    // Initialize arrays
    memset(instructionCache, 0, sizeof(instructionCache));
    memset(registerFile, 0, sizeof(registerFile));
    memset(dataCache, 0, sizeof(dataCache));

    for(int i=0; i<16; i++) valid[i] = true;

    // Read DCache.txt file & Store it in DataCache
	freopen("DCache.txt", "r", stdin);
	for(int i=0; i<256; i++){
		cin >> temp;
		int data = stoi(temp, 0, 16);
        if(data & 128) data -= 256;
		dataCache[i] = data;
	}
	fclose (stdin);

    // Read ICache.txt and store it in InstructionCache
    freopen("ICache.txt", "r", stdin);
	for(int i=0; i<256; i++){
		cin >> temp;
        int instruction = stoi(temp, 0, 16);
		instructionCache[i] = instruction;
	}
	fclose(stdin);

    // Read RF.txt and store it in Register File
    freopen("RF.txt", "r", stdin);
	for(int i=0; i<16; i++){
		cin >> temp;
		int registerValue = HexToInt(temp);
		if(registerValue & 128) registerValue -= 256;
		registerFile[i] = registerValue;
	}
	fclose(stdin);

    // Execute till flag remains true
    while(flag){		
        ControlUnit();					
		clockCycles++;
	}

    // Write to the output file DCacheOUT.txt
    freopen("DCacheOUT.txt", "w", stdout);
	for(int i = 0; i < 256; i++){
        int data = dataCache[i];
        data &= 255; 
        cout << IntToHex(data) << "\n";
    }
    fclose(stdout);
    
    // Print output to "Output.txt"
    freopen("Output.txt", "w", stdout);
    cout << "Total number of instructions executed: " << instructionCount << '\n';
    cout << "Number of instructions in each class" << '\n';
    cout << "Arithmetic instructions              : " << arithmeticInstructions << '\n';
    cout << "Logical instructions                 : " << logicalInstructions << '\n';
    cout << "Data instructions                    : " << dataInstructions << '\n';
    cout << "Control instructions                 : " << controlInstructions <<'\n';
    cout << "Halt instructions                    : " << halts << '\n';
    cout << "Cycles Per Instruction               : " << (double) clockCycles/instructionCount << '\n';
    cout << "Total number of stalls               : " << stalls << '\n';
    cout << "Data stalls (RAW)                    : " << dataStalls <<'\n';
    cout << "Control stalls                       : " << controlStalls << '\n';
    fclose(stdout);

    return 0;
}
