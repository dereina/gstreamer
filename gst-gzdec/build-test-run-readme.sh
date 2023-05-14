meson build --reconfigure
ninja -C build

#update this to point to the plugin *.so output from "ninja -C build" i.e gst-gzdec/build/gst-plugin or install it with "ninja install" so gstreamer can find it
export GST_PLUGIN_PATH=$(pwd)/build/gst-plugin

#compile and run test pipeline program
gcc gst-plugin/src/main.c -o gst-plugin/src/main `pkg-config --cflags --libs gstreamer-1.0`
gst-plugin/src/main gst-plugin/src/test-file.txt.gz gst-plugin/src/test-file-mainout.txt

#test building pipeline with gst-lunch-1.0
gst-launch-1.0  filesrc location=gst-plugin/src/test-file.txt.gz  ! gzdec ! filesink location=gst-plugin/src/test-file-gstout.txt