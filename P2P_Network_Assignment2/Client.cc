#include "Client.h"
using namespace std;
using namespace omnetpp;
#include <random>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <map>
#include <queue>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <set>

// Include Server_Client_Message definition
class Server_Client_Message:public cMessage
{
     public:
         int ans;
         int subtask_id;
         int subtask_num;
         int task_num;
};

Define_Module(Client);

bool first=true;
int counter=0;

int part=0;
int k=0;
vector<int>clientNo;
vector<int> Gateidx;
std::map<int, int> Clientid;
int task=1;
std::map<std::string, bool> receivedGossips; // To track received gossip messages

// Helper function to get current timestamp as string
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Helper function to log message to both console and file
void logMessage(const std::string& message, const std::string& filename) {
    EV << message << endl;
    cout << message << endl;
    
    std::ofstream outFile(filename, std::ios_base::app);
    if (outFile.is_open()) {
        outFile << message << std::endl;
        outFile.close();
    }
}

// Helper function to select random servers
vector<int> selectRandomServers(int totalServers, int count) {
    // Create a list of all server indices
    vector<int> allServers;
    for (int i = 0; i < totalServers; i++) {
        allServers.push_back(i);
    }
    
    // Shuffle the list
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(allServers.begin(), allServers.end(), g);
    
    // Take the first 'count' servers
    vector<int> selectedServers;
    for (int i = 0; i < count && i < allServers.size(); i++) {
        selectedServers.push_back(allServers[i]);
    }
    
    return selectedServers;
}

void Client::initialize()
{
    int n = par("totalServers").intValue();
    int m = par("totalClients").intValue();
    int clientIndex = getIndex();
    
    // Create or overwrite this client's output file
    std::string clientOutputFileName = "Client" + std::to_string(clientIndex) + ".txt";
    std::ofstream clientOutFile(clientOutputFileName, std::ios::trunc);
    if (clientOutFile.is_open()) {
        clientOutFile << "=== Client " << clientIndex << " Output Log ===" << std::endl;
        clientOutFile << "Simulation started at: " << simTime() << std::endl;
        clientOutFile << "==============================" << std::endl;
        clientOutFile.close();
        EV << "Created new client output file: " << clientOutputFileName << endl;
    } else {
        EV << "Error: Could not create client output file: " << clientOutputFileName << endl;
    }
    
    // Create or overwrite this client's gossip output file
    std::string gossipOutputFileName = "Client" + std::to_string(clientIndex) + "_gossip.txt";
    std::ofstream gossipOutFile(gossipOutputFileName, std::ios::trunc);
    if (gossipOutFile.is_open()) {
        gossipOutFile << "=== Client " << clientIndex << " Gossip Log ===" << std::endl;
        gossipOutFile << "Simulation started at: " << simTime() << std::endl;
        gossipOutFile << "==============================" << std::endl;
        gossipOutFile.close();
        EV << "Created new gossip output file: " << gossipOutputFileName << endl;
    } else {
        EV << "Error: Could not create gossip output file: " << gossipOutputFileName << endl;
    }
    
    // Initialize the result vector
    res.resize(n, 0);
    
    // Calculate how many servers to send each subtask to
    int serversPerSubtask = (n / 2) + 1;
    
    // Create an array of integers for the maximum element task
    std::vector<int> inputArray = {45, 22, 87, 34, 56, 12, 99, 23, 67, 78};
    
    // Ensure x/n >= 2 (where x is array size and n is number of servers)
    if (inputArray.size() / n < 2) {
        // Add more elements if needed
        while (inputArray.size() / n < 2) {
            inputArray.push_back(rand() % 100);
        }
    }

    // Log the start of the task
    std::string initMessage = "Client " + to_string(clientIndex) + " starting task 1: Find maximum element in array";
    logMessage(initMessage, clientOutputFileName);

    // Divide the array into parts for each server
    int partSize = inputArray.size() / n; // Calculate the size of each part
    int remainder = inputArray.size() % n;
    
    int startIdx = 0;
    for (int i = 0; i < n; i++) {
        // Calculate the end index for the current part
        int currentPartSize = partSize + (i < remainder ? 1 : 0);
        int endIdx = startIdx + currentPartSize;
        
        // Create a string representation of this part of the array
        std::stringstream ss;
        for (int j = startIdx; j < endIdx && j < inputArray.size(); j++) {
            ss << inputArray[j] << " ";
        }
        std::string partString = ss.str();
        
        // Log the subtask
        std::string subtaskMessage = "Client " + to_string(clientIndex) + " created subtask " + to_string(i) + ": " + partString;
        logMessage(subtaskMessage, clientOutputFileName);
        
        // Create a new message with the array part as the name
        cMessage *partMsg = new cMessage(partString.c_str());
        partMsg->setKind(i); // Use setKind to store the subtask ID
        
        // Select random servers for this subtask
        vector<int> randomServers = selectRandomServers(n, serversPerSubtask);
        
        // Send the subtask to the selected servers
        for (int serverId : randomServers) {
            std::string sendMessage = "Client " + to_string(clientIndex) + " sending subtask " + to_string(i) + " to server " + to_string(serverId);
            logMessage(sendMessage, clientOutputFileName);
            send(partMsg->dup(), "out", serverId);
        }
        
        delete partMsg; // Clean up the original message
        
        startIdx = endIdx;
    }
}

