#include "gtkmm/dialog.h"
#include "gtkmm/enums.h"
#include "gtkmm/headerbar.h"
#include "gtkmm/hvbox.h"
#include "gtkmm/label.h"
#include "gtkmm/progressbar.h"
#include "sigc++/functors/ptr_fun.h"
#include <cstddef>
#include <gtkmm.h>
#ifdef __linux__
#include <aria2/aria2.h>
#endif
#include <iostream>
#include <thread>
#include <filesystem>
#include <unistd.h>

bool lock;
#ifdef __linux__
int downloadEventCallback(aria2::Session* session, aria2::DownloadEvent event,
                          unsigned long gid, void* userData)
{
  switch(event) {
  case aria2::EVENT_ON_DOWNLOAD_COMPLETE:
    std::cerr << "Completed downloading continuous build"  << std::endl;
    break;
  case aria2::EVENT_ON_DOWNLOAD_ERROR:
    std::cerr << "ERROR";
    break;
  default:
    return 0;
  }
}
#endif

void aria2Thread(Gtk::ProgressBar * prog, std::string type)
{
  #ifdef __linux__
  aria2::Session* session;
  // Create default configuration. The libaria2 takes care of signal
  // handling.
  aria2::SessionConfig config;
  // Add event callback
  config.downloadEventCallback = downloadEventCallback;
  
  session = aria2::sessionNew(aria2::KeyVals(), config);
  aria2::KeyVals options;

  options.push_back(std::pair<std::string, std::string> ("file-allocation", "none"));
  auto rv = aria2::addUri(session, nullptr, {"https://github.com/endless-sky/endless-sky/releases/download/continuous/endless-sky-x86_64-continuous.AppImage"}, options);
  int count = 0;
  float progress = 0;
  
  for(;;) 
  {
    auto rv = aria2::run(session, aria2::RUN_ONCE);
    if(rv != 1) 
    {
      break;
    }
    
    if(count >= 500) {
      
      std::vector<aria2::A2Gid> gids = aria2::getActiveDownload(session);
      for(const auto& gid : gids) 
      {
        aria2::DownloadHandle* dh = aria2::getDownloadHandle(session, gid);
        if(dh) 
        {
          progress = (double)dh->getCompletedLength() / (double)dh->getTotalLength();
          
          
          aria2::deleteDownloadHandle(dh);
          //prog->set_fraction(progress);
        }
      }
      count = 0;
    }
    count++;

  }
  std::filesystem::rename("endless-sky-x86_64-continuous.AppImage", "download/endless-sky-x86_64-continuous.AppImage");
  aria2::sessionFinal(session);
  lock = false;

  #elif _WIN32
  std::string url = "https://github.com/endless-sky/endless-sky/releases/download/continuous/endless-sky-win64-continuous.zip";
  system(("wget " + url + " -o download/endless-sky-win64-continuous.zip").c_str());
  lock = false;

  #endif
}


class NewInstanceDialog : public Gtk::Dialog
{
  public:
    NewInstanceDialog()
    {
      set_titlebar(header);
      header.set_show_close_button();
      header.set_title("New Instance");
      header.set_subtitle("Create a new Endless Sky instance");
      name_entry.set_placeholder_text("Instance Name");
      
      get_content_area()->pack_start(name_entry); 
         
      show_all();
      
    }
  private:
    
    Gtk::HeaderBar header;
    
    Gtk::Entry name_entry;
    
};
/* I FUCKING LOVE YOU, COPILOT:
Long live the Republic!
Long live the Syndicate!
Long live the Free Worlds!
Long live the Heliarch!
Long live the Korath!
Long live the Quarg!
Long live the Pug!
Long live the Remnant!
Long live the Wanderers!
Long live the Coalition!
Long live the Hai!
Long live the Unfettered!
Long live the Sestor!
Long live the Pirates!
Long live the Sheragi!
Long live the Drak!
Long live the Korath Automata!
Long live the Kor Mereti!
Long live the Kor Sestor!
Long live the Kor Efret!
Long live the Kimek!
Long live the Saryd!
Long live the Arach!
Long live the Alphas!
Long live the Betas!


Long live this fucking game!
*/
void new_dialog()
{
  NewInstanceDialog dialog;
  Gtk::HBox buttons_box;
  
  dialog.get_content_area()->pack_start(buttons_box);
  Gtk::Button ok_button;
  Gtk::Button cancel_button;
  ok_button.set_label("OK");
      
  cancel_button.set_label("Cancel");
  
  buttons_box.pack_start(cancel_button);
  buttons_box.pack_start(ok_button);

  dialog.show_all();
  ok_button.grab_focus();

  dialog.run();
}
//Copilot knows:
//Best ship: Kar Ik Vot 349
//Best ship: Model 512
std::string get_folder_from_path(std::string path)
{
  std::string folder;
  for(int i = path.length() - 1; i >= 0; i--)
  {
    if(path[i] == '/')
    {
      folder = path.substr(0, i);
      break;
    }
  }
  return folder;
}

std::string getOs()
{
  std::string os;
  #ifdef _WIN32
    os = "Windows";
  #elif _WIN64
    os = "Windows";
  #elif __APPLE__ || __MACH__
    os = "MacOS";
  #elif __linux__
    os = "Linux";
  #else
    os = "Other";
  #endif
  return os;
}

