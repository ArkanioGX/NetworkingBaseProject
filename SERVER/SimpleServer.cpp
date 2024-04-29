#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
using namespace std;

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

	TCPsocket clientSocket;
	while (true) {
		clientSocket = SDLNet_TCP_Accept(serverSocket);
		if (clientSocket) {
			cout << "A client reached the server!" << endl;
			char buffer[1024];
			int bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
			if (bytesRead > 0) {
				cout << "Incoming message: " << buffer << endl;
				break;
			}
		}
	}


	return 0;
}
