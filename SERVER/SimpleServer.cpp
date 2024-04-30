#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include <vector>
#include <unordered_map>
using namespace std;

struct Message
{
	string nickname;
	string content;
};

struct User {
	string nickname;
	TCPsocket socket;

	inline bool operator==(const TCPsocket tcp) {
		return socket == tcp;
	}

};

enum infoType { Unknown, iNickname, iMessage };

const std::string DELIMITER = "/|||/";

void add(std::string* sOut, std::string s2);
std::string parse(std::string s1, int infPos);
infoType getInfoType(std::string s1);
void Send(TCPsocket tcp, std::string s);
std::string Receive(TCPsocket tcp, bool pause);

std::unordered_map<std::string, infoType> const infoTable = {
	{"nn",infoType::iNickname},
	{"msg",infoType::iMessage} };

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

	std::vector<User*> userList;

	TCPsocket clientSocket;
	while (true) {
		clientSocket = SDLNet_TCP_Accept(serverSocket);
		if (clientSocket) {
			char buffer[1024];

			if (std::find(userList.begin(),userList.end(),clientSocket) != userList.end()) {
				cout << "New User Joined" << endl;
				std::string bf = Receive(clientSocket,true);
				string nick = "";
				if (getInfoType(bf) == infoType::iNickname) {
					nick = parse(bf, 1);
				}

				userList.push_back(new User{ nick,clientSocket });

				std::string bufferContent;
				add(&bufferContent, "msg");
				add(&bufferContent, "ChArtFX");
				add(&bufferContent, nick + " joined the chat");
				for (int i = 0; i < userList.size(); i++)
				{
					Send(userList[i]->socket, bufferContent);
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
		for (int i = 0; i < userList.size(); i++)
		{
			string bf = Receive(userList[i]->socket, false);
			if (bf.compare("") == 0) {
				if (getInfoType(bf) == infoType::iMessage) {
					for (int j = 0; j < userList.size(); j++)
					{
						if (i != j) {
							Send(userList[j]->socket, bf);
						}
					}
				}
			}
		}
	}


	return 0;
}

void add(std::string* sOut, std::string s2) {
	sOut->append(s2 + DELIMITER );
}

std::string parse(std::string s1,int infPos) {
	size_t pos = 0;
	int CurrentInfPos = 0;
	std::string token;
	while ((pos = s1.find(DELIMITER)) != std::string::npos) {
		token = s1.substr(0, pos);
		if (CurrentInfPos == infPos) {
			return token;
		}
		CurrentInfPos++;
		s1.erase(0, pos + DELIMITER.length());
	}
	return "";
}

infoType getInfoType(std::string s1) {
	auto it = infoTable.find(parse(s1, 0));
	if (it != infoTable.end()) {
		return it->second;
	}
	return infoType::Unknown;
}

void Send(TCPsocket tcp, std::string s) {
	int bytesSent = SDLNet_TCP_Send(tcp, s.c_str(), s.length() + 1);

	if (bytesSent < s.length() + 1) {
		cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << endl;
		return;
	}
}

std::string Receive(TCPsocket tcp,bool pause) {
	SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(1);
	SDLNet_AddSocket(socketSet, reinterpret_cast<SDLNet_GenericSocket>(tcp));
	if (SDLNet_CheckSockets(socketSet, 0) != 0 || pause) {
		char buffer[1024];
		int bytesRead = SDLNet_TCP_Recv(tcp, buffer, sizeof(buffer));
		if (bytesRead > 0) {
			return std::string(buffer);
		}
	}
}
