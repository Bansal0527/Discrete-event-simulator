#B22CS070 B22CS064
# Project Description
### Link to repo - https://github.com/Bansal0527/Discrete-event-simulator
This project implements a distributed task execution simulation using the **OMNeT++ Discrete Event Simulator**. The simulation models a network of client and server nodes where clients divide a task (e.g., finding the maximum element in an array) into subtasks and distribute them to a subset of servers. The system incorporates **fault tolerance** by considering the majority result as valid and includes a **gossip protocol** for clients to share server performance scores.

## Key Features

### Network Setup
- **Server Nodes**: Execute subtasks (e.g., finding the maximum element in a sub-array). Servers can act **honestly** (returning correct results) or **maliciously** (returning incorrect results). At most **n/4** servers behave maliciously for any task.
- **Client Nodes**: Divide tasks into **n subtasks**, send each to **n/2 + 1** servers, aggregate results based on **majority voting**, and compute a final result. Clients also assign scores to servers and **gossip these scores** to other clients.

### Task Execution
- The initial task is to **find the maximum element** in an array of integers.
- The array is divided into **n roughly equal parts** (each with at least 2 elements), and each part is sent to **n/2 + 1 randomly selected servers**.
- After the first round, clients use **gossip-derived server scores** to select the top **n/2 + 1 servers** for a second task round.

### Gossip Protocol
- Clients broadcast **server scores** after each task using a gossip message format: <self.timestamp>:<self.IP>:<self.Score#>

- Messages are **forwarded to all peers** except the sender.
- A mechanism prevents redundant forwarding using a **message log (ML)**.

### Output
- Servers log their **subtask results** to both the console and a file (**serverOutput.txt**).
- Clients log **subtask results, consolidated task results, and gossip messages** to the console and individual files (**ClientX.txt** and **ClientX_gossip.txt**).

### Topology
- The **network topology** is maintained in a separate file (**topo.txt**), allowing **dynamic adjustment** of the number of servers (**n**) and clients (**m**).

## Steps to Execute the Program
1. Open the project in **OMNeT++**.
2. Build and execute the simulation in the **OMNeT++ environment**.
3. Set values of variables **numServers** and **numClients** in the **omnet.ini** file.
