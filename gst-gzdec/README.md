# GStreamer Gzip text decompresser plugin

This git module contains a Gstremer Plugin that Decompresses gzip files with text.

## License

This code is provided under a MIT license [MIT], which basically means "do
with it as you wishs. [Licensing].

## Usage

read next scripts to build and run the test programs, run them from it's folder

This will build the plugin and run the test programs:
    
    ./build-test-run-readme.sh
If the previous outputs

    Directory does not contain a valid build tree:
     
Run in the same folder without --reconfigure:

    meson build

This will run the test programs. Expects the plugin to be already built:
    
    cd gst-plugin/src
    ./run-readme.sh

You can also check if it has been built correctly with:

    gst-inspect-1.0 build/gst-plugin/libgstgzdec.so


[MIT]: http://www.opensource.org/licenses/mit-license.php or COPYING.MIT
[LGPL]: http://www.opensource.org/licenses/lgpl-license.php or COPYING.LIB
[Licensing]: https://gstreamer.freedesktop.org/documentation/application-development/appendix/licensing.html
