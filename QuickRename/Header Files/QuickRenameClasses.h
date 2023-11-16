#pragma once
#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <nlohmann/json.hpp>
#include <filesystem>

using json = nlohmann::json;

class File {
private:
    std::filesystem::path path;
    std::string name;
    std::string extension;
    std::string new_name;

public:
    File(const std::filesystem::path& p, const std::string& n, const std::string& ext) : path(p), name(n), extension(ext) {
        new_name = n;
    }

    void set_new_name(const std::string& new_n);

    std::string get_name() const;
    std::string get_extension() const;
    std::string get_new_name() const;
    std::string get_new_full_name() const;
    std::string get_full_name() const;
    std::filesystem::path get_path() const;
    std::filesystem::path get_new_name_path() const;

    bool is_name_changed();
    bool apply_new_name();
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
    std::string target_dir;
    std::vector<std::string> unwantedExtensionList;
    std::vector<std::string> stringDeleteList;
    std::vector<std::pair<std::string, std::string>> stringReplaceList;

    StrAddPatternConfig stringAddPattern;

    bool confirm{};

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
};

class TaskHandler {
public:
    TaskHandler(const Config& config);

    void executeTasks();

private:
    void processFiles(const std::function<void(File&)>& action);
    std::vector<File> GetFileVector(const std::filesystem::path& directory = ".");
    bool deleteFile(const std::filesystem::path& filePath);
    std::string deleteSubString(const std::string& input, const std::string& target);
    std::string generateNewName(const std::string& prefix, const std::string& suffix, const int& number_width, int& number, const int& step = 1);

    void getTasks();
    void processUnwantedExtensions();
    void processStringDeleteList();
    void processStringReplacePattern();
    void processStringAddPattern();
    void showChanges();
    void applyChanges();

    const Config& config;
    std::vector<File> files;
    std::vector<File> files_name_changed;
    std::vector<File> to_be_deleted;

    using TaskFunction = std::function<void()>;
    std::vector<TaskFunction> tasks;
};