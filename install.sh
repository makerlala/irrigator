#!/bin/bash

mkdir models
echo "Please download the models highlited in the README, in models subfolder."

sudo apt update
sudo apt install -y python3 python3-pip

python3 -m venv env3
source env3/bin/activate
pip3 install https://dl.google.com/coral/python/tflite_runtime-2.1.0.post1-cp36-cp36m-linux_aarch64.whl
deactivate

