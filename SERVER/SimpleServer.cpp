#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include <map>
using namespace std;

struct Message
{
	string nickname;
	string content;
};


int main(int argc, char* argv[]) {

	if (SDLNet_Init() == -1) {
		cerr << "SDLNet_Init error: " << SDLNet_GetError() << endl;
		return 1;
	}


	cout << "Thank you for using ChArtFX !\n";

	IPaddress ip;
	if (SDLNet_ResolveHost(&ip, nullptr, 4242) == -1) {
		cerr << "Resolve Host Error: " << SDLNet_GetError() << endl;
		SDLNet_Quit();
		return 1;
	}

	TCPsocket serverSocket = SDLNet_TCP_Open(&ip);
	if (!serverSocket) {
		cerr << "TCP Open Error: " << SDLNet_GetError() << endl;
		SDLNet_Quit();
		return 1;
	}

	std::map<TCPsocket, string> userList;

	TCPsocket clientSocket;
	while (true) {
		clientSocket = SDLNet_TCP_Accept(serverSocket);
		if (clientSocket) {
			char buffer[1024];

			if (userList[clientSocket].empty()) {
				cout << "A client reached the server!" << endl;
				Message message = Message{ "ChArtFX", "A client reached the server!" };
				char buffer[1024];
				int len = sprintf_s(buffer, "%s" "%s", "Hello", "Hi");
				int bytesSent = SDLNet_TCP_Send(clientSocket, buffer, sizeof(buffer));
				if (bytesSent < sizeof(message)) {
					cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << endl;
					break;
				}
			}
			
			
			/*
			int bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
			if (bytesRead > 0) {
				cout << "Incoming message: " << buffer << endl;
				string answer = "Message received 5/5, client!";

				//This is what is going to surpass ChatGPT
				if (strcmp(buffer,"Hello") == 0) {
					answer = "Hello !!";
				}
				else if (strcmp(buffer, "Ping") == 0) {
					answer = "Pong !!";
				}
				else if (strcmp(buffer, "Can you help me ?") == 0) {
					answer = "Sorry, i can't help you Dave!";
				}
				
				
				int bytesSent = SDLNet_TCP_Send(clientSocket, answer.c_str(), answer.length() + 1);
				if (bytesSent < answer.length() + 1) {
					cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << endl;
					break;
				}

			}*/

		}
	}


	return 0;
}
