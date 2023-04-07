#include "functions.hpp"
#include "global_variables.hpp"
#include "secondary_dialogs.hpp"

std::string Functions::get_OS()
{
    #ifdef _WIN32
        return "Windows";
    #elif _WIN64
        return "Windows";
    #elif __APPLE__ || __MACH__
        return "MacOS";
    #elif __linux__
        return "Linux";
    #elif __FreeBSD__
        return "FreeBSD";
    #else
        return "Other";
    #endif
}

std::string Functions::get_first_folder(std::string folder)
{
    std::string first_folder;
    std::filesystem::directory_iterator it(folder);
    for (const auto &entry : it)
    {
        if (entry.is_directory())
        {
            first_folder = entry.path().string();
            break;
        }
    }
    return first_folder;
}

int Functions::get_number_of_files_in_folder(std::string folder)
{
    int number_of_files = 0;
    for (const auto & entry : std::filesystem::directory_iterator(folder))
    {
        number_of_files++;
    }
    return number_of_files;
}


bool Functions::exists_modplugins(std::string datadir)
{
	return std::filesystem::exists(datadir + "/_plugins");
}

bool Functions::is_plugin_installed(const std::string &plugin_name)
{
	// = "/usr/share/aria/plugins/" + plugin_name;
	std::string os = Functions::get_OS();
	std::string plugin_folder;
	std::string plugin_folder_postfix = "plugins/";
	std::string data_folder;

	if(os == "Linux")
	{
		data_folder = std::getenv("HOME") + std::string("/.local/share/endless-sky/");
		if(exists_modplugins(data_folder))
		{
			plugin_folder_postfix = "_plugins/";
		}
		plugin_folder = data_folder + plugin_folder_postfix + plugin_name;
	}
	else if(os == "Windows")
	{
		data_folder = std::getenv("APPDATA") + std::string("/endless-sky/");
		if(exists_modplugins(data_folder))
		{
			plugin_folder_postfix = "_plugins/";
		}
		plugin_folder = data_folder + plugin_folder_postfix + plugin_name;
	}
	else if(os == "MacOS")
	{
		data_folder = std::getenv("HOME") + std::string("/Library/Application Support/endless-sky/");
		if(exists_modplugins(data_folder))
		{
			plugin_folder_postfix = "_plugins/";
		}
		plugin_folder = data_folder + plugin_folder_postfix + plugin_name;
	}

	if (std::filesystem::exists(plugin_folder))
	{
		return true;
	}
	else
	{
		return false;
	}
}


std::string Functions::get_folder_for_filename(std::string filename)
{
	std::string folder;
	for(int i{static_cast<int>(filename.size() - 1)}; i >= 0; i--)
	{
		if(filename[i] == '/')
		{
			folder = filename.substr(0, i);
			break;
		}
	}
	return folder;
}

void Functions::open_folder(std::string instance_name, std::string instance_type, std::string instance_version)
{
    std::string path = global::config_dir + "download/" + instance_name;
    std::string command{""};
    std::string os = Functions::get_OS();
    if(instance_type == "Custom")
    {
        path = get_folder_for_filename(instance_version);
    }

    if(Functions::get_OS() == "Linux")
    {
        command = "xdg-open \"" + path + "\"";
    }
    else if(Functions::get_OS() == "Windows")
    {
        command = "explorer \"" + path + "\"";
    }
    else
    {
        command = "open \"" + path + "\"";
    }
    system(command.c_str());
}

