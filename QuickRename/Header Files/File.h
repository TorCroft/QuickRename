#pragma once

#include <filesystem>

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

    bool is_name_changed() const;
    bool applyNewName();
};