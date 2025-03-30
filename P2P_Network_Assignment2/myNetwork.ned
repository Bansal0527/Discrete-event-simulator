
simple Server
{
    parameters:
//        @display("i=block/routing");

        int totalServers;
        int totalClients;
    gates:
        input in[];
        output out[];

    // You can access totalServers and totalClients in handleMessage method or other member functions
}

simple Client
{
    parameters:
        //        @display("i=block/routing");
        int totalServers;
        int totalClients;
    gates:
        input in[];
        output out[];

    // You can access totalServers and totalClients in handleMessage method or other member functions
}

network Net
{
    parameters:
        int numServers;
        int numClients;
        @display("bgb=504,357");
    submodules:
        s[numServers]: Server {
            //            @display("p=100,100");
            totalServers = parent.numServers; // Assign total number of servers to each server module
            totalClients = parent.numClients; // Assign total number of clients to each server module
        }
        c[numClients]: Client {
            //            @display("p=200,100");
            totalServers = parent.numServers; // Assign total number of servers to each client module
            totalClients = parent.numClients; // Assign total number of clients to each client module
        }
    connections:
        // Connect each server to each client
        for i=0..(numServers-1), for j=0..(numClients-1) {
            s[i].out++ --> {  delay = 10ms; } --> c[j].in++;
            s[i].in++ <--{  delay = 1000ms; } <-- c[j].out++;
        }

        // Connect each client to each other
        for i=0..(numClients-1), for j=0..(numClients-1) {
            //            if (i != j) { // Ensure not connecting a client to itself
            c[i].out++ --> {  delay = 10ms; } --> c[j].in++;
//            c[i].in++ <-- {  delay = 10ms; } <-- c[j].out++;
        }
}
