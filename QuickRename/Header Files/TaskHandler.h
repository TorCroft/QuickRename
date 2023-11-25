#pragma once
#include <Config.h>
#include <File.h>

class TaskHandler {
public:
    TaskHandler(const Config& config);

    void executeTasks();

private:
    void processFiles(const std::function<void(File&)>& action);
    std::vector<File> GetFileVector(const std::filesystem::path& directory = ".");
    void deleteFile(const std::filesystem::path& filePath);
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
    std::vector<File> nameChangedFiles;
    std::vector<File> filesToDelete;

    using TaskFunction = std::function<void()>;
    std::vector<TaskFunction> tasks;
};