void Functions::launch_game(const std::string &instance_name, const std::string &instance_type, const std::string &instance_version, bool untouched)
{
	//I don't even know how to launch the game on macos in the first place.
	if(!global::lock)
	{
		if(instance_type == "Custom"? std::filesystem::exists(instance_version) : std::filesystem::exists(global::config_dir + "download/" + instance_name))
		{
			std::string es_folder;
			std::string os = Functions::get_OS();
			std::string game_executable{""};
			if(os == "Linux")
			{
				es_folder = std::getenv("HOME") + std::string("/.local/share/endless-sky");
				game_executable = "endless-sky.AppImage";
			}
			else if(os == "Windows")
			{
				es_folder = std::getenv("APPDATA") + std::string("/endless-sky");
				game_executable = "EndlessSky.exe";
			}
			else if(os == "MacOS")
			{
				es_folder = std::getenv("HOME") + std::string("/Library/Application Support/endless-sky");
				game_executable = "EndlessSky";
			}
			bool plugins_exists{std::filesystem::exists(es_folder + "/plugins")};
			bool _plugins_exists{std::filesystem::exists(es_folder + "/_plugins")};
			std::string target_folder_plugins{untouched ? es_folder + "/plugins" : es_folder + "/_plugins"};

			if(untouched && plugins_exists && !_plugins_exists)
			{
				//C++'s native filesystem library doesn't support moving non-empty directories, so we have to use this handy C function.

				rename((es_folder + "/plugins").c_str(), (es_folder + "/_plugins").c_str());
			}
			else if(!untouched && _plugins_exists)
			{
				if(plugins_exists)
				{
					std::filesystem::remove_all(es_folder + "/plugins");
				}
				rename((es_folder + "/_plugins").c_str(), (es_folder + "/plugins").c_str());
			}
			std::string command{""};
			std::string game_command{""};
			if(instance_type != "Custom")
			{
				command = "chmod +x\"" + global::config_dir + "download/" + instance_name + "/" + game_executable + "\"";
				game_command = global::config_dir + "download/" + instance_name + "/" + game_executable;
			}
			else
			{
				command = "chmod +x \"" + global::config_dir + "" + instance_version + "\"";
				game_command = instance_version;
			}
			
			system(command.c_str());
			#ifdef __linux__
			char *const  args[] = {(char *)game_command.c_str()};
			pid_t pid = fork();
			switch(pid) 
			{
				case 0: 
					execvp(game_command.c_str(), args);
					break;
				case -1: 
					std::cout << "[ERROR] Unable to fork secondary process\n";
			}
			#elif _WIN32
			if(instance_type != "Custom")
			{
				bool run_extraction_command{false};
				if(!std::filesystem::exists(global::config_dir + "download/" + instance_name + "/" + game_executable))
				{
					run_extraction_command = true;
					command = "7za x \"" + global::config_dir + "download/" + instance_name + "/EndlessSky-win64.zip\" -o\"" + global::config_dir + "download/" + instance_name + "\"";
				}
				if(run_extraction_command)
				{
					system(command.c_str());
				}
			}
			system(game_command.c_str());
			#endif
		}
		else 
		{
			InformationDialog warn{"Not found",
			"The selected game installation could not be found.\nPlease download the game.",
			true};
			warn.run();
		}
	}
	else
	{
		InformationDialog warn{"Game is being downloaded",
		"The game is being downloaded. Please wait until the download is finished.",
		true};
		warn.run();
	}
}

bool Functions::str_replace(std::string& str, const std::string& from, const std::string& to) 
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
	{
        return false;
	}
    str.replace(start_pos, from.length(), to);
    return true;
}

//This function returns the instance name without the first "v" in the version number
//Used (mostly) for windows, since the ES naming scheme sucks
std::string Functions::instance_version_minus_v(std::string instance_version)
{
    std::string version = instance_version;
    version.erase(0, 1);
    return version;
}

bool Functions::has_v(std::string instance_version)
{
    if(instance_version[0] == 'v')
    {
        return true;
    }
    return false;
}

//Opens the data folder in the file manager
void Functions::open_data_folder()
{
    std::string os{Functions::get_OS()};
    std::string command;
    if(os == "Linux")
    {
        command = "xdg-open " + std::string(getenv("HOME")) + "/.local/share/endless-sky/";
    }
    else if(os == "Windows")
    {
        command = "explorer " + std::string(getenv("APPDATA")) + "\\endless-sky\\";
    }
    else if(os == "MacOS")
    {
        command = "open " + std::string(getenv("HOME")) + "/Library/Application Support/endless-sky/";
   }
    system(command.c_str());
}

//Return the home dir
std::string Functions::get_home_dir()
{
	std::string os{Functions::get_OS()};
	if(os == "Linux")
	{
		return std::string(getenv("HOME"));
	}
	else if(os == "Windows")
	{
		return std::string(getenv("APPDATA"));
	}
	else if(os == "MacOS")
	{
		return std::string(getenv("HOME"));
	}
	return "";
}