# irrigator

The Irrigator - an AI-powered irrigation robot.

## Presentation

The Irrigator - https://youtu.be/p2FFj9mP8a0

The Making Of - https://youtu.be/KjNYCxCwHMo

## Hardware

For more details about the hardware, please read my project on Hackster.io (https://www.hackster.io/dumiloghin/the-irrigator-80f1d5).


## Setup Dev Environment

### On a x86_64 Dev Host

#### Bazel

I downloaded bazel binary (bazel-0.19.2-linux-x86_64) from (https://github.com/bazelbuild/bazel/releases/tag/0.19.2)[https://github.com/bazelbuild/bazel/releases/tag/0.19.2] into ``isaac_sdk`` folder.

#### Isaac SDK

Download Isaac SDK and unzip it into a folder called ``isaac_sdk``.

Copy the content of ``src/jetson/isaac`` into a folder called ``irrigator`` under ``packages``:

```
cd isaac_sdk
mkdir -p packages/irrigator
cp ~/git/irrigator/src/jetson/isaac/* packages/irrigator/
cd packages/irrigator/
bazel build irrigator
```

Bazel run will not work on the development host.

Push the package to Jetson Nano:

```
./engine/build/deploy.sh --remote_user <user> -p //packages/irrigator:irrigator -h <jetson IP> -d jetpack43
```

### Jetson Nano

Setup an SD card and power on the Nano, as described here: https://developer.nvidia.com/embedded/learn/get-started-jetson-nano-devkit.

#### WiFi

Jetson Nano does not come with a WiFi adapter (this is a big issue, in my opinion). One option is to install an Intel WiFi + Bluetooth adapter, as described by JetsonHacks (https://www.jetsonhacks.com/2019/04/08/jetson-nano-intel-wifi-and-bluetooth/). However, I just purchased a TP-link AC1300 Archer T4U WiFi USB adapter. My adapter uses a Realtek RTL8812BU and not RTL8812AU. Here is how I installed the correct driver:

```
sudo apt install dkms
git clone https://github.com/cilynx/rtl88x2bu.git
cd rtl88x2bu
VER=$(sed -n 's/\PACKAGE_VERSION="\(.*\)"/\1/p' dkms.conf)
sudo rsync -rvhP ./ /usr/src/rtl88x2bu-${VER}
sudo dkms add -m rtl88x2bu -v ${VER}
sudo dkms build -m rtl88x2bu -v ${VER}
sudo dkms install -m rtl88x2bu -v ${VER}
sudo modprobe 88x2bu
```

To make sure rtl8812au is not loaded, add this line to ``/etc/modprobe.d/blacklist.conf``:

```
blacklist 8812au
```

To make sure rtl88x2bu is loaded, add a file called ``rtl88x2bu.conf`` in ``/etc/modprobe.d/blacklist.conf``, with the following content:

```
/sbin/modprobe 88x2bu
```

### TFLite (TensorFlow Lite)

Make sure you have the following models in the ``models`` subfolder:
* [mobilenet_v1_1.0_224_quant_and_labels](https://storage.googleapis.com/download.tensorflow.org/models/mobilenet_v1_2018_02_22/mobilenet_v1_1.0_224.tgz)
* [coco_ssd_mobilenet_v1_1.0_quant_2018_06_29](https://storage.googleapis.com/download.tensorflow.org/models/tflite/coco_ssd_mobilenet_v1_1.0_quant_2018_06_29.zip)
* [inception_resnet_v2_2018_04_27](https://storage.googleapis.com/download.tensorflow.org/models/tflite/model_zoo/upload_20180427/inception_resnet_v2_2018_04_27.tgz)

Make sure you have Python 3.6 and pip3. My ``install.sh`` script will help you install them.

It also installs tflite for Python 3.6 on ARM 64-bit. For more details, see https://www.tensorflow.org/lite/guide/python.

In addition to tflite, you need to install ``pillow``.

### Camera

Useful documentation: https://developer.download.nvidia.com/embedded/L4T/r32_Release_v1.0/Docs/Accelerated_GStreamer_User_Guide.pdf?z5qCkGUGPtbvZ3lvPXcknORRqeSXe0eujwaRA5dYvv53Mu0JJqvLkW4P-p9ZPQqIFF64zuqLYvH9X4x0BYnwPOKIW_G-jNhTHA8mndu6oN7boIXna0siQdXJ6WfAVMBSERhvKMO_1RT8x_Znea0gxseW01ix6l9WK3Df6r4exbpFhJL8AuU

In the end, the following command captures good-enough photos,

```
timeout 4 gst-launch-1.0 nvarguscamerasrc ! 'video/x-raw(memory:NVMM),width=3820, height=2464, framerate=21/1, format=NV12' ! nvvidconv flip-method=2 ! 'video/x-raw,width=960, height=616, format=I420' ! nvjpegenc ! filesink location=$FILE -e
```

where $FILE is the output file name. Note the use of ``timeout`` command, otherwise, the pipeline does not end.

## Licence

Some of the code is licensed under:

* Apache License, Version 2.0 - (label_image.py and other scripts in src/jetson/driver)
* GNU General Public License, version 2 (spi code in src/jetson/driver and src/jetson/isaac)

