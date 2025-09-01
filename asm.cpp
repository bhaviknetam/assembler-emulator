// /*****************************************************************************
// TITLE: Assembler.cpp
// AUTHOR: Bhavik Netam
// ROLL NO: 2201CS84
// Declaration of Authorship
// This file is part of the assignment of CS210 at the
// department of Computer Science and Engineering, IIT Patna.
// *****************************************************************************/

#include <bits/stdc++.h>
#include <fstream>
using namespace std;

unordered_map<string, int> symtab;                          // Symbol Table containing labels.
unordered_map<int, int> PCtoLine;                           // Storing the PC value with respect to line number.
vector<pair<int, string>> errors;                           // Vector for storing errors in pass 1.
vector<pair<int, pair<string, string>>> Pass2_instructions; // Instructions which can send to Pass 2.
vector<string> hex_ins;                                     // Storing Hexadecimal Instructions for converting it to binary object code.
unordered_map<string, pair<int, int>> mnemonics = {
    {"data", {1, -1}},
    {"ldc", {1, 0}},
    {"adc", {1, 1}},
    {"ldl", {1, 2}},
    {"stl", {1, 3}},
    {"ldnl", {1, 4}},
    {"stnl", {1, 5}},
    {"add", {0, 6}},
    {"sub", {0, 7}},
    {"shl", {0, 8}},
    {"shr", {0, 9}},
    {"adj", {1, 10}},
    {"a2sp", {0, 11}},
    {"sp2a", {0, 12}},
    {"call", {1, 13}},
    {"return", {0, 14}},
    {"brz", {1, 15}},
    {"brlz", {1, 16}},
    {"br", {1, 17}},
    {"HALT", {0, 18}},
    {"SET", {1, -1}}};

// For removing spaces from front and back of the line.
string trim(string str)
{
    string ans = "";
    int n = str.size();
    int start = 0, end = str.size() - 1;
    for (int i = 0; i < n; i++)
    {
        if (str[i] != ' ')
        {
            start = i;
            break;
        }
    }
    for (int i = n - 1; i >= 0; i--)
    {
        if (str[i] != ' ')
        {
            end = i;
            break;
        }
    }
    for (int i = start; i <= end; i++)
    {
        ans.push_back(str[i]);
    }
    return ans;
}
// Function for removing comments from line, if any.
string RemoveComments(string line)
{
    int n = line.size();
    string ans = "";
    int end = n;
    for (int i = 0; i < n; i++)
    {
        if (line[i] == ';')
        {
            end = i;
            break;
        }
    }
    for (int i = 0; i < end; i++)
    {
        ans.push_back(line[i]);
    }
    return ans;
}

// Function returning if the line contains any label or not.
bool ContainsLabel(string line)
{
    return line.find(':') != string::npos;
}

// Return the label, if found.
string findLabel(string line)
{
    int end = line.size();
    string label = "";
    for (int i = 0; i < line.size(); i++)
    {
        if (line[i] == ':')
        {
            end = i;
            break;
        }
    }
    for (int i = 0; i < end; i++)
    {
        label.push_back(line[i]);
    }
    return label;
}

// Function for checking the bogus label name.
bool ifBogusLabel(string str)
{
    if (!isalpha(str[0]))
    {
        return true;
    }
    for (int i = 1; i < str.size(); i++)
    {
        if (!(isalpha(str[i]) || isdigit(str[i])))
        {
            return true;
        }
    }
    return false;
}

// Function for checking if the line contains an unused label.
bool isUnusedLabel(string str)
{
    return !str.empty() && str.back() == ':';
}

