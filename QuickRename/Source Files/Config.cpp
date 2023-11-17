#include "QuickRenameClasses.h"
#include <fstream>
#include <iostream>

// Exits the program with an error message and a prompt to press Enter.
static void exitWithError(const std::string& errorMessage) {
    std::cerr << "Error: " << errorMessage << std::endl;
    std::cout << "Press Enter to exit." << std::endl;
    std::cin.get();
    std::exit(EXIT_FAILURE);
}

// Constructor for Config, loads configuration data from a JSON file.
Config::Config(const std::string& filename) {
    json configData;
    std::filesystem::path filePath = std::filesystem::absolute(filename);

    if (std::filesystem::exists(filePath)) {
        // Attempt to open the file
        std::ifstream file(filePath, std::ios::binary);

        if (file.is_open()) {
            std::cout << "Config Loaded: " << filePath << std::endl;

            // Read and parse the JSON data
            file >> configData;
            file.close();

            // Process the JSON data
            processJson(configData);
        }
        else {
            // Handle error if unable to open the file
            exitWithError("Error opening file, provided file path: " + filePath.string());
        }
    }
    else {
        // Handle error if the config file is not found
        createConfigFile(filename);
        std::stringstream msg;
        msg << "Config File Not Found, QuickRename auto created config.json: " << filePath.string() << std::endl;
        msg << "Please edit config.json and then run QuickRename again." << std::endl;
        exitWithError(msg.str());
    }
}

// Processes configuration data from the provided JSON object.
void Config::processJson(const json& Data) {
    // Load configuration settings from JSON data
    confirm = Data["confirm"].get<bool>();

    target_dir = Data["target_dir"].get<std::string>();
    if (target_dir.empty()) {
        target_dir = ".";
    }

    unwantedExtensionList = Data["unwanted_extension"].get<std::vector<std::string>>();

    for (auto it = unwantedExtensionList.begin(); it != unwantedExtensionList.end(); ) {
        if (it->empty()) {
            it = unwantedExtensionList.erase(it);
            continue;
        }
        else if ((*it)[0] != '.') {
            *it = "." + *it;
        }
        ++it;
    }

    stringDeleteList = Data["string_delete"].get<std::vector<std::string>>();

    for (const auto& entry : Data["string_replace_pattern"]) {
        try {
            std::regex regexObj(entry["re_match"].get<std::string>());
            stringReplaceList.push_back({ entry["re_match"].get<std::string>(), entry["replace"].get<std::string>() });
        }
        catch (const std::regex_error& e) {
            std::cerr << "Error in regular expression: \"" + entry["re_match"].get<std::string>() + "\"\nDetail: " << e.what() << std::endl;
        }
    }

    // Load and process string add pattern if present in the JSON data
    if (Data.find("string_add_pattern") != Data.end()) {
        const auto& strAddPattern = Data["string_add_pattern"];
        stringAddPattern.match = strAddPattern["re_match"].get<std::string>();
        stringAddPattern.format = strAddPattern["format"].get<std::string>();
        stringAddPattern.formatConfig.start = strAddPattern["format_config"]["start"].get<int>();
        stringAddPattern.formatConfig.step = strAddPattern["format_config"]["step"].get<int>();
        stringAddPattern.position = strAddPattern["position"].get<int>();
    }
}

void Config::createConfigFile(const std::string& filename) {
    // Named constants for default values
    const bool defaultConfirm = true;
    const std::string defaultTargetDir = "";
    const json defaultUnwantedExtension = json::array();
    const json defaultStringDelete = json::array();
    const json defaultStringReplacePattern = {{"re_match", ""}, {"replace", ""}};
    const json defaultStringAddPattern = {
        {"re_match", ""},
        {"format", ""},
        {"format_config", {{"start", 1}, {"step", 1}}},
        {"position", 0}
    };

    // Constructing the JSON configuration
    json config;
    config["confirm"] = defaultConfirm;
    config["target_dir"] = defaultTargetDir;
    config["unwanted_extension"] = defaultUnwantedExtension;
    config["string_delete"] = defaultStringDelete;
    config["string_replace_pattern"] = json::array({ defaultStringReplacePattern });
    config["string_add_pattern"] = defaultStringAddPattern;

    // Writing the JSON to the file
    std::ofstream file(filename);
    if (file.is_open()) {
        file << std::setw(4) << config;
        std::cout << "Config file '" << filename << "' created successfully." << std::endl;
    }
    else {
        std::stringstream msg;
        msg << "Unable to create config file \"" << filename <<"\"." << std::endl;
        msg << "Please check permissions or disk space." << std::endl;
        exitWithError(msg.str());
    }
}

const std::string& Config::getTargetDir() const {
    return target_dir;
}

const std::vector<std::string>& Config::getUnwantedExtensionList() const {
    return unwantedExtensionList;
}

const std::vector<std::string>& Config::getStringDeleteList() const {
    return stringDeleteList;
}

const std::vector<std::pair<std::string, std::string>>& Config::getStringReplaceList() const {
    return stringReplaceList;
}

const Config::StrAddPatternConfig& Config::getStringAddPattern() const {
    return stringAddPattern;
}

bool Config::isUnwantedExtensionListEmpty() const {
    return unwantedExtensionList.empty();
}

bool Config::isStringDeleteListEmpty() const {
    return stringDeleteList.empty();
}

bool Config::isStringReplacePatternEmpty() const {
    return stringReplaceList.empty() || stringReplaceList[0].first.empty();
}

bool Config::isStringAddPatternEmpty() const {
    return stringAddPattern.format.empty();
}

bool Config::isConfirmEnabled() const {
    return confirm;
}
