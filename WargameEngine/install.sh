sudo apt-get install freeglut3-dev
sudo apt-get install libfreetype6-dev
sudo apt-get install libglew-dev
cd LUA
make linux
cd ..
cd MathLib\MathLib
make
cd ..\..
cd WargameEngine
make
cd ..