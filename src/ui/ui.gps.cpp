#include "ui.gps.h"

/*------------------------------------
-- CONSTRUCTOR: GpsWindow::GpsWindow
-- Date: November 12 2013
-- Designer: Sam Youssef
-- Programmer: Sam Youssef
--
-- INTERFACE:   
--              gui(gpsdata* gpsd, QWidget* parent)
--                  gpsdata* gpsd: pointer to data struct for holding data from gps device
--                  QWidget* parent: pointer to a parent object
--
-- NOTES:
-- Constructor for GpsWindow class. Generates the Gui and instantiates a seperately 
-- threaded class for polling the gps device. The Gui includes a display graphic
-- showing satellite positions, a table for displaying satellite-specific 
-- information (e.g. PRN, Azimuth, etc.), a table for displaying the fix,
-- and a menu.
--
-- In addition, it creates a number of signal-to-slot connections used for 
-- for inter-thread communication and handles memory deallocation.
--
-- Rather than handling the above directly, the tasks are decomposed and passed on 
-- to specialized helper functions.
--------------------------------------*/
GpsWindow::GpsWindow(gpsdata* gpsd, QWidget* parent) : QMainWindow(parent) {  
    
    this->gpsd = gpsd;

    const char gps_headers[][10] = { "Timestamp", "Latitude", "Longitude" };
    const char sat_headers[][10] = { "PRN", "ELEV", "AZIM", "SNR", "USED" };
    
    struct table sat_table = { &sat_data, 5, 55  };
    struct table gps_table = { &gps_data, 3, 200 };
    
    ctor_createThread();
                
    ctor_createTable( sat_table, sat_headers, SAT_DISPLAY_SLOTS );
    ctor_createTable( gps_table, gps_headers, 1  );
    ctor_createMenuBar();
    ctor_createSatelliteView();

    ctor_useGridLayout();


    gps_data->setItem(0, 0, new QTableWidgetItem(""));
    gps_data->setItem(0, 1, new QTableWidgetItem(""));
    gps_data->setItem(0, 2, new QTableWidgetItem(""));
    
    
    for(int i = 0; i < SAT_DISPLAY_SLOTS; ++i) {
        for(int j = 0; j < 5; ++j) {
            sat_data->setItem(i, j, new QTableWidgetItem(""));
        }
    }
    
    diss_action->setEnabled(false);
    
    // for cleanup before quitting
    connect( qApp, SIGNAL(aboutToQuit()), this, SLOT(cleanup()) );
    
}

/*------------------------------------
-- Method: GpsWindow::ctor_createThread
-- Date: November 12 2013
-- Designer: Sam Youssef
-- Programmer: Sam Youssef
--
-- INTERFACE: void GpsWindow::ctor_createThread(void)
--
-- RETURNS: void.
--
-- NOTES:
-- Helper function for instantiating a new Thread class. 
-- Sets up a signal to print data on being received.
-- Sets up a signal that is sent to stop the device polling thread on program close.
--------------------------------------*/
void GpsWindow::ctor_createThread() {
    thread = new Thread(gpsd);
    connect( thread, SIGNAL(read_true()), this, SLOT(print_data()), Qt::QueuedConnection );
    connect( qApp, SIGNAL(aboutToQuit()), thread, SLOT(stop_gps()) );
}

/*------------------------------------
-- Method: GpsWindow::ctor_createTable
-- Date: November 12 2013
-- Designer: Sam Youssef
-- Programmer: Sam Youssef
--
-- INTERFACE:   void GpsWindow::ctor_createTable(struct table& t, const char labels[][10], int sz)
--                              struct table& t: number of headers
--                              const char labels[][10]: array of header names
--                              int sz: number of table rows
-- RETURNS: void
--
-- NOTES:
-- Creates a gui table for displaying gps information.
--------------------------------------*/
void GpsWindow::ctor_createTable(struct table& t, const char labels[][10], int sz) {
   
  (*t.qtable) = new QTableWidget (sz, t.nlbl, this);
  
  for(size_t i = 0; i < (t.nlbl); ++i)
  { 
        QTableWidgetItem* header = new QTableWidgetItem();
    header->setText( labels[i] );
    
    (*t.qtable)-> setHorizontalHeaderItem(i, header);
      (*t.qtable)-> setColumnWidth(i, t.colW);
  }  
  
    (*t.qtable)-> horizontalHeader()->setStretchLastSection(true);
}