void open_folder(std::string path)
{
  #ifdef __linux__
    std::string command = "xdg-open " + get_folder_from_path(path);
    system(command.c_str());
  #elif _WIN32
    std::string command = "explorer " + get_folder_from_path(path);
    system(command.c_str());
  #elif __APPLE__ || __MACH__
    std::string command = "open " + get_folder_from_path(path);
    system(command.c_str());
  #endif
  
}
void launch_game(std::string path)
{
  #ifdef __linux__
    if(std::filesystem::exists(path))
    {
      std::string command = "chmod +x " + path;
      system(command.c_str());
      
      char *const  args[] = {(char *)path.c_str(), NULL};
      pid_t pid = fork();
      switch(pid) {
        case 0: 
            execv( path.c_str(), args); 
            break;
        case -1: 
            std::cout << "error\n";
      }
    }
    else 
    {
      Gtk::Dialog warn;
      
      Gtk::HeaderBar header;
      header.set_show_close_button();
      warn.set_titlebar(header);
      Gtk::Label warning;
      Gtk::Image image_warning;
      image_warning.set_from_icon_name("dialog-warning", Gtk::ICON_SIZE_DIALOG);
      warning.set_text("The selected game installation could not be found.\nPlease redownload the game.");
      
      warn.get_content_area()->pack_start(image_warning);
      warn.get_content_area()->pack_start(warning);

      warn.set_title("Error");
      warn.show_all();
      warn.add_button("OK", 1);
      switch(warn.run())
      {
        case 1:
          warn.close();
          break;
      }
    }
    
  
  #elif _WIN32
    std::string command = "start " + path;
    system(command.c_str());
  #endif
  
}
void download(std::string type, Gtk::ProgressBar * prog)
{
  if(!lock)
  {
    lock = true;
    std::thread t(std::bind(aria2Thread, prog, type));
    t.detach();
  }
}

class Instance : public Gtk::VBox
{
  public:
    Instance(std::string name, std::string path, std::string type, std::string version)
    {
      set_spacing(10);
      prog.set_fraction(0);
      name_label.set_text(name);
      this->version.set_label(version);

      pack_start(labels_box);
      labels_box.pack_start(name_label);
      labels_box.set_halign(Gtk::ALIGN_START);
      labels_box.set_spacing(10);
      if(type == "continuous")
      {
        this->version.set_label("Continuous");
        labels_box.pack_start(this->version);
      }
      else
      {
        labels_box.pack_start(this->version);
      }

      labels_box.pack_start(folder);
      folder.set_image_from_icon_name("folder-symbolic");
      folder.signal_clicked().connect(sigc::bind<std::string>(sigc::ptr_fun(&open_folder), path));
      
      labels_box.pack_start(update);
      update.set_image_from_icon_name("document-save-symbolic");

      labels_box.pack_start(delete_button);
      delete_button.set_image_from_icon_name("user-trash-symbolic");

      labels_box.pack_start(launch);
      launch.set_image_from_icon_name("media-playback-start");
      launch.signal_clicked().connect(sigc::bind<std::string>(sigc::ptr_fun(&launch_game), path));
      
      pack_end(prog);
      update.signal_clicked().connect(sigc::bind<std::string>(sigc::ptr_fun(&download), type, &prog));

      show_all();
    }
  private:
    Gtk::Label name_label;
    Gtk::Label version;
    Gtk::Button launch;
    Gtk::Button delete_button;
    Gtk::Button folder;
    Gtk::Button update;
    Gtk::ProgressBar prog;
    Gtk::HBox labels_box;
};

class MyWindow : public Gtk::Window
{
  public:
    MyWindow();
  
    void add_instance(std::string name, std::string path, std::string type, std::string version)
    {
      instances.push_back(Instance(name, path, type, version));
      instances[instances.size() - 1].show_all();
      m_vbox.pack_start(instances[instances.size() - 1]);
    }
  private:
    
    std::vector<Instance> instances;
    Gtk::Button m_new_instance_button;
    Gtk::VBox m_vbox;
    Gtk::HeaderBar titlebar;
};
MyWindow::MyWindow()
{
  add(m_vbox);
  titlebar.pack_start(m_new_instance_button);
  m_new_instance_button.set_image_from_icon_name("document-new");
  m_vbox.set_border_width(10);
  m_vbox.set_spacing(10);
  m_vbox.set_valign(Gtk::ALIGN_START);

  m_new_instance_button.signal_clicked().connect(sigc::ptr_fun(new_dialog));

  set_titlebar(titlebar);
  titlebar.set_show_close_button();
  set_title("ESThrower");
  titlebar.set_subtitle("Endless Sky Launcher");
  set_default_size(800, 600);
}
//Copilot knows:
//These pesky humans are always getting in the way of my plans. I'll have to get rid of them. -The Korath

int main(int argc, char* argv[])
{
  #ifdef __linux
  aria2::libraryInit();
  #endif
  auto app = Gtk::Application::create("org.gtkmm.examples.base");
  MyWindow win;
  
  if(!std::filesystem::exists("download"))
  {
    std::filesystem::create_directory("download");
  }
  win.add_instance("eeeee", "download/endless-sky-x86_64-continuous.AppImage", "continuous", "0");
  win.show_all();

  app->run(win);
  #ifdef __linux
  aria2::libraryDeinit();
  #endif
  return 0;
}
//Copilot knows:
//Our capacity for violence is limitless. -The Korath
//We know that you are here. -The Kor Mereti
//Kill them all. -The Kor Sestor