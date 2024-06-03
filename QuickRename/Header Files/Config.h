#pragma once

#include <nlohmann/json.hpp>

using json = nlohmann::json;


class ConfigFile {
private:
    void createConfigFile(const std::string& filename);
    json global{};
    std::vector<json> profiles;

public:
    ConfigFile(const std::string& filename = ".\\config.json");
    json getGlobalConfig();
    std::vector<json> getProfiles();
};


class GlobalConfig {
private:
    bool confirm{};
    bool exitWhenDone{};

public:
    GlobalConfig(const json& globalConfig);
    bool isConfirmEnabled() const;
    bool isExitWhenDoneEnabled() const;
};


class Config {

private:
    struct StrAddPatternConfig {
        std::string match;
        std::string format;
        struct FormatConfig {
            int start;
            int step;
            FormatConfig() : start(1), step(1) {}
        } formatConfig;
        int position;

        StrAddPatternConfig() : position(0) {}
    };
    std::string targetDir;
    std::vector<std::string> unwantedExtensionList;
    std::vector<std::string> stringDeleteList;
    std::vector<std::pair<std::string, std::string>> stringReplaceList;

    StrAddPatternConfig stringAddPattern;

public:
    Config(const json& profile);
    const std::string& getTargetDir() const;
    const std::vector<std::string>& getUnwantedExtensionList() const;
    const std::vector<std::string>& getStringDeleteList() const;
    const std::vector<std::pair<std::string, std::string>>& getStringReplaceList() const;
    const StrAddPatternConfig& getStringAddPattern() const;

    bool isUnwantedExtensionListEmpty() const;
    bool isStringDeleteListEmpty() const;
    bool isStringReplacePatternEmpty() const;
    bool isStringAddPatternEmpty() const;
};