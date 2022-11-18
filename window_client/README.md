# Windows Client

## START_UP Case
need to copy pc_on_checker.exe file in C:\Users\[User Name]\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup, as a startup program.

## SHUT_DOWN Case
[Solution](https://stackoverflow.com/questions/101647/how-to-schedule-a-task-to-run-when-shutting-down-windows)

1. open __gpedit.msc__ (search in WinKey + R or windows search box)
2. Computer Configuration => Windows Settings => Scripts (Startup / Shutdown) => Shutdown => Properties => Add
3. paste location of pc_off_noti.exe in Script Name
4. apply Scripts

## Build Command
### Windows
```gcc -o pc_on_noti .\pc_on_noti.c -lws2_32 ```

```gcc -o pc_off_noti .\pc_off_noti.c -lws2_32 ```
