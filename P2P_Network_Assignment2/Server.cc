#include "Server.h"
#include<string>
#include <random>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <sstream>
#include <vector>
#include <climits>
using namespace std;
using namespace omnetpp;

// Define the Server_Client_Message class if not already defined
class Server_Client_Message:public cMessage
{
     public:
         int ans;
         int subtask_id;
         int subtask_num;
         int task_num;
};

Define_Module(Server);

void Server::initialize()
{
    // Get the server index (position in the array)
    int serverIndex = getIndex();
    
    // Create or overwrite the server output file
    std::string outputFileName = "serverOutput.txt";
    
    // If this is server 0, clear the file (overwrite it)
    if (serverIndex == 0) {
        std::ofstream outFile(outputFileName, std::ios::trunc);
        if (outFile.is_open()) {
            outFile << "=== Server Output Log ===" << std::endl;
            outFile << "Simulation started at: " << simTime() << std::endl;
            outFile << "==============================" << std::endl;
            outFile.close();
            EV << "Created new server output file: " << outputFileName << endl;
        } else {
            EV << "Error: Could not create server output file: " << outputFileName << endl;
        }
    }
    
    // Log initialization message
    std::string initMessage = "Server " + std::to_string(serverIndex) + " initialized";
    EV << initMessage << endl;
    
    // Log to the server output file
    std::ofstream outFile(outputFileName, std::ios::app);
    if (outFile.is_open()) {
        outFile << initMessage << std::endl;
        outFile.close();
    }
}

// Helper function to log message to serverOutput.txt
void logToServerOutput(const std::string& message) {
    std::ofstream outFile("serverOutput.txt", std::ios_base::app);
    if (outFile.is_open()) {
        outFile << message << std::endl;
        outFile.close();
        std::cout << "Server output logged to file successfully." << std::endl;
    } else {
        std::cerr << "Failed to open serverOutput.txt for writing!" << std::endl;
    }
}

void Server::handleMessage(cMessage *msg)
{
    int n = par("totalServers").intValue();
    int serverIndex = getIndex(); // Get the server's index (0, 1, 2, ...)
    
    // Seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Parse the array from the message name
    std::string msgName = msg->getName();
    std::stringstream ss(msgName);
    std::vector<int> numbers;
    int num;
    
    while (ss >> num) {
        numbers.push_back(num);
    }
    
    // Find the maximum element in the array
    int maxElement = INT_MIN;
    if (!numbers.empty()) {
        for (int val : numbers) {
            maxElement = std::max(maxElement, val);
        }
    }
    
    int subtaskId = msg->getKind(); // Get the subtask ID from the message kind
    
    // Create a Server_Client_Message to send back to the client
    Server_Client_Message *resultMsg = new Server_Client_Message();
    
    // Determine if this server should be malicious based on index
    bool isMalicious = (serverIndex % 4 == subtaskId % 4);
    
    if(isMalicious) {
        // Malicious behavior - return incorrect max value
        resultMsg->ans = maxElement - 10; // Deliberately incorrect
        resultMsg->subtask_id = subtaskId;
        resultMsg->subtask_num = subtaskId; // Use subtaskId for subtask_num
        resultMsg->task_num = (subtaskId >= 100) ? 2 : 1; // Task 2 if subtaskId >= 100

        // Log the result to console and file
        std::string logMessage = "Malicious Node Server " + to_string(serverIndex) + 
                                " Output " + to_string(resultMsg->ans) + 
                                " on subtask " + msgName + 
                                " (SubtaskID: " + to_string(subtaskId) + ")";
        
        EV << logMessage << endl;
        cout << logMessage << endl;
        
        // Log to serverOutput.txt
        std::ofstream outFile("serverOutput.txt", std::ios_base::app);
        if (outFile.is_open()) {
            outFile << logMessage << std::endl;
            outFile.close();
        }
        
        for (int i = 0; i < gateSize("out"); i++) {
            send(resultMsg->dup(), "out", i);
        }
    }
    else {
        // Honest behavior - return correct max value
        resultMsg->ans = maxElement;
        resultMsg->subtask_id = subtaskId;
        resultMsg->subtask_num = subtaskId; // Use subtaskId for subtask_num
        resultMsg->task_num = (subtaskId >= 100) ? 2 : 1; // Task 2 if subtaskId >= 100
        
        // Log the result to console and file
        std::string logMessage = "Honest Node Server " + to_string(serverIndex) + 
                               " Output " + to_string(resultMsg->ans) + 
                               " on subtask " + msgName + 
                               " (SubtaskID: " + to_string(subtaskId) + ")";
        
        EV << logMessage << endl;
        cout << logMessage << endl;
        
        // Log to serverOutput.txt
        std::ofstream outFile("serverOutput.txt", std::ios_base::app);
        if (outFile.is_open()) {
            outFile << logMessage << std::endl;
            outFile.close();
        }
        
        for (int i = 0; i < gateSize("out"); i++) {
            send(resultMsg->dup(), "out", i);
        }
    }

    delete msg;
}
