#/bin/bash

export GST_PLUGIN_PATH=$(pwd)/../../build/gst-plugin

#from gst-gzdec/gst-plugin/src 
gcc main.c -o main `pkg-config --cflags --libs gstreamer-1.0`

./main test-file.txt.gz test-file-mainout.txt

#test building pipeline with gst-lunch-1.0
gst-launch-1.0  filesrc location=./test-file.txt.gz  ! gzdec ! filesink location=./test-file-gstout.txt