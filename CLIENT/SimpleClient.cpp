#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
//#include <string>
using namespace std;

int main(int argc, char* argv[]) {

	if (SDLNet_Init() == -1) {
		cerr << "SDLNet_Init error: " << SDLNet_GetError() << endl;
		return 1;
	}

	cout << "Thank you for using ChArtFX !\n";

	IPaddress ip;
	if (SDLNet_ResolveHost(&ip, "localhost", 4242) == -1) {
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

	string message = "Hello World !";

	int bytesSent = SDLNet_TCP_Send(clientSocket, message.c_str(), message.length() + 1);
	if (bytesSent < message.length() + 1) {
		cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << endl;
		SDLNet_TCP_Close(clientSocket);
		SDLNet_Quit();
		return 1;
	}

	cout << "Sent " << bytesSent << " bytes to the server !" << std::endl;
	SDLNet_TCP_Close(clientSocket);
	SDLNet_Quit();


	return 0;
}
