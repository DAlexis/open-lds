#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/grid.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>


class MapArea : public Gtk::DrawingArea
{
public:
    MapArea();
    virtual ~MapArea();

protected:
    //Override default signal handler:
    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
};

class AreaSizeSettings : public Gtk::Frame
{
public:
    AreaSizeSettings();
    virtual ~AreaSizeSettings();

private:
    Gtk::Box verticalBox;
    Gtk::Grid grid;
    Gtk::Label leftLongitude, rightLongitude, topLatitude, bottomLatitude;
    
    Gtk::Box buttonsBox;
    Gtk::Button apply, restore;
    
    Glib::RefPtr<Gtk::Adjustment> leftLonAdj, rightLonAdj, topLatAdj, bottomLatAdj;
    Gtk::SpinButton leftLonSpin, rightLonSpin, topLatSpin, bottomLatSpin;
};

class MainWindow : public Gtk::Window
{
public:
    MainWindow();
    virtual ~MainWindow();

protected:

    // Signal handlers:
    // Our new improved on_button_clicked(). (see below)
    void on_button_clicked(Glib::ustring data);
    
    // Child widgets:
    Gtk::Box level0Horisontal;
    Gtk::Box level1RightVertial;
    MapArea mapArea;
    AreaSizeSettings areaSizeSettings;
    
    Gtk::Button m_button1, m_button2;
};

#endif //GUI_H_INCLUDED
