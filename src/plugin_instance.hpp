#include "global_variables.hpp"
#include <gtkmm.h>
#include "global_variables.hpp"


class PluginInstance : public Gtk::VBox
{
    public:
        PluginInstance(Plugin_ID id)
        {
            plugin_id = id;
        }
        Plugin_ID get_plugin_id()
        {
            return plugin_id;
        }
    private:
        Plugin_ID plugin_id;
};