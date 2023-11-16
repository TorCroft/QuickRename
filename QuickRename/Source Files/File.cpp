#include "QuickRenameClasses.h"

void File::set_new_name(const std::string& new_n) {
    new_name = new_n;
}

std::string File::get_name() const {
    return name;
}

std::string File::get_extension() const {
    return extension;
}

std::string File::get_new_name() const
{
    return new_name;
}

std::string File::get_full_name() const {
    return name + extension;
}

std::filesystem::path File::get_path() const {
    return path;
}

std::string File::get_new_full_name() const
{
    return new_name + extension;
}

std::filesystem::path File::get_new_name_path() const {
    return path.parent_path() / (new_name + extension);
}

bool File::is_name_changed() {
    return !(name == new_name);
}

// Applies the new name to the file, renaming it in the file system.
// Returns true if the operation is successful, false otherwise.
bool File::apply_new_name() {
    try {
        std::filesystem::path old_path = path;
        std::filesystem::path new_path = File::get_new_name_path();

        // Rename the file in the file system
        std::filesystem::rename(old_path, new_path);

        // Output the rename operation details
        std::cout << "Rename: " << old_path << "  --->  " << new_path << std::endl;

        // Update file attributes with the new name and path
        name = new_name;
        path = new_path;

        return true;
    }
    catch (const std::filesystem::filesystem_error& e) {
        // Handle and log any errors that occur during the rename operation
        std::cerr << e.what() << std::endl;
        return false;
    }
}
