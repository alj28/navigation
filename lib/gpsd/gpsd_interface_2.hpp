#ifndef _GPSD_INTERFACE_H_
#define _GPSD_INTERFACE_H_

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <math.h>
#include <thread> 
#include <memory>

#include <gps.h>

struct gps_data
{
    float longitude;
    float latitude;
};

#define MODE_STR_NUM 4
static char *mode_str[MODE_STR_NUM] = {
    "n/a",
    "None",
    "2D",
    "3D"
};

/**
 * @class gpsd_interface
 * @brief A wrapper for the GPS library
 *
 *  https://gpsd.gitlab.io/gpsd/libgps.html 
 */
class gpsd_interface
{
public:
    gpsd_interface(char* server, char* port)
    {
        if (0 != gps_open(server, port, &this->lib_handler))
        {
            std::ostringstream msg;
            msg << "Cannot open gpsd_interface opened on " << server << ":" << port << " opened.";
            throw std::runtime_error(msg.str());
        }
        std::cout << "gpsd_interface opened on " << server << ":" << port << " opened." << std::endl; 
        (void)gps_stream(&this->lib_handler, (WATCH_ENABLE | WATCH_JSON), NULL);
    }

    gpsd_interface(): gpsd_interface("localhost", "2947") {};

    ~gpsd_interface()
    {
        std::cout << "gpsd_interface deconstructor." << std::endl;
        (void)gps_stream(&this->lib_handler, WATCH_DISABLE, NULL);
        (void)gps_close(&this->lib_handler);
    }

public:

    void start_thread(void)
    {
        //if (NULL != this->thread_handle)
        //{
        //    return;
        //}
        this->do_run_thread = true;
        this->thread_handle = std::make_unique<std::thread>(&gpsd_interface::worker, this);
        
    }

    void stop_thread(void)
    {
        if (NULL == this->thread_handle)
        {
            return;
        }
        this->do_run_thread = false;
        this->thread_handle->join();
        this->thread_handle.reset();
    }

    void worker(void)
    {
        std::cout << "worker thread" << std::endl;
        while (gps_waiting(&this->lib_handler, 5000000) && do_run_thread) {
            if (-1 == gps_read(&this->lib_handler, NULL, 0)) {
                std::cerr << "Read error.  Bye, bye" << std::endl;
                break;
            }
            if (MODE_SET != (MODE_SET & this->lib_handler.set)) {
                // did not even get mode, nothing to see here
                continue;
            }
            if (0 > this->lib_handler.fix.mode ||
                MODE_STR_NUM <= this->lib_handler.fix.mode) {
                this->lib_handler.fix.mode = 0;
            }
            printf("Fix mode: %s (%d) Time: ",
                   mode_str[this->lib_handler.fix.mode],
                   this->lib_handler.fix.mode);
            if (TIME_SET == (TIME_SET & this->lib_handler.set)) {
                // not 32 bit safe
                printf("%ld.%09ld ", this->lib_handler.fix.time.tv_sec,
                       this->lib_handler.fix.time.tv_nsec);
            } else {
                puts("n/a ");
            }
            if (isfinite(this->lib_handler.fix.latitude) &&
                isfinite(this->lib_handler.fix.longitude)) {
                // Display data from the GPS receiver if valid.
                printf("Lat %.6f Lon %.6f\n",
                       this->lib_handler.fix.latitude, this->lib_handler.fix.longitude);
            } else {
                printf("Lat n/a Lon n/a\n");
            }
        }
        
    }

protected:
    struct gps_data_t lib_handler;
    std::unique_ptr<std::thread> thread_handle;
    bool do_run_thread;
};

#endif /* _GPSD_INTERFACE_H_ */