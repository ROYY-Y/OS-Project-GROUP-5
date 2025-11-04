#include <iostream>
#include <thread>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void send_Thread(int sockfd){
    std::string msg;

    while(true){
        std::getline(std::cin, msg);
        if (msg == "QUIT") {
            send(sockfd, msg.c_str(), msg.size(), 0);  // ส่งแจ้งเซิร์ฟเวอร์
            std::cout << "Disconnecting...\n";
            close(sockfd);  // ปิด socket
            exit(0);
            break;
        }else if (msg == "INFO"){
            std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n";
            std::cout << "- Command :                                                           -\n";
            std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n";
            std::cout << "- JOIN <room_name> (join new room)\n";
            std::cout << "- SAY <message> (send message to all in room)\n";
            std::cout << "- DM <client_id> <message> (send direct message to specific client id)\n";
            std::cout << "- WHO (check all client online in the same room)\n";
            std::cout << "- LEAVE (leave from current room)\n";
            std::cout << "- QUIT (disconnect)\n";
            continue;
        }
        send(sockfd, msg.c_str(), msg.size(), 0);
    }
}

void recive_Thread(int sockfd){
    char buff[1024];

    while(true){
        int n = recv(sockfd, buff, sizeof(buff)-1, 0);
        if(n <= 0) break; //Server maybe closed or any problem
        buff[n] = '\0';
        std::cout << buff << std::endl;
    }
    std::cout << "Server shutdown!";
}

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(3000);
    inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr);
    
    if(connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        std::cerr << "Connection failed!!";
        return 1;
    }
    std::thread t1(send_Thread, sockfd);
    std::thread t2(recive_Thread, sockfd);

    t1.join();
    t2.join();

    close(sockfd);
    return 0;
}