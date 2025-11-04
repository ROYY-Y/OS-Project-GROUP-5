#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <memory>

struct Client {
    int sockfd;
    std::queue<std::string> mail;
    int id;
    std::string currRoom;
};

struct Room {
    std::unordered_map<int, std::shared_ptr<Client>> member;
};

std::unordered_map<std::string, std::shared_ptr<Room>> Rooms;
int next_Clients_Id = 1;

void server_Check(){
    std::string command;
    while(true){
        std::getline(std::cin, command);
        if(command == "trm"){
            std::cout << "Total Room: "<< Rooms.size() << std::endl;
        }else if(command == "lsrm"){
            std::cout << "Rooms list:" << std::endl;
            for(auto &[name,room] : Rooms){
                std::cout << name << " " << room->member.size() << " online"<< std::endl;
            }
            std::cout <<"Total Room:"  << Rooms.size() << std::endl;
        }else std::cout << "Command not found!!" << std::endl;
    }
}

void send_Thread(std::shared_ptr<Client> c) {
    while (true) {
        if (c->mail.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        std::string msg = c->mail.front();
        c->mail.pop();

        //std::this_thread::sleep_for(std::chrono::milliseconds(100));

        size_t total_send = 0;
        while (total_send < msg.size()) {
            int n = send(c->sockfd, msg.c_str() + total_send, msg.size() - total_send, 0);
            if (n <= 0) return;
            total_send += n;
        }
    }
}

void addMail(std::shared_ptr<Client> c, std::string msg) {
    c->mail.push(msg);
}

void addMember(std::shared_ptr<Client> c, std::string rName) {
    Rooms[rName]->member[c->id] = c;
    c->currRoom = rName;
}

void showJoined(std::shared_ptr<Client> c, std::string rName) {
    for (auto &[id, cl] : Rooms[rName]->member) {
        if (id == c->id) continue;
        addMail(cl, "[Server] :Client " + std::to_string(c->id) + " joined " + rName + " room ...");
    }
}

void removeMember(std::shared_ptr<Client> c, std::string rName) {
    Rooms[rName]->member.erase(c->id);
    for (auto &[id, cl] : Rooms[rName]->member) {
        addMail(cl, "[Server] :Client " + std::to_string(c->id) + " disconnected ...");
    }

    if (rName != "Lobby" && Rooms[rName]->member.empty()) {
        Rooms.erase(rName);
    }
}

void client_Thread(std::shared_ptr<Client> c) {
    char buff[1024];
    while (true) {
        int n = recv(c->sockfd, buff, sizeof(buff) - 1, 0);
        if (n <= 0) break;
        buff[n] = '\0';
        std::string msg(buff);

        std::cout << "From client " << c->id << " (" << c->currRoom << "): " << msg << std::endl;

        if (msg.rfind("JOIN ", 0) == 0) {
            std::string rName = msg.substr(5);
            addMail(c, "[Server] :JOIN " + rName + " Successfull");
            removeMember(c, c->currRoom);

            if (Rooms.count(rName) == 0)
                Rooms[rName] = std::make_shared<Room>();
            addMember(c, rName);
            showJoined(c, rName);
        }
        else if (msg == "LEAVE") {
            if (c->currRoom == "Lobby") {
                addMail(c, "[Server] :Already in Lobby");
                continue;
            }
            removeMember(c, c->currRoom);
            addMember(c, "Lobby");
            addMail(c, "[Server] :Return to Lobby ...");
            showJoined(c, "Lobby");
        }
        else if (msg.rfind("SAY ", 0) == 0) {
            std::string sendMsg = msg.substr(4);
            for (auto &[id, cl] : Rooms[c->currRoom]->member) {
                if (id == c->id) continue;
                addMail(cl, "[Client " + std::to_string(c->id) + "] (All): " + sendMsg);
            }
        }
        else if (msg == "WHO") {
            std::string mainStr = "[Member in " + c->currRoom + "] :\n";
            for (auto &[id, cl] : Rooms[c->currRoom]->member) {
                if (id == c->id) continue;
                mainStr += "Client " + std::to_string(cl->id) + "\n";
            }
            addMail(c, mainStr);
        }
        else {
            addMail(c, "Command not found!");
        }
    }

    close(c->sockfd);
    removeMember(c, c->currRoom);
    std::cout << "Client " << c->id << " disconnected.\n";
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int port = 3000;
    sockaddr_in serAdd{};
    serAdd.sin_family = AF_INET;
    serAdd.sin_addr.s_addr = INADDR_ANY;
    serAdd.sin_port = htons(port);

    if (bind(sockfd, (sockaddr*)&serAdd, sizeof(serAdd)) < 0) {
        std::cerr << "Binding failed!" << std::endl;
        return 0;
    }

    if (listen(sockfd, 10) < 0) {
        std::cerr << "Listening failed!" << std::endl;
        return 0;
    }

    std::cout << "Server listening on port " << port << " ..." << std::endl;
    Rooms["Lobby"] = std::make_shared<Room>();

    while (true) {
        int client_sockfd = accept(sockfd, nullptr, nullptr);
        if (client_sockfd < 0) continue;

        auto c = std::make_shared<Client>();
        c->sockfd = client_sockfd;
        c->id = next_Clients_Id++;
        c->currRoom = "Lobby";
        Rooms["Lobby"]->member[c->id] = c;

        showJoined(c, "Lobby");
        std::thread(client_Thread, c).detach();
        std::thread(send_Thread, c).detach();
        std::thread(server_Check).detach(); 
    }

    close(sockfd);
}
