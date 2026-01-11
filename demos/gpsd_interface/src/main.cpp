
#include <iostream>
#include <memory>
#include <chrono>

#include <gpsd_interface.hpp>

#include "spdlog/spdlog.h"

class evt_hanlders: public gps_evt_hanlders
{
public:
    void on_new_data_received(struct gps_info data)
    {
        spdlog::info("Latitude: {:.06f}, Longitude: {:.06f}", data.latitude, data.longitude);
    }
};

int main(void)
{
    spdlog::info("gsp_interface");
    spdlog::set_level(spdlog::level::debug);
    std::shared_ptr<evt_hanlders> evt_hnd = std::make_shared<evt_hanlders>();
    std::unique_ptr<gpsd_interface> obj = std::make_unique<gpsd_interface>(evt_hnd);
    obj->start();
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    evt_hnd.reset();
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    
    return 0;
}