vector<int> findIndexesOfHighestK(const std::vector<int>& nums, int k) {
    // Create a vector of pairs to store the value-index pairs
    vector<pair<int, int>> valueIndexPairs(nums.size());

    // Populate the vector with value-index pairs
    for (int i = 0; i < nums.size(); ++i) {
        valueIndexPairs[i] = std::make_pair(nums[i], i);
    }

    // Sort the vector of pairs based on values in descending order
    std::sort(valueIndexPairs.begin(), valueIndexPairs.end(), std::greater<std::pair<int, int>>());

    // Extract the indexes of the highest k values
    std::vector<int> indexes;
    for (int i = 0; i < k && i < valueIndexPairs.size(); ++i) {
        indexes.push_back(valueIndexPairs[i].second);
    }

    return indexes;
}

void Client::handleMessage(cMessage *msg)
{
    int n = par("totalServers").intValue();
    int m = par("totalClients").intValue();
    int clientIndex = getIndex(); // Get the client's index
    
    // Check if the message is from a server (Server_Client_Message)
    Server_Client_Message *serverMsg = dynamic_cast<Server_Client_Message *>(msg);
    
    // Check if the message is from another client (ClientMessage)
    ClientMessage *clientMsg = dynamic_cast<ClientMessage *>(msg);
    
    if (clientMsg) {
        // This is a gossip message from another client
        vector<int> NEW = clientMsg->arr;
        int senderIndex = clientMsg->ID; // This should be the sender's index, not ID
        time_t t = clientMsg->time;

        // Format the timestamp
        std::stringstream ss;
        char buffer[80]; // Buffer to store the formatted time
        struct tm* timeinfo = localtime(&t); // Convert time_t to tm structure
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        std::string strTime = ss.str();

        // Create a unique key for this gossip message
        string gossipKey = to_string(senderIndex) + "_" + strTime;
        
        // Check if we've already received this gossip
        if (receivedGossips.find(gossipKey) == receivedGossips.end()) {
            // This is a new gossip message
            receivedGossips[gossipKey] = true;
            
            // Format the gossip message
            string gossipMessage = "GOSSIP [" + getCurrentTimestamp() + "] from Client " + to_string(senderIndex) + ": Server scores: ";
            for (auto score : NEW) {
                gossipMessage += to_string(score) + " ";
            }
            
            // Log the gossip message
            logMessage(gossipMessage, "Client" + to_string(clientIndex) + "_gossip.txt");
            
            // Update our scores based on the gossip
            if (res.size() == 0) {
                res = NEW;
            } else {
                for (int i = 0; i < n && i < NEW.size(); i++) {
                    res[i] += NEW[i];
                }
            }
            
            // Forward the gossip to other clients (except the one we got it from)
            for (int i = 0; i < m; i++) {
                if (i != senderIndex && i != clientIndex) {
                    ClientMessage *fwdMsg = new ClientMessage();
                    fwdMsg->arr = NEW;
                    fwdMsg->ID = senderIndex;
                    fwdMsg->time = t;
                    fwdMsg->arr_len = NEW.size();
                    send(fwdMsg, "out", n + i); // Send to client at index i
                    
                    std::string gossipSendMessage = "Client " + to_string(clientIndex) + " gossiping scores to client " + to_string(i);
                    logMessage(gossipSendMessage, "Client" + to_string(clientIndex) + ".txt");
                }
            }
            
            // If this is task 1 completion, start task 2
            if (task == 1) {
                task = 2;
                
                // Log the start of task 2
                std::string task2Message = "Client " + to_string(clientIndex) + " starting task 2: Find maximum element in array (using trusted servers)";
                logMessage(task2Message, "Client" + to_string(clientIndex) + ".txt");
                
                // Normalize scores
                vector<int> score(n, 0);
                for (int i = 0; i < n; i++) {
                    score[i] += NEW[i];
                    if (res.size() > i) {
                        score[i] += res[i];
                    }
                }
                
                // Create a new array for the second task
                std::vector<int> secondTaskArray = {67, 42, 101, 55, 88, 33, 77, 22, 91, 44, 66, 11, 95, 30, 82};
                
                // Find the top-performing servers
                vector<int> topserver = findIndexesOfHighestK(score, (n/2)+1);
                
                // Log the selected servers
                std::string serverSelectionMsg = "Client " + to_string(clientIndex) + " selected servers for task 2: ";
                for (auto server : topserver) {
                    serverSelectionMsg += to_string(server) + " ";
                }
                logMessage(serverSelectionMsg, "Client" + to_string(clientIndex) + ".txt");
                
                // Divide the array into parts for each server
                int partSize = secondTaskArray.size() / n; // Calculate the size of each part
                int remainder = secondTaskArray.size() % n;
                
                int startIdx = 0;
                for (int i = 0; i < n; i++) {
                    // Calculate the end index for the current part
                    int currentPartSize = partSize + (i < remainder ? 1 : 0);
                    int endIdx = startIdx + currentPartSize;
                    
                    // Create a string representation of this part of the array
                    std::stringstream ss;
                    for (int j = startIdx; j < endIdx && j < secondTaskArray.size(); j++) {
                        ss << secondTaskArray[j] << " ";
                    }
                    std::string partString = ss.str();
                    
                    // Log the subtask
                    std::string subtaskMessage = "Client " + to_string(clientIndex) + " created subtask " + to_string(i) + " for task 2: " + partString;
                    logMessage(subtaskMessage, "Client" + to_string(clientIndex) + ".txt");
                    
                    // Create a new message with the array part as the name
                    cMessage *partMsg = new cMessage(partString.c_str());
                    partMsg->setKind(i + 100); // Use a different range for task 2
                    
                    // Send the subtask to the top-performing servers
                    for (int serverIdx : topserver) {
                        std::string sendMessage = "Client " + to_string(clientIndex) + " sending subtask " + to_string(i) + " of task 2 to server " + to_string(serverIdx);
                        logMessage(sendMessage, "Client" + to_string(clientIndex) + ".txt");
                        send(partMsg->dup(), "out", serverIdx);
                    }
                    
                    delete partMsg; // Clean up the original message
                    
                    startIdx = endIdx;
                }
            }
        }
    }
    else if (serverMsg) {
        // Handle message from server
        int serverIndex = msg->getArrivalGate()->getIndex();
        int maxValue = serverMsg->ans;
        int subtaskId = serverMsg->subtask_id;
        int subtaskNum = serverMsg->subtask_num;
        
        // Log the received result
        std::string resultMessage = "Client " + to_string(clientIndex) + " received result " + to_string(maxValue) + 
                                   " for subtask " + to_string(subtaskNum) + " from server " + to_string(serverIndex);
        logMessage(resultMessage, "Client" + to_string(clientIndex) + ".txt");
        
        // Update the score for this server
        if (res.size() == 0) {
            res.resize(n, 0);
        }
        
        // Increment the score for this server
        res[serverIndex]++;
        
        server_mutex++;
        if(server_mutex == n * ((n/2) + 1)) {
            server_mutex = 0;
            
            // Log the consolidated scores
            std::string scoreMessage = "Client " + to_string(clientIndex) + " consolidated server scores: ";
            for(int j=0; j<n; j++) {
                scoreMessage += to_string(res[j]) + " ";
                EV << res[j] << " ";
            }
            logMessage(scoreMessage, "Client" + to_string(clientIndex) + ".txt");
            
            // Gossip the scores to other clients
            for(int i=0; i<m; i++) {
                if (i != clientIndex) {
                    ClientMessage *gossipMsg = new ClientMessage();
                    gossipMsg->arr = res;
                    gossipMsg->ID = clientIndex;
                    gossipMsg->time = time(nullptr);
                    gossipMsg->arr_len = res.size();
                    send(gossipMsg, "out", n + i); // Send to client at index i
                    
                    std::string gossipSendMessage = "Client " + to_string(clientIndex) + " gossiping scores to client " + to_string(i);
                    logMessage(gossipSendMessage, "Client" + to_string(clientIndex) + ".txt");
                }
            }
            
            EV << "\n";
            EV << clientIndex;
            EV << "\n";
        }
    }
    else {
        // Handle other types of messages (regular cMessage)
        int serverIndex = msg->getArrivalGate()->getIndex();
        int subtaskId = msg->getKind(); // Get the subtask ID from the message kind

        // Log the received message
        std::string recvMessage = "Client " + to_string(clientIndex) + " received message from server " + 
                                 to_string(serverIndex) + ": " + msg->getName();
        logMessage(recvMessage, "Client" + to_string(clientIndex) + ".txt");

        // Update scores
        if(res.size() == 0) {
            res.resize(n, 0);
        }
        res[serverIndex]++;

        string ress = " ";
        for(auto i : res) {
            ress += to_string(i);
            ress += " ";
        }

        cout << "Client " << clientIndex << " Receives " << msg->getName() << " from server " 
             << serverIndex << ress << "\n";

        string output = "Client" + to_string(clientIndex) + ".txt";
        std::ofstream outFile(output, std::ios_base::app);
        if (outFile.is_open()) {
            // Append a new line to the file
            string s = "Client " + to_string(clientIndex) + " Receives " + msg->getName() + 
                      " from server " + to_string(serverIndex) + ress;
            outFile << s << std::endl;
            outFile.close();
        }
    }
    
    delete msg;
}