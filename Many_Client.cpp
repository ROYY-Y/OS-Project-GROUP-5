#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void client_sim(int id) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(3000);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    if(connect(sock, (sockaddr*)&server, sizeof(server)) < 0){
        std::cerr << "Client " << id << " failed to connect\n";
        return;
    }

    char buff[1024];
    int n = recv(sock, buff, sizeof(buff)-1, 0);
    if(n > 0){
        buff[n] = '\0';
    }
}

int main(){
    const int NUM_CLIENTS = 1000;
    std::vector<std::thread> threads;
    for(int i=0; i<NUM_CLIENTS; ++i){
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        threads.emplace_back(client_sim, i+1);
    }
    for(auto &t : threads) t.join();
}
