/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2023 Oriol <<user@hostname.org>>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-gzdec
 *
 * FIXME:Describe gzdec here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! gzdec ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <string.h>
#include <assert.h>

#include <gst/gst.h>

#include "gstgzdec.h"

GST_DEBUG_CATEGORY_STATIC(gst_gzdec_debug);
#define GST_CAT_DEFAULT gst_gzdec_debug

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_SILENT
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE("sink",
                                                                   GST_PAD_SINK,
                                                                   GST_PAD_ALWAYS,
                                                                   GST_STATIC_CAPS("ANY"));

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE("src",
                                                                  GST_PAD_SRC,
                                                                  GST_PAD_ALWAYS,
                                                                  GST_STATIC_CAPS("ANY"));

#define gst_gzdec_parent_class parent_class
G_DEFINE_TYPE(GstGzdec, gst_gzdec, GST_TYPE_ELEMENT);

// GST_ELEMENT_REGISTER_DEFINE (gzdec, "gzdec", GST_RANK_NONE,
//     GST_TYPE_GZDEC);

/* Standard macros for defining types for this element.  */
#define GST_TYPE_GZDEC (gst_gzdec_get_type())
#define GST_GZDEC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_GZDEC, GstGzdec))
#define GST_GZDEC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_GZDEC, GstGzdecClass))
#define GST_IS_GZDEC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_GZDEC))
#define GST_IS_GZDEC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_GZDEC))

/* Standard function returning type information. */
GType gst_gzdec_get_type(void);

// GST_ELEMENT_REGISTER_DECLARE(gzdec);

static void gst_gzdec_set_property(GObject *object,
                                   guint prop_id, const GValue *value, GParamSpec *pspec);
static void gst_gzdec_get_property(GObject *object,
                                   guint prop_id, GValue *value, GParamSpec *pspec);

static gboolean gst_gzdec_sink_event(GstPad *pad,
                                     GstObject *parent, GstEvent *event);
static GstFlowReturn gst_gzdec_chain(GstPad *pad,
                                     GstObject *parent, GstBuffer *buf);

/* GObject vmethod implementations */

