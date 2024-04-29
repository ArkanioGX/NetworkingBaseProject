#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include <string>
#include <raylib.h>
#include <vector>

using namespace std;

constexpr int SCREEN_WIDTH = 500, SCREEN_HEIGHT = 750;

struct Message
{
	string nickname;
	string content;
};

int main(int argc, char* argv[]) {

	vector<Message> log{Message{"ChArtFX", "Waiting for someone to talk to..."}};

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

	IPaddress ip = {stoi(ipStr)};

	if (SDLNet_ResolveHost(&ip, "localhost", port) == -1) {
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

	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "My first chat window!");
	SetTargetFPS(60);

	string typing;

	while (!WindowShouldClose()) {

		BeginDrawing();
		ClearBackground(GRAY);
		DrawText("Welcome to ChArtFX!", 220, 15, 25, WHITE);
		DrawRectangle(20, 50, SCREEN_WIDTH - 40, SCREEN_HEIGHT - 150, DARKGRAY);

		char buffer[1024];
		int bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
		if (bytesRead > 0) {
			
			char content[128] = { 0 };
			char nick[128] = { 0 };

			sscanf_s(buffer, "%s" "%s", nick, content);
			log.push_back(Message{std::string(nick),std::string(content)});
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
				int bytesSent = SDLNet_TCP_Send(clientSocket, typing.c_str(), typing.length() + 1);
				if (bytesSent < typing.length() + 1) {
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



