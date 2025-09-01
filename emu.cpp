// /*****************************************************************************
// TITLE: Emulator.cpp
// AUTHOR: Bhavik Netam
// ROLL NO: 2201CS84
// Declaration of Authorship
// This file is part of the assignment of CS210 at the
// department of Computer Science and Engineering, IIT Patna.
// *****************************************************************************/
#include <bits/stdc++.h>
using namespace std;
const int MEM_SIZE = 1 << 24;
// Declaring memory size, and intializing Program Counter and registers
int Memory[MEM_SIZE];
int PC = 0, RegisterA = 0, RegisterB = 0, StackPtr = 0;
int totalInstructions = 0;
int loop_limit = 100000;
int StackLimit = 1 << 23;
// Vector for storing mnemonics
vector<string> mnemonics = {"ldc", "adc", "ldl", "stl", "ldnl", "stnl", "add", "sub", "shl", "shr", "adj", "a2sp", "sp2a", "call", "return", "brz", "brlz", "br", "HALT", "data", "SET"};

// Function to read machine code from file and load into memory
void loadMachineCode(const string &filename, vector<int> &machineCode)
{
    ifstream file(filename, ios::in | ios::binary);
    if (!file)
    {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }
    int instruction;
    while (file.read((char *)&instruction, sizeof(int)))
    {
        machineCode.push_back(instruction);
    }

    file.close();
}
// Executing instruction according to given Opcode
void executeInstruction(int Opcode, int Operand)
{
    switch (Opcode)
    {
    case 0:
        RegisterB = RegisterA;
        RegisterA = Operand;
        break;
    case 1:
        RegisterA += Operand;
        break;
    case 2:
        RegisterB = RegisterA;
        RegisterA = Memory[StackPtr + Operand];
        break;
    case 3:
        Memory[StackPtr + Operand] = RegisterA;
        RegisterA = RegisterB;
        break;
    case 4:
        RegisterA = Memory[RegisterA + Operand];
        break;
    case 5:
        Memory[RegisterA + Operand] = RegisterB;
        break;
    case 6:
        RegisterA = RegisterB + RegisterA;
        break;
    case 7:
        RegisterA = RegisterB - RegisterA;
        break;
    case 8:
        RegisterA = RegisterB << RegisterA;
        break;
    case 9:
        RegisterA = RegisterB >> RegisterA;
        break;
    case 10:
        StackPtr = StackPtr + Operand;
        break;
    case 11:
        StackPtr = RegisterA;
        RegisterA = RegisterB;
        break;
    case 12:
        RegisterB = RegisterA;
        RegisterA = StackPtr;
        break;
    case 13:
        RegisterB = RegisterA;
        RegisterA = PC;
        PC = Operand + PC;
        break;
    case 14:
        PC = RegisterA;
        RegisterA = RegisterB;
        break;
    case 15:
        if (RegisterA == 0)
        {
            PC = PC + Operand;
        }
        break;
    case 16:
        if (RegisterA < 0)
        {
            PC = PC + Operand;
        }
        break;
    case 17:
        PC = PC + Operand;
        break;
    case 18:
        printf("A = %08X, B = %08X, PC = %08X, SP = %08X\n", RegisterA, RegisterB, PC, StackPtr);
        cout << "Number of instructions passed: " << totalInstructions << endl;
        exit(0);
        break;
    default:
        cout << "Invalid opcode detected. Please check the machine codes.";
        exit(0);
        break;
    }
}
// Function for conversion of numbers to hexadecimal representation.
string toHex(int num, int fill)
{
    ostringstream oss;
    oss << setfill('0') << setw(fill) << hex << num;
    string ans = oss.str();
    if (fill < ans.length())
    {
        ans = ans.substr(ans.length() - fill, fill);
    }
    return ans;
}
// Function for returning a hex Operand
long long readOperand(const string &operand)
{
    // Check for empty string
    if (operand.empty())
    {
        return 0;
    }

    // Use string literal for conversion
    char *end;
    long long num;
    num = strtol(operand.c_str(), &end, 0); // Base 0 lets strtol choose

    // Check for successful conversion and non-empty characters after
    return (*end == '\0') ? num : -1;
}
// Emulator program which detects how the program is working.
bool Emulator(vector<int> &machineCode, ofstream &outFile)
{
    if (PC > machineCode.size())
    {
        cout << "Segmentation fault.\n";
        exit(1); // Accessing other address greater than program counter may occur segmentation fault.
    }
    int opCode = machineCode[PC] & 0xFF;
    int operand = machineCode[PC] >> 8;
    if (opCode == 18)
    {
        totalInstructions++;
        cout << "HALT instruction detected.\n";
        return false;
    }
    cout << mnemonics[opCode] << "\t";
    printf("%08X\n", operand);
    executeInstruction(opCode, operand);
    printf("A = %08X, B = %08X, PC = %08X, SP = %08X\n", RegisterA, RegisterB, PC, StackPtr);
    // Printing the values of respective registers.
    outFile << "A = " << toHex(RegisterA, 8) << " B = "
            << toHex(RegisterB, 8) << " PC = "
            << toHex(PC, 8) << " SP = "
            << toHex(StackPtr, 8) << endl;
    totalInstructions++;
    PC++;
    if (totalInstructions > loop_limit)
    {
        cout << "Infinite loop detected.\n"
             << endl;
        exit(1);
    }
    if (StackLimit < StackPtr)
    {
        cout << "Stack Overflow\n"
             << endl;
        exit(1);
    }
    return true;
}
// Main function taking input file as an argument in command line.
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Error: Input valid file name" << endl;
        return 1;
    }
    string n_file = (string)argv[1];
    string in_file = "";
    for (auto i : n_file)
    {
        if (i == '.')
            break;
        in_file.push_back(i);
    }

    string outfile = in_file + ".txt";
    ofstream out(outfile);
    vector<int> machineCode; // Vector for storing instructions from object code.
    string inFile = argv[1];
    loadMachineCode(inFile, machineCode);
    cout << "Output one instruction: -one\n"
         << "Output all instructions: -all\n"
         << "See Memory dump: -dump\n"
         << "Exit: -exit\n";
    int i = 0;
    for (int &code : machineCode) // Storing the instructions and values in memory.
    {
        Memory[i++] = code;
        // out << code << endl;
    }
    while (true)
    {
        cout << "Enter the input in given format: ";
        string input;
        cin >> input;
        for (int i = 0; i < input.size(); i++)
        {
            input[i] = tolower(input[i]);
        }
        if (input == "-one")
        {
            if (Emulator(machineCode, out) == 0)
                ;
        }
        else if (input == "-all")
        {
            if (Emulator(machineCode, out) == 0)
                ;
            else
            {
                while (Emulator(machineCode, out) == 1)
                    ;
            }
        }
        else if (input == "-dump")
        {
            cout << "Base address: "; // Starting address from which value needs to be dump.
            string in, val;
            cin >> in;
            long long BaseAddress = readOperand(in);
            cout << "No. of values: ";
            cin >> val; // Number of values after base address which will dump.
            long long value = readOperand(val);
            long long address = BaseAddress;
            while (address < BaseAddress + value && address < (1LL << 24))
            {
                printf("%08llX \t %08X\n", address, Memory[address]);
                address++;
            }
        }
        else if (input == "-exit") // Exit Call
        {
            break;
        }
        else
        {
            cout << "Invalid emulator input." << endl;
        }
    }
    cout << "Total number of Instructions passed: " << totalInstructions << endl;
    return 0;
}