Overview of the Problem:
To mimic remote program execution, use the OMNeT++ Discrete Event Simulator. Each job is split up into n subtasks and delivered to n/2 + 1 servers. The majority's response will be regarded as legitimate. 
Server is requested to carry out a basic operation, such as determining the number of vowels in the string. The client splits string into about n equal portions, where x/n ≥ 2.

Implementation:
First we created a network architecture made up of clients and servers that are connected to one another to enable communication. Two simple modules Client and Server are initiated. The server module is configured
with parameters like ‘totalServers’ and ‘totalClients’. And the client module has an additional parameter ‘res’ which stores the consolidated scores.

Code Explanation:
1) Server.cc : This file contains declaration of the Server module. Server will perform task of
counting vowels in the input string sent by the Client. Server may or may not respond
honestly. Server will act maliciously randomly, we have implemented the logic when treeId%4 of
message==serverId%4,it will act maliciously.

3) Client.cc : This file contains declaration of the Client module. Firstly client will divide task
into totalServers subtasks and then it will send the subtasks to totalServers / 2 +
1 selected server. For Task 1, these servers will be selected randomly. For later tasks
servers will be selected on the basis of scores assigned by clients calculated through
implementation if previous tasks,giving zero for wrong and 1 for correct answer.
Clients will also communicate these scores with each other. After this we execute the program
again for second task.

Steps to execute the program:
1) Open the project in Omnetpp
2) Build and Execute the simulation in Omnetpp environment.
2) Set Values of variables numServers and numClients in omnet.ini file

