# irrigator

The Irrigator - an autonomous robot that takes care of your plants

## Hardware

### Links

Jetson Nano header: https://www.element14.com/community/community/designcenter/single-board-computers/blog/2019/05/21/nvidia-jetson-nano-developer-kit-pinout-and-diagrams

## Setup Dev Environment

### Jetson Nano Setup

#### WiFi

Jetson Nano does not come with an WiFi adapter (this is a big issue, in my opinion). One option is to install an Intel WiFi + Bluetooth adapter, as described by JetsonHacks (https://www.jetsonhacks.com/2019/04/08/jetson-nano-intel-wifi-and-bluetooth/). However, I just purchased a TP-link AC1300 Archer T4U WiFi USB adapter. My adapter uses an Realtek RTL8812BU and not RTL8812AU. Here is how I installed the correct driver:

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

### TFLite

Make sure you have the following models in the ``models`` subfolder:
* mobilenet_v1_1.0_224_quant_and_labels
* coco_ssd_mobilenet_v1_1.0_quant_2018_06_29
* inception_resnet_v2_2018_04_27

Make sure you have Python 3.6 and pip3. My ``install.sh`` script will help you install them.

It also installs tflite for Python 3.6 on ARM 64-bit. For more details, see https://www.tensorflow.org/lite/guide/python.

In addition to tflite, you need to install ``pillow``.

### Camera

Useful documentation: https://developer.download.nvidia.com/embedded/L4T/r32_Release_v1.0/Docs/Accelerated_GStreamer_User_Guide.pdf?z5qCkGUGPtbvZ3lvPXcknORRqeSXe0eujwaRA5dYvv53Mu0JJqvLkW4P-p9ZPQqIFF64zuqLYvH9X4x0BYnwPOKIW_G-jNhTHA8mndu6oN7boIXna0siQdXJ6WfAVMBSERhvKMO_1RT8x_Znea0gxseW01ix6l9WK3Df6r4exbpFhJL8AuU

### Bazel

I downloaded bazel binary (bazel-0.19.2-linux-x86_64) from (https://github.com/bazelbuild/bazel/releases/tag/0.19.2)[https://github.com/bazelbuild/bazel/releases/tag/0.19.2] into ``isaac_sdk`` folder.