/*------------------------------------
-- Method: GpsWindow::ctor_createMenuBar
-- Date: November 12 2013
-- Designer: Sam Youssef
-- Programmer: Sam Youssef
--
-- INTERFACE: void GpsWindow::ctor_createMenuBar(void) 
-- RETURNS: void.
--
-- NOTES:
-- Helper function which creates a menu bar with options to exit, connect and disconnect. 
-- Sets up the signaling mechanism between a menu click and the appropriate method to be called.
--------------------------------------*/
void GpsWindow::ctor_createMenuBar() {

    quit_action = new QAction("&Quit", this);
    conn_action = new QAction("&Connect", this);
    diss_action = new QAction("&Disconnect", this);

    QMenu* file_menu    = menuBar() -> addMenu("&File");
    QMenu* gps_menu   = menuBar() -> addMenu("&Gps");
    
    file_menu->addAction(quit_action);
    
    gps_menu->addAction(conn_action);
    gps_menu->addAction(diss_action);
        

    connect( quit_action, SIGNAL(triggered()), qApp, SLOT(quit()) );

    connect( conn_action, SIGNAL(triggered()), thread, SLOT(read_gps()) );
    connect( conn_action, SIGNAL(triggered()), this, SLOT(menu_conn()) );   
    
    connect( diss_action, SIGNAL(triggered()), thread, SLOT(stop_gps()) );
    connect( diss_action, SIGNAL(triggered()), this, SLOT(menu_diss()) );
}

/*------------------------------------
-- Method: GpsWindow::ctor_createSatelliteView
-- Date: November 12 2013
-- Designer: Sam Youssef
-- Programmer: Sam Youssef
--
-- INTERFACE: void GpsWindow::ctor_createSatelliteView(void)
-- RETURNS: void.
--
-- NOTES:
-- Helper function which generates the graphical satellite view.
--------------------------------------*/
void GpsWindow::ctor_createSatelliteView() {

    graphical_scene = new QGraphicsScene(this);
    graphical_scene->setSceneRect(0, 0, 360, 360);
            
    QPen pen;
    pen.setColor(Qt::black);
    
    graphical_scene->addEllipse(0, 0, 360, 360, pen);
    graphical_scene->addEllipse(90, 90, 180, 180, pen);
    graphical_scene->addLine(180, 0, 180, 360 , pen);
    graphical_scene->addLine(0, 180, 360, 180 , pen);
    graphical_scene->addText("N")->setPos(180, 0);
    graphical_scene->addText("E")->setPos(340, 180);
        
    graphical_view = new QGraphicsView( graphical_scene );
    
    for(int i = 0; i < SAT_DISPLAY_SLOTS; ++i) {
            points[i] = new QGraphicsEllipseItem( 0, graphical_scene );
            labels[i] = new QGraphicsTextItem( 0, graphical_scene );
    }
    
}

/*------------------------------------
-- Method: GpsWindow::ctor_useGridLayout
-- Date: November 12 2013
-- Designer: Sam Youssef
-- Programmer: Sam Youssef
-- 
-- INTERFACE: void GpsWindow::ctor_useGridLayout(void)
-- RETURNS: void
--
-- NOTES:
-- Sets window layout to Grid layout.
--------------------------------------*/
void GpsWindow::ctor_useGridLayout() {

  gridLayout = new QGridLayout();
  
  grid = new QWidget(this);
  grid->setLayout(gridLayout);
 
  gridLayout->addWidget(sat_data, 0, 1, 1, 1);
  gridLayout->addWidget(gps_data, 1, 0, 2, 0);
  gridLayout->addWidget(graphical_view, 0, 0, 1, 1);
  gridLayout->setSpacing(1);
    
  this->setCentralWidget(grid);

}


/*------------------------------------
-- Method: GpsWindow::cleanup
-- Date:    November 12 2013
-- Designer:    Sam Youssef
-- Programmer: Sam Youssef
--
-- INTERFACE: void GpsWindow::cleanup(void)
-- RETURNS: void
--
-- NOTES:
-- Deallocates memory on programming exit.
--------------------------------------*/
void GpsWindow::cleanup() 
{       
    for(int i = 0; i < SAT_DISPLAY_SLOTS; ++i) {
        delete points[i];
        delete labels[i];
    }
    
    sat_data->clear();
    gps_data->clear();
    
    graphical_scene->clear();

    delete gridLayout;
                
    delete sat_data;
    delete gps_data;    
    
    delete graphical_view;
    delete graphical_scene;
    
    thread->wait();
    delete thread;
}
