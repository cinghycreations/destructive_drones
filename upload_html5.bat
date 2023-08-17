set PATH=%PATH%;"C:\Projects\destructive_drones\temp\butler"
butler push --ignore "*.cmake" --ignore "*.ninja" --ignore "*.ninja_deps" --ignore "*.ninja_log" --ignore "*.txt" --ignore "ext" --ignore "CMakeFiles" temp\build\html5 cinghycreations/destructive-drones:html5
pause
