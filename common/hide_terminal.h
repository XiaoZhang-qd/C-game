#ifndef CGAME_HIDE_TERMINAL_H
#define CGAME_HIDE_TERMINAL_H

#if defined(_WIN32) || defined(_WIN64)
    /*
     * Windows: terminal hidden by CMake WIN32_EXECUTABLE property.
     * This sets the /SUBSYSTEM:WINDOWS linker flag, which tells the
     * linker to create a Windows GUI subsystem executable instead
     * of a console subsystem. No code needed here.
     */
    #define CGAME_HIDE_TERMINAL_WINDOWS 1

#elif defined(__APPLE__)
    /*
     * macOS: hide from Dock and detach from terminal.
     *
     * Two complementary mechanisms:
     * 1. CMake embeds an LSUIElement plist via -Wl,-sectcreate,__TEXT,__info_plist
     *    鈥?hides the app from the Dock and Cmd+Tab switcher.
     * 2. This constructor function re-launches the binary under 'nohup' so the
     *    original terminal process exits and the GUI runs in the background.
     */
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <libgen.h>
    #include <mach-o/dyld.h>

    __attribute__((constructor))
    static void cgame_hide_terminal_macos(void)
    {
        if (getenv("CGAME_ACTIVE")) return;

        char path[1024];
        uint32_t size = sizeof(path);
        if (_NSGetExecutablePath(path, &size) != 0) return;

        char *dir = dirname(path);
        chdir(dir);
        setenv("CGAME_ACTIVE", "1", 1);

        char cmd[2048];
        snprintf(cmd, sizeof(cmd),
                 "nohup \"%s\" > /dev/null 2>&1 & disown",
                 path);
        system(cmd);
        _exit(0);
    }

#elif defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
    /*
     * Linux / BSD: daemon-style terminal hiding via double-fork.
     *
     * The process forks twice, detaches from the controlling terminal
     * with setsid(), redirects stdin/stdout/stderr to /dev/null, and
     * then continues running the GUI in the background.
     */
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <sys/types.h>

    __attribute__((constructor))
    static void cgame_hide_terminal_unix(void)
    {
        if (getenv("CGAME_ACTIVE")) return;

        pid_t pid = fork();
        if (pid < 0) return;
        if (pid > 0) _exit(0);

        if (setsid() < 0) return;

        pid = fork();
        if (pid < 0) return;
        if (pid > 0) _exit(0);

        umask(0);

        int fd = open("/dev/null", O_RDWR);
        if (fd >= 0) {
            dup2(fd, STDIN_FILENO);
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);
        }

        setenv("CGAME_ACTIVE", "1", 1);
    }

#endif

#endif /* CGAME_HIDE_TERMINAL_H */