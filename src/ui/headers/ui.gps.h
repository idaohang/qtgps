#ifndef GUI_H
#define GUI_H

#include "dev.gps.h"

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>

#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>

#include <QGraphicsView>
#include <QGraphicsScene>

#include <QGridLayout>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>

#include <QThread>
#include <QMutex>

#include <cstring>

#define WINDOWHEIGHT 700
#define WINDOWWIDTH  700

#define SAT_DISPLAY_SLOTS 10

struct table {

    QTableWidget **qtable;
    
    const size_t nlbl;
    const size_t colW;  
    
};


/*------------------------------------
-- Class:   Thread
-- Date:    November 12 2013
-- Designer:    Sam Youssef
-- Programmer: Sam Youssef
--
-- PUBLIC INTERFACE:    
--
--               Thread::Thread(gpsdata* gpsd)
--                      gpsdata* gpsd: pointer to struct used for holding data read from a gps device
--
--               void Thread::read_gps();
--               void Thread::stop_gps();
--               
-- NOTES:
-- This class reads from gps device and runs in its own thread.
--------------------------------------*/
class Thread : public QThread {
    Q_OBJECT
    public:
        Thread(gpsdata*);
    
    public slots:
    void read_gps() { 
        start(); 
    }
    
    void stop_gps() { 
        abort = true; 
    }
    
    signals:
        void read_true();
    
    protected:
        void run();
        int exec();
        
    private:
        // data struct
        gpsdata* gpsd;
        
        // thread synchronization attributes
        QMutex mutex;
        bool abort;
};

/*------------------------------------
-- Class:   GpsWindow
-- Date:    November 12 2013
-- Designer:    Sam Youssef
-- Programmer: Sam Youssef
--
-- PUBLIC INTERFACE:    
--
--              GpsWindow::GpsWindow(gpsdata* gpsd, QWidget* parent)
--                      gpsdata* gpsd: pointer to data struct for holding read data read from gps device.
--                      QWidget* parent: pointer to the class' parent.
--
--              void GpsWindow::cleanup();
--              void GpsWindow::print_data();
--              void GpsWindow::menu_conn();
--              void GpsWindow::menu_diss();
--              
--               
-- NOTES:
-- This class displays and provides an interface for operating the GUI.
--------------------------------------*/
class GpsWindow : public QMainWindow
{
    Q_OBJECT

    public:
        GpsWindow(gpsdata*, QWidget* parent = 0);

        
    public slots:
        void cleanup();
    
        void print_data();
        
        void menu_conn() {
            conn_action->setEnabled(false);
            diss_action->setEnabled(true);
        }

        void menu_diss() {
            conn_action->setEnabled(true);
            diss_action->setEnabled(false);
        }
        
    private:
    
        // menu items
        QAction* quit_action;
        QAction* conn_action;
        QAction* diss_action;
    
        // satellite graphical display items
        QGraphicsEllipseItem* points[SAT_DISPLAY_SLOTS];
        QGraphicsTextItem       * labels[SAT_DISPLAY_SLOTS];
            
        // threading
        Thread* thread;
        gpsdata* gpsd;

        // ctor helpers
        void ctor_createThread();
        void ctor_useGridLayout();  
        void ctor_createMenuBar();
        void ctor_createTable(struct table&, const char[][10], int = 1);
        void ctor_createSatelliteView();
        
        // print_data() helpers
        void print_gps_data();
        void print_sat_data();
        void update_sat_view();
                
        // table widgets
        QTableWidget* sat_data;
        QTableWidget* gps_data;
        
        QGraphicsView * graphical_view;
        QGraphicsScene* graphical_scene;
        
        // layout management
        QGridLayout* gridLayout;
        QWidget* grid;
};

#endif
