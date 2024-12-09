# CSCI411-final-project

## Description

Author - Elias Legato

Harris corner detection

## Dependantcies

- OpenCV

install OpenCV on linux or wsl with: 
sudo apt-get install libopencv-dev

## use Instructions

- use make in cmd line to compile using the included makefile
- ./hcd FilePath threshold

the program will take the image you feed it and add small red circles at all the corners detected using the input threshold, around 153500000.0 for the threshold works for most images. for some images change threshold by removing or adding 0s, higher number means less corners detected. 
