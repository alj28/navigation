#ifndef _GPSD_INTERFACE_H_
#define _GPSD_INTERFACE_H_

#include <thread>
#include <memory>
#include <string>

#include "spdlog/spdlog.h"
#include "gps.h"
#include "errors.h"

#include <gps.h>
#include <math.h>

#define MODE_STR_NUM 4

struct gps_info
{
    double latitude;
    double longitude;
    uint32_t sec;
    uint32_t nsec;
};

class gps_evt_hanlders
{
public:
    virtual ~gps_evt_hanlders() = default;
    virtual void on_new_data_received(struct gps_info data) = 0;
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
    gpsd_interface(std::string host, std::string port, std::weak_ptr<gps_evt_hanlders> handlers): 
        evt_handlers(std::move(handlers)),
        host(host),
        port(port)
    {
        spdlog::debug("gspd_interface constructor: {:s}:{:s}", host, port);
    }
    gpsd_interface(std::weak_ptr<gps_evt_hanlders> handlers): 
        gpsd_interface("localhost", "2947", handlers) {};
    gpsd_interface(): 
        gpsd_interface("localhost", "2947", std::weak_ptr<gps_evt_hanlders>{}) {};

    ~gpsd_interface()
    {
        spdlog::debug("gspd_interface deconstructor");
        stop();
    }

    ErrorCode start(void)
    {
        ErrorCode rv = stop();
        if (ErrorCode::Ok != rv)
        {
            return rv;
        }

        thread_running = true;
        thread_handle = std::thread(&gpsd_interface::worker, this);
        return ErrorCode::Ok;
    }

    ErrorCode stop(void)
    {
        if (false == thread_running)
        {
            return ErrorCode::Ok;
        }

        thread_running = false;
        if (thread_handle.joinable())
        {
            thread_handle.join();
        }
        return ErrorCode::Ok;
    }

protected:
    std::string host;
    std::string port;

    std::weak_ptr<gps_evt_hanlders> evt_handlers;

    std::thread thread_handle;
    std::atomic<bool> thread_running{false};

    struct gps_data_t gps_data{0};

    void worker(void)
    {
        spdlog::debug("gspd_interface thread started.");

        if (0 != gps_open(host.c_str(), port.c_str(), &gps_data))
        {
            spdlog::error("Cannot open gpsd_interface on {:s}:{:s}.", host, port);
            return;
        }

        (void)gps_stream(&gps_data, (WATCH_ENABLE | WATCH_JSON), NULL);

        while(thread_running)
        {
            if (false == gps_waiting(&gps_data, 5000000))
            {
                continue;
            }

            if (-1 == gps_read(&gps_data, NULL, 0))
            {
                spdlog::error("GPS data read failed. gspd_interface thread exiting...");
                break;
            }

            if (MODE_SET != (MODE_SET & gps_data.set)) {
                // did not even get mode, nothing to see here
                continue;
            }

            if (0 > gps_data.fix.mode ||
                MODE_STR_NUM <= gps_data.fix.mode) {
                gps_data.fix.mode = 0;
            }

            struct gps_info data{0};
            data.sec = gps_data.fix.time.tv_sec;
            data.nsec = gps_data.fix.time.tv_nsec;
            data.latitude = gps_data.fix.latitude;
            data.longitude = gps_data.fix.longitude;

            if (auto h = evt_handlers.lock())
            {
                h->on_new_data_received(data);
            }
        }

        thread_running = false;

        (void)gps_stream(&gps_data, WATCH_DISABLE, NULL);
        (void)gps_close(&gps_data);

        spdlog::debug("gspd_interface thread exit.");
    }
    
};


#endif /* _GPSD_INTERFACE_H_ */
