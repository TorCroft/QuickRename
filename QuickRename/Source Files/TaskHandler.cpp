#include <QuickRename.h>
#include <iostream>
#include <format>
#include <regex>


static void confirmWithMsg(const std::string& message) {
    std::cout << "\n" << message;
    std::cin.get();
}

// Processes each file with the specified action function.
void TaskHandler::processFiles(const std::function<void(File&)>& action) {
    for (File& file : files) {
        action(file);
    }
}

// Constructor for TaskHandler, initializes configuration and retrieves file list.
TaskHandler::TaskHandler(const GlobalConfig& globalConfig, const Config& config) : global(globalConfig), config(config) {
    std::filesystem::path dir = std::filesystem::absolute(config.getTargetDir());
    if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)) {
        // targetDir exists and is a directory
        std::cout << "\nTarget Directory: " << dir << std::endl;
        files = GetFileVector(dir);
        getTasks();
    }
    else {
        // targetDir does not exist or is not a directory
        std::cerr << "Error: target directory does not exist or is not a valid directory." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

// Populate 'tasks' vector with function pointers based on configured actions.
void TaskHandler::getTasks() {

    if (!config.isUnwantedExtensionListEmpty()) {
        tasks.emplace_back(std::bind(&TaskHandler::processUnwantedExtensions, this));
    }

    if (!config.isStringDeleteListEmpty()) {
        tasks.emplace_back(std::bind(&TaskHandler::processStringDeleteList, this));
    }

    if (!config.isStringReplacePatternEmpty()) {
        tasks.emplace_back(std::bind(&TaskHandler::processStringReplacePattern, this));
    }

    if (!config.isStringAddPatternEmpty()) {
        tasks.emplace_back(std::bind(&TaskHandler::processStringAddPattern, this));
    }

    tasks.emplace_back(std::bind(&TaskHandler::applyChanges, this));
}

// Executes the tasks stored in the 'tasks' vector.
void TaskHandler::executeTasks() {
    for (const auto& task : tasks) {
        task();
    }
}

// Retrieves a vector of File objects representing regular files in the specified directory.
std::vector<File> TaskHandler::GetFileVector(const std::filesystem::path& directory)
{
    std::vector<File> directory_files;
    std::filesystem::path path;

    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            path = entry.path();
            // Creating and adding File object to the vector
            directory_files.emplace_back(std::move(path), std::move(path.filename().stem().string()), std::move(path.filename().extension().string()));
        }
    }

    // Remove config.json from directory_files if target_dir is current directory.
    if (std::filesystem::equivalent(directory, std::filesystem::current_path())) {
        const std::string configFileName = "config.json";
        auto it = std::remove_if(directory_files.begin(), directory_files.end(),
            [&configFileName](const File& file) {
                std::string fullname = file.get_full_name();
                return fullname == configFileName || fullname == self_file_name;
            });

        directory_files.erase(it, directory_files.end());
    }

    return directory_files;
}

// Deletes the specified file at the given filePath.
// Returns true if the file is successfully deleted, false otherwise.
void TaskHandler::deleteFile(const std::filesystem::path& filePath) {
    try {
        std::filesystem::remove(filePath);
        std::cout << "File deleted: " << filePath << std::endl;
    }
    catch (const std::exception& e) {
        // Handle and log any exceptions that occur during file deletion.
        std::cerr << "Error deleting file " << filePath << ": " << e.what() << std::endl;
    }
}

// Deletes all occurrences of the target substring in the input string.
// Returns the modified string without the target substring.
std::string TaskHandler::deleteSubString(const std::string& input, const std::string& target) {
    std::string result;

    // Iterate through the input string to find and delete target substring
    for (auto it = input.begin(); it != input.end();) {
        auto pos = std::search(it, input.end(), target.begin(), target.end());
        result.append(it, pos);

        // If target substring found, move iterator past it
        if (pos != input.end()) {
            it = pos + target.length();
        }
        else {
            // If target substring not found, exit the loop
            it = input.end();
        }
    }

    return result;
}

// Delete files with unwanted extension.
void TaskHandler::processUnwantedExtensions() {
    const std::vector<std::string>& unwantedExtensions = config.getUnwantedExtensionList();

    processFiles([&](File& file) {
        if (std::find(unwantedExtensions.begin(), unwantedExtensions.end(), file.get_extension()) != unwantedExtensions.end())
            filesToDelete.emplace_back(file);
        });

    // Remove files with unwanted extensions from the files vector
    files.erase(std::remove_if(files.begin(), files.end(), 
        [&unwantedExtensions](const File& file) {
            return std::find(unwantedExtensions.begin(), unwantedExtensions.end(), file.get_extension()) != unwantedExtensions.end();
        }), files.end());

}

// String delete task.
// Processes the string delete list for each file, removing specified substrings from the new names.
void TaskHandler::processStringDeleteList() {
    const std::vector<std::string>& stringDeleteList = config.getStringDeleteList();

    // Iterate through files and apply string deletions
    processFiles([&](File& file) {
        std::string temp = file.get_new_name();

        // Remove substrings listed in the string delete list
        for (const auto& entry : stringDeleteList) {
            temp = deleteSubString(temp, entry);
        }

        // Update file's new name if modifications were made
        if (temp != file.get_new_name()) {
            file.set_new_name(temp);
        }
        });
}

