plugin_c_args = ['-DHAVE_CONFIG_H']

cdata = configuration_data()
cdata.set_quoted('PACKAGE_VERSION', gst_version)
cdata.set_quoted('PACKAGE', 'gst-gzdec')
cdata.set_quoted('GST_LICENSE', 'LGPL')
cdata.set_quoted('GST_API_VERSION', api_version)
cdata.set_quoted('GST_PACKAGE_NAME', 'GStreamer gzip file inflate Plug-in')
cdata.set_quoted('GST_PACKAGE_ORIGIN', 'https://www.github.com/dereina')
configure_file(output : 'config.h', configuration : cdata)


# The gzdec Plugin
 gstgzdec_sources = [
  'src/gstgzdec.c',
  ]

gstgzdecexample = library('gstgzdec',
  gstgzdec_sources,
  c_args: plugin_c_args,
  dependencies : [gst_dep, gstbase_dep, zdep],
  install : true,
  install_dir : plugins_install_dir,
)
