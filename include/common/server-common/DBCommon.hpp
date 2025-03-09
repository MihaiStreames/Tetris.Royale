#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <sstream>
#include <boost/property_tree/json_parser.hpp>

// Generates a UUID string
std::string generateUUID();

// Returns the SHA-256 hash of the given input
std::string sha256Hash(const std::string &input);

// Converts a Boost.PropertyTree ptree to a JSON-formatted string
std::string buildJSON(const boost::property_tree::ptree &pt);

// These functions use Boost.PropertyTree to parse a JSON array stored as a string,
// add a value if it does not exist, or remove a value if present
std::string addToJsonArray(const std::string &jsonArray, const std::string &value);
std::string removeFromJsonArray(const std::string &jsonArray, const std::string &value);