#ifndef SENSORPARAMETERS_H
#define SENSORPARAMETERS_H

#include <vector>
#include <list>
#include <string>
#include <map>

class SensorParameters
{
public:
    struct Parameter {
        double accelerationResolution;
        double angularVelocityResolution;
        double magneticFieldResolution;
        double pressureResolution;
        double temperatureResolution;
        double xUnit;
        std::string name;
    };

public:
    SensorParameters();

    Parameter getParameter( std::string name );
    std::list<std::string> sensorNameList();

private:
    std::map<std::string, Parameter> parameters;
};

#endif // SENSORPARAMETERS_H
