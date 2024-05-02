#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include <string>
#include <raylib.h>
#include <vector>
#include <unordered_map>

using namespace std;

constexpr int SCREEN_WIDTH = 500, SCREEN_HEIGHT = 750;

struct Message
{
	string nickname;
	string content;
};

enum infoType {Unknown, iNickname, iMessage};

const std::string DELIMITER = "/|||/";

void add(std::string* sOut, std::string s2);
std::string parse(std::string s1, int infPos);
infoType getInfoType(std::string s1);
std::string Receive(TCPsocket tcp, bool pause);

std::unordered_map<std::string, infoType> const infoTable = { 
	{"nn",infoType::iNickname},
	{"msg",infoType::iMessage}};

int main(int argc, char* argv[]) {

	vector<Message> log{};

	if (SDLNet_Init() == -1) {
		cerr << "SDLNet_Init error: " << SDLNet_GetError() << endl;
		return 1;
	}

	cout << "Thank you for using ChArtFX !\n";

	cout << "Enter the IP Adress :";
	string ipStr;
	getline(cin, ipStr);
	cin.clear();
	cout << endl;

	cout << "Enter the port :";
	int port;
	cin >> port;
	cin.clear();
	cin.ignore();
	cout << endl;

	cout << "Enter your nickname :";
	string nickname;
	getline(cin, nickname);
	cin.clear();
	cout << endl;

	IPaddress ip;

	if (SDLNet_ResolveHost(&ip, ipStr.c_str(), port) == -1) {
		cerr << "Resolve Host error: " << SDLNet_GetError() << endl;
		SDLNet_Quit();
		return 1;
	}

	TCPsocket clientSocket = SDLNet_TCP_Open(&ip);
	if (!clientSocket) {
		cerr << "TCP Open error: " << SDLNet_GetError() << endl;
		SDLNet_Quit();
		return 1;
	}

	//Send nickname Info
	{
		string nnSend;
		add(&nnSend, "nn");
		add(&nnSend, nickname);
		int bytesSent = SDLNet_TCP_Send(clientSocket, nnSend.c_str(), nnSend.length() + 1);
		if (bytesSent < nnSend.length() + 1) {
			cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << endl;
			SDLNet_TCP_Close(clientSocket);
			SDLNet_Quit();
			return 1;
		}
	}

	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "My first chat window!");
	SetTargetFPS(60);

	string typing;

	while (!WindowShouldClose()) {

		BeginDrawing();
		ClearBackground(GRAY);
		DrawText("Welcome to ChArtFX!", 220, 15, 25, WHITE);
		DrawRectangle(20, 50, SCREEN_WIDTH - 40, SCREEN_HEIGHT - 150, DARKGRAY);
		
		std::string msg = Receive(clientSocket, false);
		if (msg.compare("") != 0) {
			if (getInfoType(msg) == infoType::iMessage) {
				log.push_back(Message{ parse(msg,1), parse(msg,2) });
			}
		}

		for (int msg = 0; msg < log.size(); msg++)
		{
			DrawText((log[msg].nickname + " : " + log[msg].content).c_str(), 30, 75 + (msg * 30), 15, RAYWHITE);
		}

		DrawRectangle(20, SCREEN_HEIGHT - 90, SCREEN_WIDTH - 40, 50, LIGHTGRAY);

		int inputChar = GetCharPressed();
		if (inputChar != 0) //A character is pressed on the keyboard
		{
			typing += static_cast<char>(inputChar);
		}
		if (typing.size() > 0)
		{
			if (IsKeyPressed(KEY_BACKSPACE)) typing.pop_back();
			else if (IsKeyPressed(KEY_ENTER))
			{
				string msg;
				add(&msg, "msg");
				add(&msg, nickname);
				add(&msg, typing);
				int bytesSent = SDLNet_TCP_Send(clientSocket, msg.c_str(), msg.length() + 1);
				if (bytesSent < msg.length() + 1) {
					cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << endl;
					SDLNet_TCP_Close(clientSocket);
					SDLNet_Quit();
					return 1;
				}
				log.push_back(Message{ nickname, typing});
				typing.clear();
			}
			DrawText(typing.c_str(), 30, SCREEN_HEIGHT - 75, 25, DARKBLUE);
		}

		

		EndDrawing();

	}
	CloseWindow();

	SDLNet_TCP_Close(clientSocket);
	SDLNet_Quit();


	return 0;
}

void add(std::string* sOut, std::string s2) {
	sOut->append(s2 + DELIMITER);
}

std::string parse(std::string s1, int infPos) {
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
	auto it = infoTable.find(parse(s1,0));
	if (it != infoTable.end()) {
		return it->second;
	}
	return infoType::Unknown;
}

std::string Receive(TCPsocket tcp, bool pause) {
	SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(1);
	SDLNet_AddSocket(socketSet, reinterpret_cast<SDLNet_GenericSocket>(tcp));
	if (SDLNet_CheckSockets(socketSet, 0) != 0 || pause) {
		char buffer[1024];
		int bytesRead = SDLNet_TCP_Recv(tcp, buffer, sizeof(buffer));
		if (bytesRead > 0) {
			return std::string(buffer);
		}
	}
	return "";
}


