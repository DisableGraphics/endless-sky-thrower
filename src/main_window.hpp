#pragma once
#include <gtkmm.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "gtkmm/hvbox.h"
#include "instance_widget.hpp"
#include "plugin_instance.hpp"
#include "changelog_widget.hpp"
#include "icon/esthrower.xpm"

//The main window. Inherits from Gtk::Window
class MyWindow : public Gtk::Window
{
	public:
		MyWindow();
		//Adds an intance to the list of instances and shows it in the window
		void add_instance(std::string name, std::string type, std::string version, Gtk::Window * win, bool autoupdate, bool untouched);
		//Read the instances from the disk
		std::vector<Instance> read_instances();
		//Returns the list of instances
		std::vector<Instance> * get_instances();
		//Returns the progress bar
		Gtk::ProgressBar * get_progress();
		std::vector<Gtk::Button> * get_instance_buttons();
	private:
		std::vector<Gtk::Button> instance_buttons;
		std::vector<Instance> instances;
		Gtk::Button m_new_instance_button, m_open_data_folder_button;
		Gtk::ProgressBar progress;
		Gtk::VBox m_vbox;
		Gtk::HeaderBar titlebar;    
		Gtk::Notebook m_notebook;
		Gtk::VBox m_plugins_vbox;
		Gtk::ScrolledWindow m_plugins_scrolled_window, m_instances_scrolled_window;
		ChangelogWidget changelog;
		bool generated_plugins{false};

		void save_instances();
		void remove_instance(std::string name);

		bool on_deleete_event(GdkEventAny* any_event);
		void on_switch_page(Gtk::Widget *page, guint number);
		void new_dialog();
		void download_pr(std::string pr_number);
};