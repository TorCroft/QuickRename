# QuickRename
QuickRename is a C++ program designed to quickly rename files based on user-defined configurations. It provides various options for modifying file names, deleting files with unwanted extensions, deleting specified strings in file name, replacing patterns, and more.

## Table of Contents

- [Usage](#Usage)
- [Configuration](#Configuration)
- [Features](#Features)

## Usage

QuickRename is a versatile tool for renaming files in bulk according to user-defined configurations. Follow the steps below to efficiently use QuickRename for your file renaming tasks.

  **Configure the `config.json` File:**

   Edit the `config.json` file to specify how QuickRename should modify file names. Customize the lists for unwanted extensions, strings to delete, and string replacement patterns. Define a string add pattern if needed. Ensure that the confirmation option is set according to your preference.

   If the `config.json` file does not exist, it will be automatically created with default settings.

   Example `config.json`:

   ``` json
   {
    "confirm": true,
    "exit_when_done": false,
    "target_dir": "",
    "unwanted_extension": [ ".tmp", "bak" ],
    "string_delete": [ "_old" ],
    "string_replace_pattern": [
      {
        "re_match": "\\d{4}",
        "replace": "2077"
      }
    ],
    "string_add_pattern": {
      "re_match": "",
      "format": "S01E\\2\\ ",
      "format_config": {
        "start": 1,
        "step": 1
      },
      "position": 0
    }
  }
  ```
  **Note:<br>**
  This project utilizes the `nlohmann/json` library for handling JSON configurations. To successfully build and run the project, make sure to include this library in your project dependencies. You can find the library [here](https://github.com/nlohmann/json) and follow the installation instructions provided in the repository.

## Configuration
These configurations provide flexibility and control over how QuickRename modifies file names, allowing for a tailored file renaming process based on specific needs. Adjust these settings as required for efficient file organization.
| **Option**  | **Description**  |
|-------------------------|---------------------------------------|
| `confirm` | This boolean option determines whether QuickRename will prompt for confirmation before applying the changes. If set to true, QuickRename will display a summary of changes and ask for confirmation before proceeding. |
| `exit_when_done`| This boolean option determines whether QuickRename will prompt for confirmation after applying the changes. If set to true, QuickRename will directly exit when changes are applied. |
| `target_dir` | Specifies the target directory for QuickRename operations. This is the directory where QuickRename will execute file renaming tasks. If not specified, the default target directory is the current working directory.|
| `unwantedExtensionList` | This is a list of file extensions that QuickRename will remove files with these extensions. |
| `stringDeleteList`| QuickRename will delete the specified strings from the file names. Any occurrences of the strings in this list will be removed. |
| `stringReplaceList` | This is a list of string replacement patterns. Each pattern consists of two strings: the string to be replaced and the string to replace it with. `re_match` can be **regular expression**. Multiple patterns can be specified for complex replacements. |
| `stringAddPattern` | This section configures the addition of a custom string pattern to file names. It includes the following sub-options: <br>**match:** The string to match file name. It will match all files if it's empty. <br> **format:** The format of the string to be added. If it contains `\\number\\`, where the number represents the length of the sequential number, you can specify a sequential number using `formatConfig`.<br> **Example:** If format is set to `"S01E\\2\\"`, which means it will add a string like "S01E01", "S01E02", and so on. The `\\2\\` represents a two-digit sequential number, and it starts from 1, incrementing by 1 for each file. |
|`formatConfig` | Additional configuration for formatting the added string.<br>**start:** The starting value of the sequential number.<br> **step:** The step or increment value for the sequential number.<br>**position:** The position where the new string should be added (0 for the beginning, -1 for the end of file name, 1 for after the first character, etc.).|


## Features

### Customizable File Renaming

QuickRename provides a highly customizable approach to file renaming, allowing users to define specific rules for modifying file names. With options to remove unwanted extensions, delete specified strings, replace patterns, and add custom strings, QuickRename adapts to diverse file organization needs.

### Sequential Numbering

Add sequential numbers to your file names with ease. Utilize the `stringAddPattern` configuration to include a numbered sequence in the format of your choice. Control the starting value and step for a personalized numbering scheme.

### Regular Expression Support

For advanced users, QuickRename supports regular expressions in the `stringReplaceList`. Leverage the power of regular expressions to perform complex string replacements, providing unparalleled flexibility in file renaming.

### Target Directory Specification

Define a specific target directory for QuickRename operations using the `target_dir` configuration. This allows you to focus file renaming tasks on a particular directory, enhancing precision and control.

### Review Changes

QuickRename provides a convenient way to preview proposed changes before applying them. The summary of changes is displayed, allowing you to review and ensure they align with your intentions.

### Apply Changes

If you are satisfied with the proposed modifications, QuickRename allows you to confirm the application of changes. Upon confirmation, QuickRename proceeds to rename or delete the files based on the configured rules.

### Enjoy Organized Files

After the process is complete, your files will be renamed according to the specified rules, resulting in a more organized file structure. QuickRename enhances your file management experience by providing a seamless and efficient way to rename multiple files at once.
