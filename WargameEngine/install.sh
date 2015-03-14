function func() 
{ echo `sudo apt-get install freeglut3-dev`
echo `sudo apt-get install libfreetype6-dev`
echo `sudo apt-get install libglew-dev`
echo `sudo apt-get install libreadline-dev`
echo `sudo apt-get install libopenal1`
echo `sudo apt-get install libopenal-dev`
cd LUA/LUA
echo `dir`
echo `make linux install`
cd ../..
cd MathLib/MathLib
echo `make`
cd ../..
printf "\n"
cd WargameEngine/WargameEngine
echo `make`
cd ..
}
func
exit