// String replace task.
// Processes the string replace pattern list for each file, replacing specified patterns in the new names.
void TaskHandler::processStringReplacePattern() {
    const auto& stringReplaceList = config.getStringReplaceList();

    // Iterate through files and apply string replacements using regex
    processFiles([&](File& file) {
        std::string temp = file.get_new_name();

        // Replace patterns listed in the string replace pattern list
        for (const auto& entry : stringReplaceList) {
            try {
                temp = std::regex_replace(temp, std::regex(entry.first), entry.second);
            }
            catch (const std::regex_error& e) {
                std::cerr << "Regex Error: " << e.what() << std::endl;
            }
        }

        // Update file's new name if modifications were made
        if (temp != file.get_new_name()) {
            file.set_new_name(temp);
        }
        });
}

// Generates a new name based on the provided prefix, suffix, number width, and step.
// Updates the number for the next generation based on the step value.
std::string TaskHandler::generateNewName(const std::string& prefix, const std::string& suffix, const int& number_width, int& number, const int& step) {
    std::ostringstream result;

    // Format new name using prefix, number, and suffix
    result << prefix << std::setw(number_width) << std::setfill('0') << number << suffix;

    // Update number for the next generation based on the step
    number += step;

    return result.str();
}

// Inserts the specified string at the given position in the original string.
// If position is -1, appends the string to the end; if position is invalid, prepends the string to the original.
void insertStringAtPosition(std::string& originalStr, const std::string& addString, int position) {
    if (position >= 0 && position < originalStr.length()) {
        // Insert at the specified position
        originalStr.insert(position, addString);
    }
    else if (position == -1) {
        // Append to the end
        originalStr += addString;
    }
    else {
        // Prepend to the beginning (if position is invalid)
        originalStr = addString + originalStr;
    }
}

// Processes the string add pattern configuration for each file.
void TaskHandler::processStringAddPattern() {
    const auto& pattern = config.getStringAddPattern();

    std::smatch match;
    if (std::regex_search(pattern.format, match, std::regex("\\\\(\\d+)\\\\"))) {
        int num = pattern.formatConfig.start;
        int number_width = std::stoi(match[1].str());
        int step = pattern.formatConfig.step; if (step < 1) step = 1;
        std::string prefix = match.prefix();
        std::string suffix = match.suffix();
        std::string addString;
        
        if (pattern.match.empty()) {
            // Process files and add new string based on numeric pattern
            processFiles([&](File& file) {
                std::string temp = file.get_new_name();
                addString = generateNewName(prefix, suffix, number_width, num, step);
                insertStringAtPosition(temp, addString, pattern.position);

                if (temp != file.get_new_name()) {
                    file.set_new_name(temp);
                }
                });
        }
        else {
            std::regex matchObj(pattern.match);

            // Process files and add new string based on numeric pattern if the file matches the specified pattern
            processFiles([&](File& file) {
                std::string temp = file.get_new_name();
                if (std::regex_match(temp, matchObj)) {
                    addString = generateNewName(prefix, suffix, number_width, num, step);
                    insertStringAtPosition(temp, addString, pattern.position);
                    if (temp != file.get_new_name()) {
                        file.set_new_name(temp);
                    }
                }
                });
        }
    }
    else {
        if (pattern.match.empty()) {
            // Process files and add the specified string
            processFiles([&](File& file) {
                std::string temp = file.get_new_name();
                insertStringAtPosition(temp, pattern.format, pattern.position);
                if (temp != file.get_new_name()) {
                    file.set_new_name(temp);
                }
                });
        }
        else {
            std::regex matchObj(pattern.match);

            // Process files and add the specified string if the file matches the specified pattern
            processFiles([&](File& file) {
                std::string temp = file.get_new_name();
                if (std::regex_match(temp, matchObj)) {
                    insertStringAtPosition(temp, pattern.format, pattern.position);
                    if (temp != file.get_new_name()) {
                        file.set_new_name(temp);
                    }
                }
                });
        }
    }
}

// Displays changes made to file names and files to be deleted.
void TaskHandler::showChanges() {

    std::cout << "\n[Name changed files]" << std::endl;
    int count = 1;

    // Process files and display changes
    processFiles([&](File& file) {
        if (file.is_name_changed()) {
            nameChangedFiles.emplace_back(file);
            std::cout << std::format("{}.\"{}\"  --->  \"{}\"", count, file.get_full_name(), file.get_new_full_name()) << std::endl;
            count++;
        }
        });

    count = 1;

    // Display files to be deleted
    if (!nameChangedFiles.size()) {
        std::cout << "None\n" << std::endl;
    }

    std::cout << "\n[Files to delete]" << std::endl;
    for (File& file : filesToDelete) {
        std::cout << std::format("{}. {}", count, file.get_full_name()) << std::endl;
        count++;
    }

    if (!filesToDelete.size()) {
        std::cout << "None\n" << std::endl;
    }
}

// Applies changes to file names and deletes specified files.
void TaskHandler::applyChanges() {
    
    // Ask for confirmation if enabled in the configuration
    if (global.isConfirmEnabled()) {
        showChanges();
        if (!filesToDelete.size() && !nameChangedFiles.size()) {
            confirmWithMsg("No changes to apply, press any key to continue.");
            return;
        }
        else
        {
            confirmWithMsg("Press any key to apply changes.");
        }
    }
    else {
        processFiles(
            [&](File& file) {
                if (file.is_name_changed()) {
                    nameChangedFiles.emplace_back(file);
                }
            });
    }

    

    // Apply new names to files with name changes
    for (File& file : nameChangedFiles) {
        file.applyNewName();
    }

    // Delete specified files
    for (File& file : filesToDelete) {
        deleteFile(file.get_path());
    }

    if (!global.isExitWhenDoneEnabled()) {
        confirmWithMsg("Changes applied, press any key ...");
    }
}
