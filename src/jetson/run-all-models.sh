#!/bin/bash

# make sure you cd into src/jetson and env3 is in the root of this repo
python3 -m venv ../../env3
source ../../env3/bin/activate

MODELS_PATH="../../models"
TSTAMP=`date +%Y-%m-%d-%H-%M-%S`
FILE="one-shot-$TSTAMP.jpg"
OUT_FILE="obj-det-$TSTAMP.txt"

if [ $# -eq 1 ]; then
	FILE=$1
else
	# capture image
	timeout 4 gst-launch-1.0 nvarguscamerasrc ! 'video/x-raw(memory:NVMM),width=3820, height=2464, framerate=21/1, format=NV12' ! nvvidconv flip-method=2 ! 'video/x-raw,width=960, height=616, format=I420' ! nvjpegenc ! filesink location=$FILE -e
fi

sleep 1

#echo "*** mobilenet_v1_1.0_224_quant_and_labels" > $OUT_FILE
#python3 label_image.py -m $MODELS_PATH/mobilenet_v1_1.0_224_quant_and_labels/mobilenet_v1_1.0_224_quant.tflite -l $MODELS_PATH/mobilenet_v1_1.0_224_quant_and_labels/labels_mobilenet_quant_v1_224.txt -i $FILE | head -n 3 >> $OUT_FILE

echo "*** coco_ssd_mobilenet_v1_1.0_quant_2018_06_29" >> $OUT_FILE
python3 label_image_boxes.py -m $MODELS_PATH/coco_ssd_mobilenet_v1_1.0_quant_2018_06_29/detect.tflite -l $MODELS_PATH/coco_ssd_mobilenet_v1_1.0_quant_2018_06_29/labelmap.txt -i $FILE | head -n 3 >> $OUT_FILE

echo "*** inception_resnet_v2_2018_04_27" >> $OUT_FILE
python3 label_image.py -m $MODELS_PATH/inception_resnet_v2_2018_04_27/inception_resnet_v2.tflite -l $MODELS_PATH/inception_resnet_v2_2018_04_27/labels.txt -i $FILE | head -n 3 >> $OUT_FILE

deactivate

# if plant or pot is detected, return 1
RES=`cat $OUT_FILE | grep -E "pot|plant"`
if [ -z "$RES" ]; then
	exit 0
else
	exit 1
fi
