## Setup C for Visual Studio Code
 
#### 1. Download and install Visual Studio Code from https://code.visualstudio.com/
 
#### 2. Download and install MinGW from http://www.mingw.org/ ([Video](https://youtu.be/guM4XS43m4I))  
  You will need the following packages:
  - mingw32-base-bin (A Basic MinGW Installation)
  - mingw32-gcc-g++-bin (The GNU C++ Compiler)
 
  Navigate to environment variables and add the path of the bin folder inside the installation folder of MinGW (by default it's C:\MinGW\bin) to the path variable.
  You will need to restart Visual Studio Code before you can compile inside the internal terminal.
 
#### 3. Install the following extensions for Visual Studio Code ([Video](https://youtu.be/77v-Poud_io?t=51))
- [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
- [Code Runner](https://marketplace.visualstudio.com/items?itemName=formulahendry.code-runner)
 
#### 4. Adjust preferences for Code Runner ([Video](https://youtu.be/77v-Poud_io?t=349))
- File -> Preferences -> Settings
- Search for "code runner: run in terminal" and make sure it's checked.
