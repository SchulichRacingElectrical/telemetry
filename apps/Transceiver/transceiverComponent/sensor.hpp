// Copyright Schulich Racing, FSAE
// Written by Justin Tijunelis

#ifndef SENSOR_H
#define SENSOR_H

#include <string>
#include <variant>
#include "../inc/json.hpp"

using json = nlohmann::json;

/**
 * @brief This represents all the possible data types our system
 *  can read. Based on delegation with the team, we found
 *  that these datatypes can represent all of our sensors.
 *  In the future, this list may be expanded.
 */
using SensorDataVariant = 
  std::variant< 
    bool, 
    char, 
    unsigned char, 
    short, 
    unsigned short, 
    int, 
    unsigned int, 
    float, 
    long long, 
    unsigned long long, 
    double 
  >;

/**
 * @brief A tuple (sensor ID, SensorVariantData). 
 * TODO: We should just make this a pair.
 */
using SensorVariantPair = std::tuple<unsigned char, SensorDataVariant>;

/**
 * @brief Enumeration used to deduce sensor type from the database. May be expanded but 
 * the expansion must be added to SensorDataVariant. 
 * TODO: Check the os version for size
 */
enum class SensorType : char {
  BOOL = '?',
  BYTE = 'c',
  UNSIGNED_BYTE = 'B',
  SHORT = 'h',
  UNSIGNED_SHORT = 'H',
  INT = 'i',
  UNSIGNED_INT = 'I',
  FLOAT = 'f',
  LONG_LONG = 'q',
  UNSIGNED_LONG_LONG = 'Q',
  DOUBLE = 'd',
};

/**
 * @brief Effectively a struct. Represents sensors on a "Thing"; used to create channels to 
 * read data from. These sensors are stored on a remote database and are fetched when a 
 * "Thing" is created. 
 */
class Sensor {
public:
  /**
   * @brief A JSON object containing sensor information. JSON is used as the sensor
   * fields are subject to change.
   * Currently, the json has the following format:
   * traits: {
   *    name: string, (A unique name of the sensor)
   *    id: unsigned char, (A unique identifier for the sensor, used for compression when sending data)
   *    type: char, (An character used to specify what type of data the sensor outputs; used to decode data appropriately)
   *    lastUpdate: unsigned long int, (A timestamp specifying the last time this sensor was fetched from the database, used to figure out which sensors to fetch data for)
   *    frequency: unsigned char, (The frequency that the sensor sends updated data at)
   *    channelId: unsigned int, (The channel id through which the sensor data is sent, used to determine how the data is read for hardware, for CAN, used as a CAN ID)
   *    upperCalib: double, (The upper calibration level for the sensor)
   *    lowerCalib: double, (The lower calibration level for the sensor)
   *    upperBound: double, (The max data value the sensor can have)
   *    lowerBound: double, (The min data value the sensor can have)
   * }
   */
  json traits;

  Sensor(){};
  Sensor(json t) : traits(t) {}
  Sensor(std::vector<std::string> keys, std::vector<std::string> values);

  /**
   * @brief Returns a variant that can be used to deduce the sensor's data type.
   */
  SensorDataVariant getVariant() const;
};

#endif