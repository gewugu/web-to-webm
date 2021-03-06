/* This program replaces old binary with new,
 * downloaded from update serwer.
 * This should be rewritten as standalone
 * QT app which does the whole update
 * procedure.
*/

#include <stdio.h>

#ifdef _WIN32
    #define SYSTEM 0
    #include <windows.h>
#else
    #define SYSTEM 1
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
#endif

void log(const char *l);
void windows_procedure(char *argv);
void unix_procedure(char *argv);
void windows_rename();
void unix_rename();
void change_directory();

int main (int argc, char *argv[]) {
    if (SYSTEM == 0)
        windows_procedure(argv[1]);
    else
        unix_procedure(argv[0]);

    return 0;
}

void log(const char *l) {
    FILE *f = fopen("updatelog.txt", "a");
    if (f == NULL) {
        printf("Can't open updatelog.txt\n");
        exit(1);
    }
    fprintf(f, "%s\n", l);
    fclose(f);

    printf("%s\n", l);
}

#ifdef _WIN32
void windows_procedure(char *argv) {
    log("Detected WINDOWS system, waiting for main process to die.");
    printf("PID: %s\n", argv);
    DWORD dwExitCode = 259;
    while(1) {
        HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, atoi(argv));
        if(GetExitCodeProcess(h, &dwExitCode))
            CloseHandle(h);
        else
            break;

        Sleep(1000);
    }
    printf("Process is dead now. Starting update.");
    windows_rename();
}

void windows_rename() {
    remove("web-to-webm.old");

    int s;
    char o[] = "web-to-webm.exe";
    char n[] = "web-to-webm.old";
    s = rename(o, n);
    if (s == 0)
        log("Created backup of an old binary.");
    else
        log("Error on creating backup.");

    char o_[] = "update";
    char n_[] = "web-to-webm.exe";
    s = rename(o_ , n_);
    if (s == 0)
        log("New binary installed");
    else {
        log("Error on installing new binary. Abort.");
        exit(1);
    }

    printf("Starting web-to-webm...");

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    CreateProcess("web-to-webm.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    log("Update complete!");
}
#endif


void unix_procedure(char *argv) {
#ifndef _WIN32
    // This is necessary for app bundles on OS X
    char path[1024];
    strncpy(path, argv, strlen(argv)-7);
    path[strlen(argv)-7] = NULL;
    chdir(path);

    remove("web-to-webm.old");

    int s;
    char o[] = "web-to-webm";
    char n[] = "web-to-webm.old";
    s = rename(o, n);
    if (s == 0)
        log("Created backup of an old binary.");
    else
        log("Error on creating backup.");

    char o_[] = "update";
    char n_[] = "web-to-webm";
    s = rename(o_ , n_);
    if (s == 0)
        log("New binary installed");
    else {
        log("Error on installing new binary. Abort.");
        exit(1);
    }

    system("chmod a+x web-to-webm");

    printf("Starting web-to-webm...");
    system("./web-to-webm");
#endif
}

