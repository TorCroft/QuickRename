#pragma once

#include <nlohmann/json.hpp>

using json = nlohmann::json;

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

    bool confirm{};
    bool exitWhenDone{};

    void createConfigFile(const std::string& filename);
    void processJson(const json& Data);

public:
    Config(const std::string& filename = ".\\config.json");
    const std::string& getTargetDir() const;
    const std::vector<std::string>& getUnwantedExtensionList() const;
    const std::vector<std::string>& getStringDeleteList() const;
    const std::vector<std::pair<std::string, std::string>>& getStringReplaceList() const;
    const StrAddPatternConfig& getStringAddPattern() const;

    bool isUnwantedExtensionListEmpty() const;
    bool isStringDeleteListEmpty() const;
    bool isStringReplacePatternEmpty() const;
    bool isStringAddPatternEmpty() const;
    bool isConfirmEnabled() const;
    bool isExitWhenDoneEnabled() const;
};