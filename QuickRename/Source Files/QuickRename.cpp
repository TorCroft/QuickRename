#include <QuickRename.h>
#include <iostream>


int main(int argc, char* argv[]) {    
    // Set the locale to Chinese (Simplified) UTF-8 for proper character encoding
    setlocale(LC_ALL, "zh_CN.UTF-8");

    if (argc > 0) {
        std::string fullPath = argv[0];
        size_t lastSlash = fullPath.find_last_of("/\\");
        self_file_name = (lastSlash != std::string::npos) ? fullPath.substr(lastSlash + 1) : fullPath;
    }
    
    ConfigFile configFile;
    GlobalConfig global(configFile.getGlobalConfig());
    std::vector<json> profiles = configFile.getProfiles();

    std::cout << profiles.size() << " profiles configured." << std::endl;

    for (const auto& profile : profiles) {
        Config config(profile);
        TaskHandler task_handler(global, config);
        task_handler.executeTasks();
    }

    return 0;
}
