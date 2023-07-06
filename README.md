# Window class, that handles keyboard/joystic/mouse input and provides basic logging functionality
## Dependecies

Windows:
1. [IndieGo UI](https://github.com/YgorVasilenko/IndieGoUI)
2. [cmake](https://cmake.org/download/)
3. [Visual Studio build tools](https://visualstudio.microsoft.com/downloads/?q=build+tools)

Linux:
- TODO.

## Building example application

0. Download/Install dependencies :D

1. Make shure, that cmake is avaiable in PATH variable:
- cmd:
```
set "PATH=C:\Program Files\CMake\bin;%PATH%"
```

- powershell:
```
$env:Path="C:\Program Files\CMake\bin;$env:Path"
```

2. save path to IndieGoUI to variable:

- cmd:
```
cd IndieGoUI
set "INDIEGO_UI_HOME=%cd:\=/%"
cd ..
```

- powershell:
```
cd IndieGoUI
$INDIEGO_UI_HOME=(get-location).path.replace('\','/')
cd ..
```

3. go to example_app folder and build using cmake:

- cmd:
```
cd IndieGoWindow\example_app
cmake -Bbuild -DINDIEGO_UI=%INDIEGO_UI_HOME%
cmake --build build
```

- powershell:
```
cd IndieGoWindow\example_app
cmake -Bbuild -DINDIEGO_UI=$INDIEGO_UI_HOME
cmake --build build
```

example output should be:

```
  Generating Code...
          with
          [
              _Other1=unsigned __int64,
              _Other2=int
          ]
  CameraDemo.vcxproj -> C:\Users\vasilenk\Private2\CameraDemo\example_app\build\Debug\WindowDemo.exe
```

## Running example.

```
C:\Users\vasilenk\Private2\CameraDemo\example_app\build\Debug\WindowDemo.exe
```