#include "sensorparameters.h"

SensorParameters::SensorParameters()
{
    // Add parameters
    Parameter p;

    // sensor3.1
    p.accelerationResolution    = 1 / 2048.0 * 1000;
    p.angularVelocityResolution = 1 / 16.4;
    p.magneticFieldResolution   = 0.3;
    p.pressureResolution        = 0;
    p.temperatureResolution     = 0;
    p.xUnit = 1 / 10000.0;
    p.name = "sensor3.1";

    parameters[p.name] = p;
}

SensorParameters::Parameter SensorParameters::getParameter(std::string name)
{
    if ( parameters.find( name ) == parameters.end() ) {
        Parameter p;

        p.accelerationResolution    = 0;
        p.angularVelocityResolution = 0;
        p.magneticFieldResolution   = 0;
        p.pressureResolution        = 0;
        p.temperatureResolution     = 0;
        p.xUnit = 1;
        p.name = "null";

        return p;
    }

    return parameters[name];
}

std::list<std::string> SensorParameters::sensorNameList()
{
    // return list of sensor names
    std::list<std::string> names;

    for ( const auto &elem : parameters ) {
        names.push_back( elem.second.name );
    }

    return names;
}

