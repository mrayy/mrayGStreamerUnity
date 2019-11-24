echo off

mkdir output
move libs\armeabi-v7a\libmrayGStreamerUnity.so output\libmrayGStreamerUnity.so

echo 
echo Cleaning up / removing build folders...
RD /S /Q libs
RD /S /Q obj

echo 
echo Done!