/* initialize the gzdec's class */
static void
gst_gzdec_class_init(GstGzdecClass *klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *)klass;
  gstelement_class = (GstElementClass *)klass;

  gobject_class->set_property = gst_gzdec_set_property;
  gobject_class->get_property = gst_gzdec_get_property;

  g_object_class_install_property(gobject_class, PROP_SILENT,
                                  g_param_spec_boolean("silent", "Silent", "Produce verbose output ?",
                                                       FALSE, G_PARAM_READWRITE));

  gst_element_class_set_details_simple(gstelement_class,
                                       "Gzdec",
                                       "Gzip file Inflater",
                                       "Gzip file Inflater plugin for Gstreamer", "Oriol <<orioldereina@outlook.com>>");

  gst_element_class_add_pad_template(gstelement_class,
                                     gst_static_pad_template_get(&src_factory));

  gst_element_class_add_pad_template(gstelement_class,
                                     gst_static_pad_template_get(&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad callback functions
 * initialize instance structure
 */
static void
gst_gzdec_init(GstGzdec *filter)
{
  filter->sinkpad = gst_pad_new_from_static_template(&sink_factory, "sink");
  gst_pad_set_event_function(filter->sinkpad,
                             GST_DEBUG_FUNCPTR(gst_gzdec_sink_event));

  gst_pad_set_chain_function(filter->sinkpad,
                             GST_DEBUG_FUNCPTR(gst_gzdec_chain));

  GST_PAD_SET_PROXY_CAPS(filter->sinkpad);
  gst_element_add_pad(GST_ELEMENT(filter), filter->sinkpad);

  filter->srcpad = gst_pad_new_from_static_template(&src_factory, "src");
  GST_PAD_SET_PROXY_CAPS(filter->srcpad);
  gst_element_add_pad(GST_ELEMENT(filter), filter->srcpad);

  filter->silent = TRUE;
  filter->first_read = TRUE;
}

static void
gst_gzdec_set_property(GObject *object, guint prop_id,
                       const GValue *value, GParamSpec *pspec)
{
  GstGzdec *filter = GST_GZDEC(object);

  switch (prop_id)
  {
  case PROP_SILENT:
    filter->silent = g_value_get_boolean(value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void
gst_gzdec_get_property(GObject *object, guint prop_id,
                       GValue *value, GParamSpec *pspec)
{
  GstGzdec *filter = GST_GZDEC(object);

  switch (prop_id)
  {
  case PROP_SILENT:
    g_value_set_boolean(value, filter->silent);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean
gst_gzdec_sink_event(GstPad *pad, GstObject *parent,
                     GstEvent *event)
{
  GstGzdec *filter;
  gboolean ret;

  filter = GST_GZDEC(parent);

  GST_LOG_OBJECT(filter, "Received %s event: %" GST_PTR_FORMAT,
                 GST_EVENT_TYPE_NAME(event), event);

  switch (GST_EVENT_TYPE(event))
  {
  case GST_EVENT_STREAM_START:
  {
    filter->infstream.zalloc = Z_NULL;
    filter->infstream.zfree = Z_NULL;
    filter->infstream.opaque = Z_NULL;
    filter->infstream.avail_in = 0; // size of input
    CALL_ZLIB(inflateInit2(&filter->infstream, windowBits | ENABLE_ZLIB_GZIP));
    g_print("Initializing in zlib decoder stream\n");
  }
  break;
  case GST_EVENT_EOS:
  {
    g_print("End Of Stream Event \n");
    inflateEnd(&filter->infstream);
    ret = gst_pad_event_default(pad, parent, event);
  }
  break;
  case GST_EVENT_CAPS:
  {
    GstCaps *caps;

    gst_event_parse_caps(event, &caps);
    /* do something with the caps */

    /* and forward */
    ret = gst_pad_event_default(pad, parent, event);
    break;
  }
  default:
    ret = gst_pad_event_default(pad, parent, event);
    break;
  }
  return ret;
}

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn
gst_gzdec_chain(GstPad *pad, GstObject *parent, GstBuffer *buf)
{
  GstGzdec *filter;
  filter = GST_GZDEC(parent);
  GstMemory *memory;

  if (filter->silent == FALSE)
  {
    g_print("I'm plugged, therefore I'm in.\n");
    g_print("Have data of size %" G_GSIZE_FORMAT " bytes!\n",
            gst_buffer_get_size(buf));
  }
  GstMapInfo info_in;
  gst_buffer_map(buf, &info_in, GST_MAP_READ);
  guint8 *in = info_in.data;
  filter->infstream.next_in = (Bytef *)in;
  filter->infstream.avail_in = info_in.size;

  // Usually you don't get more than 95% compression
  guint memory_based_compression_ratio = (guint)info_in.size / 0.0499999;
  // g_print("\nAmount of requested memory: %d \n", memory_based_compression_ratio);
  memory = gst_allocator_alloc(NULL, memory_based_compression_ratio, NULL);
  if (memory == NULL)
    g_print("\nmemory is NULL");

  GstMapInfo info_out;
  gst_memory_map(memory, &info_out, GST_MAP_WRITE);
  guint8 *out = info_out.data;
  unsigned int new_size = 0;
  unsigned int have = 0;
  do
  {
    filter->infstream.avail_out = memory_based_compression_ratio;
    filter->infstream.next_out = (Bytef *)out;
    CALL_ZLIB(inflate(&filter->infstream, Z_NO_FLUSH));

    have = memory_based_compression_ratio - filter->infstream.avail_out; // compute output size
    out += have;                                                         // advance the pointer
    new_size += have;                                                    // accumulate the final decompressed size
    memory_based_compression_ratio -= have;                              // reduce available memory

  } while (filter->infstream.avail_out == 0);
  // resize the memory to fit the actual decompression
  gst_memory_resize(memory,
                    0,
                    new_size);
  // g_print("\nMemory after inflating block: %d - not needed: %d \n", new_size, memory_based_compression_ratio);

  // unmap things so you can replace memory after
  gst_buffer_unmap(buf, &info_in);
  gst_memory_unmap(memory, &info_out);

  // replace the buffer with the new memory holding the inflated result before returning it
  gst_buffer_replace_all_memory(buf, memory);
  return gst_pad_push(filter->srcpad, buf);
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
gzdec_init(GstPlugin *plugin)
{
  /* debug category for filtering log messages
   *
   * exchange the string 'Template gzdec' with your description
   */
  GST_DEBUG_CATEGORY_INIT(gst_gzdec_debug, "gzdec",
                          0, "Template gzdec");

  return gst_element_register(plugin, "gzdec",
                              GST_RANK_NONE,
                              GST_TYPE_GZDEC);
  // return G_PASTE(gst_element, _register) (plugin);
  // return GST_ELEMENT_REGISTER (gzdec, plugin);
}

/* PACKAGE: this is usually set by meson depending on some _INIT macro
 * in meson.build and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use meson to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "gzdec"
#endif

/* gstreamer looks for this structure to register gzdecs
 *
 * exchange the string 'Template gzdec' with your gzdec description
 */
GST_PLUGIN_DEFINE(GST_VERSION_MAJOR,
                  GST_VERSION_MINOR,
                  gzdec,
                  "gzdec",
                  gzdec_init,
                  PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