// Function for removing label for obtaining instructions.
string RemoveLabel(string str)
{
    int start = 0;
    string ans = "";
    int n = str.size();
    for (int i = 0; i < n; i++)
    {
        if (str[i] == ':')
        {
            start = i + 1;
            break;
        }
    }
    for (int i = start; i < n; i++)
    {
        ans.push_back(str[i]);
    }
    return ans;
}
// Function for obtaining mnemonics from the line.
string ExtractInstr(string str)
{
    int end = str.size();
    string ins = "";
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] == ' ')
        {
            end = i;
            break;
        }
    }
    for (int i = 0; i < end; i++)
    {
        ins.push_back(str[i]);
    }
    return ins;
}
// Function to check whether the mnemonic is bogus or not.
bool isBogusMnemonic(string str)
{
    return mnemonics.find(str) == mnemonics.end();
}
// Function for obtaining the operand from the line.
string ExtractOper(string line)
{
    int start = 0;
    int n = line.size();
    for (int i = 0; i < n; i++)
    {
        if (line[i] == ' ')
        {
            start = i + 1;
            break;
        }
    }
    string ans = "";
    if (start > 0)
    {
        for (int i = start; i < n; i++)
        {
            ans.push_back(line[i]);
        }
    }
    return ans;
}
// Return whether extra operand is present or not.
bool ExtraOper(string str, string instr)
{
    for (char c : str)
    {
        if (c == ' ')
            return true;
    }

    string word = "";
    int num = 0;

    // adding delimiter character at the end
    // of 'str'
    str = str + ',';
    int l = str.size();
    // traversing 'str' from left to right
    vector<string> substr_list;
    for (int i = 0; i < l; i++)
    {

        // if str[i] is not equal to the delimiter
        // character then accumulate it to 'word'
        if (str[i] != ',')
            word = word + str[i];

        else
        {
            if ((int)word.size() != 0)
                substr_list.push_back(word);
            word = "";
        }
    }
    if (substr_list.size() > mnemonics[instr].first)
        return true;
    return false;
}
// Function for returning the operand as a number in any representation.
int Num(const string &opr)
{
    const char *start_ptr = opr.c_str();
    char *end_ptr;
    int num;
    if (opr.length() > 1 && opr[0] == '0')
    {
        switch (opr[1])
        {
        case 'b': // Base 2
            num = strtol(start_ptr + 2, &end_ptr, 2);
            break;
        case 'x': // Base 16
            num = strtol(start_ptr + 2, &end_ptr, 16);
            break;
        default: // Base 8
            num = strtol(start_ptr, &end_ptr, 8);
        }
    }
    else
    { // Base 10
        num = strtol(start_ptr, &end_ptr, 10);
    }
    return num;
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
// Function to find if an opernand is a valid number.
bool isNum(string str)
{
    int start = 0;
    if (str[0] = '-')
    {
        start = 1;
    }
    if (str[1] == 'x')
    {
        start = 2;
    }
    for (int i = start; i < str.size(); i++)
    {
        char ch = str[i];
        if (!((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f')))
        {
            return false;
        }
    }
    return true;
}
// Function for conversion of hex to binary.
string hexToBinary(string input)
{
    /*Using stoul() to get the decimal value of the given String*/
    unsigned int x = stoul(input, nullptr, 16);
    /*Using bitset<16>(x).to_string() to get the binary String of given integer*/
    string result = bitset<16>(x).to_string();
    return result;
}
// First pass of assembler for detecting errors, if any, and storing instructions for Pass 2.
void FirstPass(ifstream &inFile)
{
    string line;
    int locctr = 0;
    int linenum = 0;
    while (getline(inFile, line))
    {
        linenum++;
        line = trim(line);
        line = RemoveComments(line);
        line = trim(line);
        string label = "";
        if (ContainsLabel(line))
        {
            label = findLabel(line);
            label = trim(label);
            if (ifBogusLabel(label))
            {
                errors.push_back({linenum, label + " :Bogus Labelname"});
                continue;
            }
            else if (isUnusedLabel(label))
            {
                errors.push_back({linenum, label + " :an unused label"});
                continue;
            }
            else if (symtab.find(label) != symtab.end())
            {
                errors.push_back({linenum, label + " :Duplicate Label"});
            }
            else
            {
                symtab[label] = locctr;
            }
        }
        if (line.empty() || line.back() == ':')
            continue;
        line = RemoveLabel(line);
        line = trim(line);
        string instr = ExtractInstr(line);
        instr = trim(instr);
        if (isBogusMnemonic(instr))
        {
            errors.push_back({linenum, instr + " :Bogus Mnemonic"});
        }
        else
        {
            string oper = ExtractOper(line);
            oper = trim(oper);
            if (!oper.empty() && mnemonics[instr].first == 0)
            {
                errors.push_back({linenum, instr + " " + oper + " :Unexpected Operand"});
                continue;
            }
            if (oper.empty() && mnemonics[instr].first == 1)
            {
                errors.push_back({linenum, instr + " " + oper + " :Missing Operand"});
                continue;
            }
            if (ExtraOper(oper, instr))
            {
                errors.push_back({linenum, instr + " " + oper + " :extra on end of line"});
                continue;
            }
            if ((oper[0] >= '0' and oper[0] <= '9') and !isNum(oper))
            {
                errors.push_back({linenum, instr + " " + oper + " :Not a number"});
                continue;
            }
            if (instr != "SET")
            {
                PCtoLine[locctr] = linenum;
            }
            else
            {
                int opr = Num(oper);
                symtab[label] = opr;
                continue;
            }
            Pass2_instructions.push_back({locctr, {instr, oper}});
            locctr++;
        }
    }
}
// Second Pass will be executed completely if program has no errors.
void SecondPass(ofstream &objFile, ofstream &outfile, ofstream &LogFile)
{
    if (!errors.empty())
    {
        for (auto &i : errors)
        {
            LogFile << "Error in line no.: " << i.first << " " << i.second << endl;
        }
        return;
    }
    for (auto &i : Pass2_instructions)
    {
        int pc = i.first;
        string ins = i.second.first;
        string oper = i.second.second;
        int opcode = mnemonics[ins].second;
        int opr;
        if (symtab.find(oper) != symtab.end()) // Search if it is a label or not.
        {
            opr = symtab[oper];
        }
        else if (isNum(oper)) // Return true, if operand is a number
        {
            opr = Num(oper);
        }
        else
        {
            LogFile << "Error in line no.: " << PCtoLine[pc] << " " << ins << " " << oper << " :No such label" << endl;
            return;
        }
        for (auto &sym : symtab)
        {
            if (sym.second == pc)
            {
                outfile << toHex(pc, 8) << "          " << sym.first << ":" << endl;
            }
        }
        if (ins == "data")
        {
            outfile << toHex(pc, 8) << " " << toHex(opr, 8) << " " << ins << " " << oper << endl; // Storing the instruction and data in listing file.
            // objFile << hexToBinary(toHex(opr, 6)) << hexToBinary(toHex(opcode, 2));
            hex_ins.push_back(toHex(opr, 8));
            continue;
        }
        unordered_set<int> offset = {13, 15, 16, 17};
        if (offset.find(opcode) != offset.end())
        {
            opr = opr - (pc + 1);
        }
        outfile << toHex(pc, 8) << " " << toHex(opr, 6) << toHex(opcode, 2) << " " << ins << " " << oper << endl;
        hex_ins.push_back(toHex(opr, 6) + toHex(opcode, 2));
        // objFile << hexToBinary(toHex(opr, 6)) << hexToBinary(toHex(opcode, 2));
    }
}

void Binary_MachineCode(ofstream &obj)
{
    string line;
    for (auto &line : hex_ins)
    {
        if (line.size() == 0)
            continue;
        for (int i = 0; i < line.size(); i++)
            line[i] = toupper(line[i]);
        unsigned long long y;

        auto &s = line;
        stringstream ss;
        ss << hex << s;
        ss >> y;
        static_cast<long long>(y);
        // Converting hexadecimal value to byte-sized char which machine reads.
        obj.write((const char *)&y, sizeof(unsigned));
    }
    obj.close();
}

int main(int argc, char *argv[])
{
    string file = (string)argv[1];
    string in_file = "";
    for (auto i : file)
    {
        if (i == '.')
            break;
        in_file.push_back(i);
    }
    // Assigning output files, the name as input files.
    string asmfile = in_file + ".asm",
           logfile = in_file + ".log",
           objfile = in_file + ".o",
           lstfile = in_file + ".lst";
    ifstream inFile(asmfile);
    if (!inFile.is_open())
    {
        cerr << "Error: Could not open input file." << endl;
        return 1;
    }

    FirstPass(inFile);
    inFile.close();

    ofstream outFile(lstfile);
    ofstream logFile(logfile);
    ofstream objFile(objfile, ios::out | ios::binary);

    SecondPass(objFile, outFile, logFile);
    Binary_MachineCode(objFile);

    outFile.close();
    logFile.close();
    objFile.close();

    return 0;
}
