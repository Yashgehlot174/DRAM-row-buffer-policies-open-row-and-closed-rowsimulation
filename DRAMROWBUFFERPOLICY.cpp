#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;
// Class to store DRAM parameters
class dram_parameters {
public:
    long rbhl;           // ROWBUF_HIT_LATENCY
    long rbml;           // ROWBUF_MISS_LATENCY
    long rbcl;           // ROWBUF_CONFLICT_LATENCY
    long no_rows;        // ROWS_IN_BANK
    long no_columns;     // COLUMNS_IN_ROW
};
// Class to store row and column access information
class row_and_column_access {
public:
    long rowID;
    long columnID;
    long timeOfArrival;
};
int main(int argc, char* argv[]) 
{
    string in_name = argv[1];
    ifstream inputFile(in_name);
    // Initialize DRAM parameters and access vector
    dram_parameters p;
    vector<row_and_column_access> acc;
    string line;
    // Read input file line by line
    while (getline(inputFile, line)) 
    {
        // Skip empty lines or lines starting with '/'
        if (line.empty() || line[0] == '/') {
            continue;
        }
        // Process each line based on the first parameter
        istringstream iss(line);
        string param;
        iss >> param;
        if (param == "ROWBUF_HIT_LATENCY") 
        {
            iss >> p.rbhl;
        } else if (param == "ROWBUF_MISS_LATENCY") {
            iss >> p.rbml;
        } else if (param == "ROWBUF_CONFLICT_LATENCY") {
            iss >> p.rbcl;
        } else if (param == "ROWS_IN_BANK") {
            iss >> p.no_rows;
        } else if (param == "COLUMNS_IN_ROW") {
            iss >> p.no_columns;
        } else 
        {
            // Process row and column access information
            long rowID, columnID, timeOfArrival;
            char comma;
            istringstream iss(param);
            iss >> rowID >> comma >> columnID >> comma >> timeOfArrival;
            acc.push_back({rowID, columnID, timeOfArrival});
        }
    }
    // Initialize variables for tracking timing information
    long o_r_c_t = 0;
    long c_r_c_t = 0;
    long c_r = -1;
    long i = 0;
    long CAS = p.rbhl;
    long RAS = p.rbml - p.rbhl;
    long PRE = p.rbcl - CAS - RAS;
    // Create output file
    string out_name = "22116109_DRAM_Parameters_Accesses.txt";
    ofstream outputFile(out_name);
    // Write header to the output file
    outputFile << "Row,Column  " << setw(13) << "TimeOfArrival" << setw(15) << "OpenRow" << setw(10) << "CloseRow" << endl;
    // Process each row and column access
    for (const row_and_column_access& access : acc) 
    {
        long accessTime = access.timeOfArrival;
        // Initialize timing information for the first access
        if(i == 0)
        {
            o_r_c_t = RAS + CAS + accessTime;
            c_r_c_t = RAS + CAS + accessTime;
            c_r = access.rowID;
            i++;
        }
        // Update timing information based on the current row
        else if (access.rowID == c_r) 
        {
            o_r_c_t = max(o_r_c_t + CAS, accessTime + CAS);
            c_r_c_t = max(c_r_c_t + PRE + CAS + RAS, accessTime + CAS + RAS);
        } 
        // Update timing information for a new row
        else
        {
            o_r_c_t = max(o_r_c_t + PRE + RAS + CAS, accessTime + PRE + RAS + CAS);
            c_r_c_t = max(c_r_c_t + PRE + CAS + RAS, accessTime  + CAS + RAS);
            c_r = access.rowID;
        }
        // Write timing information to the output file
        outputFile << access.rowID << "," << access.columnID << setw(13) << accessTime << setw(19) << o_r_c_t << setw(13) << c_r_c_t << endl;
    }
    return 0;
}
