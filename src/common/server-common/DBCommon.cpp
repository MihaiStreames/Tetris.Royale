#include "DBCommon.hpp"

using boost::property_tree::ptree;

std::string
addToJsonArray(const std::string &jsonArray, const std::string &value) {
    ptree arr;
    std::istringstream iss(jsonArray);

    try {
        read_json(iss, arr);
    } catch (...) {
        arr.clear(); // assume an empty array if parsing fails
    }

    bool exists = false;
    for (const auto &child: arr) {
        if (child.second.get_value<std::string>() == value) {
            exists = true;
            break;
        }
    }

    if (!exists) {
        ptree elem;
        elem.put("", value);
        arr.push_back(std::make_pair("", elem));
    }

    std::ostringstream oss;
    write_json(oss, arr, false);
    return oss.str();
}

std::string
removeFromJsonArray(const std::string &jsonArray, const std::string &value) {
    ptree arr;
    std::istringstream iss(jsonArray);

    try {
        read_json(iss, arr);
    } catch (...) {
        arr.clear();
    }

    ptree newArr;
    for (const auto &child: arr) {
        if (child.second.get_value<std::string>() != value) {
            newArr.push_back(std::make_pair("", child.second));
        }
    }

    std::ostringstream oss;
    write_json(oss, newArr, false);
    return oss.str();
}

std::string
generateUUID() {
    boost::uuids::random_generator generator;
    const boost::uuids::uuid uuid = generator();
    return to_string(uuid);
}

std::string
sha256Hash(const std::string &input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char *>(input.c_str()), input.size(),
           hash);

    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    for (const unsigned char i: hash) {
        ss << std::setw(2) << static_cast<int>(i);
    }

    return ss.str();
}

std::string
buildJSON(const boost::property_tree::ptree &pt) {
    std::ostringstream oss;
    write_json(oss, pt, false);
    return oss.str();
}
