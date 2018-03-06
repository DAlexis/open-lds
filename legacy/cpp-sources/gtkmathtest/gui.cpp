#include "gui.hpp"

#include <iostream>


// AreaSizeSettings
AreaSizeSettings::AreaSizeSettings() :
    verticalBox(Gtk::ORIENTATION_VERTICAL, 10),
    leftLongitude("Left longitude:"),
    rightLongitude("Right longitude:"),
    topLatitude("Top latitude:"),
    bottomLatitude("Bottom latitude:"),
    
    buttonsBox(Gtk::ORIENTATION_HORIZONTAL, 10),
    apply("Apply settings"),
    restore("Restore"),
    
    leftLonAdj(Gtk::Adjustment::create(40, -90, 90, 1, 5.0, 0.0)),
    rightLonAdj(Gtk::Adjustment::create(50, -90, 90, 1, 5.0, 0.0)),
    topLatAdj(Gtk::Adjustment::create(40, -180, 180, 1, 5.0, 0.0)),
    bottomLatAdj(Gtk::Adjustment::create(50, -180, 180, 1, 5.0, 0.0)),
    
    leftLonSpin(leftLonAdj),
    rightLonSpin(rightLonAdj),
    topLatSpin(topLatAdj),
    bottomLatSpin(bottomLatAdj)
{
    set_label("Map borders");
    set_border_width(5);
    add(verticalBox);
        verticalBox.pack_start(grid);
            grid.set_border_width(10);
            grid.set_row_spacing(10);
            grid.set_column_spacing(10);
            grid.add(leftLongitude);
            grid.add(leftLonSpin);
            
            grid.attach_next_to(rightLongitude, leftLongitude, Gtk::POS_BOTTOM, 1, 1);
            grid.attach_next_to(rightLonSpin, rightLongitude, Gtk::POS_RIGHT, 1, 1);
            
            grid.attach_next_to(bottomLatitude, rightLongitude, Gtk::POS_BOTTOM, 1, 1);
            grid.attach_next_to(bottomLatSpin, bottomLatitude, Gtk::POS_RIGHT, 1, 1);
            
            grid.attach_next_to(topLatitude, bottomLatitude, Gtk::POS_BOTTOM, 1, 1);
            grid.attach_next_to(topLatSpin, topLatitude, Gtk::POS_RIGHT, 1, 1);
            
            grid.show_all_children();
        grid.show();
        verticalBox.pack_end(buttonsBox, Gtk::PACK_EXPAND_PADDING, 10);
            buttonsBox.pack_start(apply, Gtk::PACK_EXPAND_PADDING, 10);
            apply.show();
            
            buttonsBox.pack_end(restore, Gtk::PACK_EXPAND_PADDING, 10);
            restore.show();
        buttonsBox.show();
    verticalBox.show();
    
    //show_all_children();
}

AreaSizeSettings::~AreaSizeSettings()
{
}


// MapArea

MapArea::MapArea()
{
    set_hexpand(true);
}

MapArea::~MapArea()
{
    set_size_request(400, 400);
    //set_default_size(200, 500);
}

bool MapArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();
    
    // coordinates for the center of the window
    int xc, yc;
    xc = width / 2;
    yc = height / 2;

    cr->set_line_width(10.0);

    // draw red lines out from the center of the window
    cr->set_source_rgb(0.8, 0.0, 0.0);
    cr->move_to(0, 0);
    cr->line_to(xc, yc);
    cr->line_to(0, height);
    cr->move_to(xc, yc);
    cr->line_to(width, yc);
    cr->stroke();

    return true;
}


MainWindow::MainWindow() :
    level0Horisontal(Gtk::ORIENTATION_HORIZONTAL, 10),
    level1RightVertial(Gtk::ORIENTATION_VERTICAL, 10),
    m_button1("Button 1"),
    m_button2("Button 2")
{
    // This just sets the title of our new window.
    set_title("Lightning positioning system testing tool");
    set_default_size(1000, 700);
    
    // sets the border width of the window.
    set_border_width(10);
    
    // put the box into the main window.
    add(level0Horisontal);
        level0Horisontal.set_homogeneous(false);
        
        level0Horisontal.pack_start(mapArea, Gtk::PACK_EXPAND_WIDGET, 10);
        mapArea.show();
        
        level0Horisontal.pack_end(level1RightVertial, Gtk::PACK_SHRINK, 2);
            level1RightVertial.pack_start(areaSizeSettings, Gtk::PACK_SHRINK, 10);
            areaSizeSettings.show();
            // instead of gtk_container_add, we pack this button into the invisible
            // box, which has been packed into the window.
            // note that the pack_start default arguments are Gtk::EXPAND | Gtk::FILL, 0
            level1RightVertial.pack_start(m_button1, Gtk::PACK_EXPAND_PADDING, 10);
            
            // always remember this step, this tells GTK that our preparation
            // for this button is complete, and it can be displayed now.
            m_button1.show();
            
            // Now when the button is clicked, we call the "on_button_clicked" function
            // with a pointer to "button 1" as it's argument
            m_button1.signal_clicked().connect(sigc::bind<Glib::ustring>(
                      sigc::mem_fun(*this, &MainWindow::on_button_clicked), "button 1"));
            // call the same signal handler with a different argument,
            // passing a pointer to "button 2" instead.
            m_button2.signal_clicked().connect(sigc::bind<Glib::ustring>(
                      sigc::mem_fun(*this, &MainWindow::on_button_clicked), "button 2"));

            level1RightVertial.pack_end(m_button2, Gtk::PACK_EXPAND_PADDING, 10);

            // Show the widgets.
            // They will not really be shown until this Window is shown.
            m_button2.show();
        level1RightVertial.show();
    level0Horisontal.show();
}

MainWindow::~MainWindow()
{
}

// Our new improved signal handler.  The data passed to this method is
// printed to stdout.
void MainWindow::on_button_clicked(Glib::ustring data)
{
  std::cout << "Hello World - " << data << " was pressed" << std::endl;
}
