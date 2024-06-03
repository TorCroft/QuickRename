#include <QuickRename.h>
#include <iostream>
#include <fstream>
#include <regex>


// Exits the program with a prompt to press Enter.
static void exitWithFailure() {
    std::cout << "Press Enter to exit." << std::endl;
    std::cin.get();
    std::exit(EXIT_FAILURE);
}


ConfigFile::ConfigFile(const std::string& filename) {
    json configData;
    std::filesystem::path filePath = std::filesystem::absolute(filename);

    if (std::filesystem::exists(filePath)) {
        // Attempt to open the file
        std::ifstream file(filePath, std::ios::binary);

        if (file.is_open()) {
            std::cout << "Config Loaded: " << filePath << "\t";

            // Read and parse the JSON data
            try {
                file >> configData;
            }
            catch (const json::parse_error& e) {
                std::cerr << "JSON parse error: " << e.what() << " at byte position " << e.byte << std::endl;
                exitWithFailure();
            }

            file.close();

            // Process the JSON data
            global = configData["global"].get<json>();
            profiles = configData["profiles"].get<std::vector<json>>();

        }
        else {
            // Handle error if unable to open the file
            std::cerr << "Error opening file, provided file path: " << filePath.string();
            exitWithFailure();
        }
    }
    else {
        // Handle error if the config file is not found
        createConfigFile(filename);
        std::cerr << "Config File Not Found, QuickRename auto created config.json: " << filePath.string() << std::endl << "Please edit config.json and then run QuickRename again." << std::endl;
        exitWithFailure();
    }
}

json ConfigFile::getGlobalConfig() {
    return global;
}

std::vector<json> ConfigFile::getProfiles() {
    return profiles;
}

void ConfigFile::createConfigFile(const std::string& filename) {
    // Named constants for default values
    const json defaultStringReplacePattern = { {"re_match", ""}, {"replace", ""} };
    const json defaultStringAddPattern = {
        {"re_match", ""},
        {"format", ""},
        {"format_config", {{"start", 1}, {"step", 1}}},
        {"position", 0}
    };

    // Constructing the JSON configuration
    json config;
    json globalConfig;
    globalConfig["confirm"] = true;
    globalConfig["exit_when_done"] = false;
    config["global"] = globalConfig;

    json profile;
    profile["target_dir"] = "";
    profile["unwanted_extension"] = json::array();
    profile["string_delete"] = json::array();
    profile["string_replace_pattern"] = json::array({ defaultStringReplacePattern });
    profile["string_add_pattern"] = defaultStringAddPattern;

    config["profiles"] = json::array({ profile });

    // Writing the JSON to the file
    std::ofstream file(filename);
    if (file.is_open()) {
        file << std::setw(4) << config;
        std::cout << "Config file '" << filename << "' created successfully." << std::endl;
    }
    else {
        std::cerr << "Unable to create config file '" << filename << "'." << std::endl << "Please check permissions or disk space." << std::endl;
        exitWithFailure();
    }
}

// Constructor for Config, loads configuration data from a JSON file.
Config::Config(const json& profile) {
    // Process profile
    targetDir = profile["target_dir"].get<std::string>();
    if (targetDir.empty()) {
        targetDir = ".";
    }

    unwantedExtensionList = profile["unwanted_extension"].get<std::vector<std::string>>();

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

    stringDeleteList = profile["string_delete"].get<std::vector<std::string>>();

    for (const auto& entry : profile["string_replace_pattern"]) {
        try {
            std::regex regexObj(entry["re_match"].get<std::string>());
            stringReplaceList.push_back({ entry["re_match"].get<std::string>(), entry["replace"].get<std::string>() });
        }
        catch (const std::regex_error& e) {
            std::cerr << "Error in regular expression: \"" + entry["re_match"].get<std::string>() + "\"\nDetail: " << e.what() << std::endl;
        }
    }

    // Load and process string add pattern if present in the JSON data
    if (profile.find("string_add_pattern") != profile.end()) {
        const auto& strAddPattern = profile["string_add_pattern"];
        stringAddPattern.match = strAddPattern["re_match"].get<std::string>();
        stringAddPattern.format = strAddPattern["format"].get<std::string>();
        stringAddPattern.formatConfig.start = strAddPattern["format_config"]["start"].get<int>();
        stringAddPattern.formatConfig.step = strAddPattern["format_config"]["step"].get<int>();
        stringAddPattern.position = strAddPattern["position"].get<int>();
    }
}

const std::string& Config::getTargetDir() const {
    return targetDir;
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

GlobalConfig::GlobalConfig(const json& globalConfig) {
    confirm = globalConfig["confirm"].get<bool>();
    exitWhenDone = globalConfig["exit_when_done"].get<bool>();
}

bool GlobalConfig::isConfirmEnabled() const {
    return confirm;
}

bool GlobalConfig::isExitWhenDoneEnabled() const {
    return exitWhenDone;
}
