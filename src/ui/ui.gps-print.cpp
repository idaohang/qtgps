#include "ui.gps.h"
#include <cmath>

#define PI 3.1415926

/*------------------------------------
-- Method: GpsWindow::update_sat_view
-- Date:    November 12 2013
-- Designer:    Sam Youssef
-- Programmer: Sam Youssef
--
-- INTERFACE: void GpsWindow::update_sat_view(void)
-- RETURNS: void
--
-- NOTES:
-- Updates graphical satellite view (i.e. satellite position, etc.).
--------------------------------------*/
void GpsWindow::update_sat_view() {

    int nsats = gpsd->satellites_visible;
    
    for(int i = 0; i < nsats && i < SAT_DISPLAY_SLOTS; ++i) {
    
        qreal az = (gpsd->azimuth[i] - 90);
        qreal ev = gpsd->elevation[i];
        
        qreal r = 180 - (2 * ev);
        
        qreal x = r * cos(az * PI/180);
        qreal y = r * sin(az * PI/180);
        
        points[i]->setRect( 180 + x, 180 + y, 8, 8 );
        points[i]->setBrush( (gpsd->used[i] ? Qt::blue : Qt::black) );
        
        char buff[10];
        sprintf(buff, "%d", gpsd->PRN[i]);
        
        labels[i]->setPlainText(buff);
        labels[i]->setPos( 180 + x + 1, 180 + y + 1);
    }
    
    for(int i = nsats; i < SAT_DISPLAY_SLOTS; ++i) {
        points[i]->setRect( 0, 0, 0, 0);
        labels[i]->setPlainText("");
    }
}

/*------------------------------------
-- Method: GpsWindow::ctor_gps_data
-- Date:    November 12 2013
-- Designer:    Sam Youssef
-- Programmer: Sam Youssef
--
-- INTERFACE: void GpsWindow::print_gps_data(void)
-- RETURNS: void
--
-- NOTES:
-- Prints information about the fix to the gps table.
--------------------------------------*/
void GpsWindow::print_gps_data() {

    char bf0[MAX_BF+1] = { 0 };
    char bf1[MAX_BF+1] = { 0 };
    char bf2[MAX_BF+1] = { 0 };
        
    unix_to_iso8601(gpsd->fix.time, bf0, MAX_BF+1);
    sprintf(bf1, "%.4f", gpsd->fix.latitude);
    sprintf(bf2, "%.4f", gpsd->fix.longitude);
    
    gps_data->item(0, 0)->setText(bf0); 
    gps_data->item(0, 1)->setText(bf1);
    gps_data->item(0, 2)->setText(bf2);

}

/*------------------------------------
-- Method: GpsWindow::print_sat_data
-- Date:    November 12 2013
-- Designer:    Sam Youssef
-- Programmer: Sam Youssef
--
-- INTERFACE: void GpsWindow::print_sat_data(void)
-- RETURNS: void
--
-- NOTES:
-- Prints the satellite info to the sat table.
--------------------------------------*/
void GpsWindow::print_sat_data() {

    int  nsats = gpsd->satellites_visible;
    
    char bf0[MAX_BF+1] = { 0 };
    char bf1[MAX_BF+1] = { 0 }; 
    char bf2[MAX_BF+1] = { 0 }; 
    char bf3[MAX_BF+1] = { 0 };
        
    for(int i = 0; i < nsats && i < SAT_DISPLAY_SLOTS; ++i) {
    
        sprintf(bf0, "%d", gpsd->PRN[i]);
        sprintf(bf1, "%d", gpsd->elevation[i]);
        sprintf(bf2, "%d", gpsd->azimuth[i]);
        sprintf(bf3, "%.2f", gpsd->ss[i]);
                            
        sat_data->item(i, 0)->setText(bf0);
        sat_data->item(i, 1)->setText(bf1);
        sat_data->item(i, 2)->setText(bf2);
        sat_data->item(i, 3)->setText(bf3);             
        sat_data->item(i, 4)->setText(gpsd->used[i] ? "Y" : "N");
    }
    
    for(int i = nsats; i < SAT_DISPLAY_SLOTS; ++i) {
        sat_data->item(i, 0)->setText("");
        sat_data->item(i, 1)->setText("");
        sat_data->item(i, 2)->setText("");
        sat_data->item(i, 3)->setText("");              
        sat_data->item(i, 4)->setText("");      
    }
}

/*------------------------------------
-- Method: GpsWindow::print_data
-- Date:    November 12 2013
-- Designer:    Sam Youssef
-- Programmer: Sam Youssef
--
-- INTERFACE: void GpsWindow::print_data(void)
-- RETURNS: void
--
-- NOTES:
-- Meta-method for calling the print and update methods together.
--------------------------------------*/
void GpsWindow::print_data() {

    print_gps_data();
    print_sat_data();
    update_sat_view();
}
