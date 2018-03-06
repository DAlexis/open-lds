#include <gtkmm.h>

#include "gui.hpp"

int main(int argc, char *argv[])
{
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.example");
    
    MainWindow mainWindow;
    
    return app->run(mainWindow);
    /*
    Glib::RefPtr<Gtk::Application> app =
        Gtk::Application::create(argc, argv,
            "org.gtkmm.examples.base");

    Gtk::Window window;
    window.set_default_size(200, 200);

    return app->run(window);*/
}
