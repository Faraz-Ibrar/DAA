#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono> // For measuring time
#include <set>    // To ensure uniqueness of sequences

using namespace std;
using namespace std::chrono;

// Function to compute the LCS of two strings
int computeLCS(const string &a, const string &b) {
    int m = a.length();
    int n = b.length();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (a[i - 1] == b[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }
    return dp[m][n];
}

// Function to read sequences from the CSV file
vector<string> readCSV(const string &filename) {
    vector<string> sequences;
    set<string> uniqueSequences; // To avoid duplicates
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return sequences;
    }

    string line;
    while (getline(file, line)) {
        // Check if line starts with "Sequence"
        if (line.find("Sequence") == 0) {
            // Find the first space or tab after "Sequence x"
            size_t pos = line.find(' ');
            if (pos != string::npos) {
                string sequence = line.substr(pos); // Extract sequence
                sequence.erase(remove_if(sequence.begin(), sequence.end(), ::isspace), sequence.end());

                // Add to the list only if the sequence is non-empty and unique
                if (!sequence.empty() && uniqueSequences.insert(sequence).second) {
                    sequences.push_back(sequence);
                }
            }
        }
    }

    file.close();
    return sequences;
}

int main() {
    string filename;
    cout << "Enter the CSV file name: ";
    cin >> filename;

    // Read sequences from the CSV file
    vector<string> sequences = readCSV(filename);

    // Validate the number of sequences
    cout << "Total sequences read: " << sequences.size() << endl;
    if (sequences.size() < 2) {
        cerr << "Error: Not enough valid sequences found in the file." << endl;
        return 1;
    }

    // Open files to save results
    ofstream outputFile("pairwise_lcs_results.txt");
    ofstream csvFile("pairwise_lcs.csv");
    ofstream timeFile("pairwise_lcs_times.csv");

    // Add headers to the CSV files
    csvFile << "Sequence1,Sequence2,LCS_Length\n";
    timeFile << "Sequence1,Sequence2,Time_Seconds\n";

    if (!outputFile.is_open() || !csvFile.is_open() || !timeFile.is_open()) {
        cerr << "Error: Unable to open output files." << endl;
        return 1;
    }

    // Variables to track the maximum LCS
    int maxLCSLength = 0;
    size_t maxSeq1 = 0, maxSeq2 = 0;

    // Compute pairwise LCS and measure time
    cout << "Computing pairwise LCS...\n";
    for (size_t i = 0; i < sequences.size(); i++) {
        for (size_t j = i + 1; j < sequences.size(); j++) {
            // Measure start time
            auto start = high_resolution_clock::now();

            // Compute LCS
            int lcsLength = computeLCS(sequences[i], sequences[j]);

            // Measure end time
            auto end = high_resolution_clock::now();
            duration<double> elapsed = end - start;

            // Save results to files
            outputFile << "LCS(" << i + 1 << ", " << j + 1 << ") = " << lcsLength << "\n";
            csvFile << i + 1 << "," << j + 1 << "," << lcsLength << "\n";
            timeFile << i + 1 << "," << j + 1 << "," << elapsed.count() << "\n";

            // Update maximum LCS if the current one is greater
            if (lcsLength > maxLCSLength) {
                maxLCSLength = lcsLength;
                maxSeq1 = i;
                maxSeq2 = j;
            }
        }
    }

    // Output the maximum pairwise LCS
    cout << "\nMaximum Pairwise LCS:\n";
    cout << "LCS(" << maxSeq1 + 1 << ", " << maxSeq2 + 1 << ") = " << maxLCSLength << endl;
    cout << "Seq" << maxSeq1 + 1 << ": " << sequences[maxSeq1] << endl;
    cout << "Seq" << maxSeq2 + 1 << ": " << sequences[maxSeq2] << endl;

    outputFile.close();
    csvFile.close();
    timeFile.close();
    cout << "\nResults saved to pairwise_lcs_results.txt, pairwise_lcs.csv, and pairwise_lcs_times.csv." << endl;

    return 0;
}
