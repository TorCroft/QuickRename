#include "QuickRenameClasses.h"


int main() {
    // Set the locale to Chinese (Simplified) UTF-8 for proper character encoding
    setlocale(LC_ALL, "zh_CN.UTF-8");

    Config config;
    TaskHandler task_handler(config);
    task_handler.executeTasks();
    return 0;
}
