set PATH=%PATH%;C:\Projects\destructive_drones\temp\butler
robocopy /e build temp\build\windows\Release
butler push temp\build\windows\Release cinghycreations/destructive-drones:windows
pause
