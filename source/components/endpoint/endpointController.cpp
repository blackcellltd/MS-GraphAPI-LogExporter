#include "endpointController.hpp"

#include <memory>
#include <vector>

#include "endpointObj.hpp"
#include "../../providers/config/config.hpp"
#include "../../providers/accessToken/accessToken.hpp"
#include "../timmer/timmer.hpp"

namespace EndpointController
{
    static void makeEndpoints();

    static std::vector<std::shared_ptr<EndpointObj>> endpoints;
    static std::shared_ptr<AccessToken> accessToken;

    extern void launch()
    {
        logger.information("Endpoint Controller: starting...");

        makeEndpoints();
        Timmer::init();

        for (std::shared_ptr<EndpointObj> &endpoint : endpoints)
            Timmer::addTarget(std::static_pointer_cast<Timmer::Timmerable>(endpoint));

        Timmer::start(config.getInt("Access Intervals") * TIME_SECOND);
    }

    static void makeEndpoints()
    {
        EndpointController::accessToken = std::make_shared<AccessToken>();
        logger.success("Endpoint Controller: Access Token generated");

        const std::vector<Config::EndpointConf> endpointsConf =
            config.getEndpointConfigurations();

        for (const Config::EndpointConf &endpointConf : endpointsConf)
        {
            logger.information("Endpoint Controller: making endpoint [ " + endpointConf.name + " ]");

            EndpointController::endpoints.push_back(
                std::make_shared<EndpointObj>(
                    endpointConf,
                    EndpointController::accessToken));

            logger.information("Endpoint Controller: successfully made endpoint [ " + endpointConf.name + " ]");
        }
    }

} // namespace EndpointController