#include <jni.h>
#include <gst/gst.h>
#include <gio/gio.h>
#include <android/log.h>
#include <string.h>

/* XXX: Workaround for Android <21 making signal() an inline function
 * around bsd_signal(), and Android >= 21 not having any bsd_signal()
 * symbol but only signal().
 * See https://bugzilla.gnome.org/show_bug.cgi?id=766235
 */
static gpointer
load_real_signal (gpointer data)
{
  GModule *module;
  gpointer ret = NULL;

  module = g_module_open ("libc.so", G_MODULE_BIND_LOCAL);
  g_module_symbol (module, "signal", &ret);

  /* As fallback, let's try bsd_signal */
  if (ret == NULL) {
    g_warning ("Can't find signal(3) in libc.so!");
    g_module_symbol (module, "bsd_signal", &ret);
  }

  g_module_close (module);

  return ret;
}

__sighandler_t bsd_signal(int signum, __sighandler_t handler) __attribute__((weak));
__sighandler_t bsd_signal(int signum, __sighandler_t handler)
{
  static GOnce gonce = G_ONCE_INIT;
  __sighandler_t (*real_signal) (int signum, __sighandler_t handler);

  g_once (&gonce, load_real_signal, NULL);

  real_signal = gonce.retval;
  g_assert (real_signal != NULL);

  return real_signal(signum, handler);
}

static jobject _context = NULL;
static jobject _class_loader = NULL;
static JavaVM  *_java_vm = NULL;
static GstClockTime _priv_gst_info_start_time;

#define GST_G_IO_MODULE_DECLARE(name) \
extern void G_PASTE(g_io_module_, G_PASTE(name, _load_static)) (void)

#define GST_G_IO_MODULE_LOAD(name) \
G_PASTE(g_io_module_, G_PASTE(name, _load_static)) ()

/* Declaration of static plugins */
  GST_PLUGIN_STATIC_DECLARE(coreelements);  GST_PLUGIN_STATIC_DECLARE(coretracers);  GST_PLUGIN_STATIC_DECLARE(adder);  GST_PLUGIN_STATIC_DECLARE(app);  GST_PLUGIN_STATIC_DECLARE(audioconvert);  GST_PLUGIN_STATIC_DECLARE(audiomixer);  GST_PLUGIN_STATIC_DECLARE(audiorate);  GST_PLUGIN_STATIC_DECLARE(audioresample);  GST_PLUGIN_STATIC_DECLARE(audiotestsrc);  GST_PLUGIN_STATIC_DECLARE(gio);  GST_PLUGIN_STATIC_DECLARE(pango);  GST_PLUGIN_STATIC_DECLARE(rawparse);  GST_PLUGIN_STATIC_DECLARE(typefindfunctions);  GST_PLUGIN_STATIC_DECLARE(videoconvert);  GST_PLUGIN_STATIC_DECLARE(videorate);  GST_PLUGIN_STATIC_DECLARE(videoscale);  GST_PLUGIN_STATIC_DECLARE(videotestsrc);  GST_PLUGIN_STATIC_DECLARE(volume);  GST_PLUGIN_STATIC_DECLARE(autodetect);  GST_PLUGIN_STATIC_DECLARE(videofilter);  GST_PLUGIN_STATIC_DECLARE(playback);  GST_PLUGIN_STATIC_DECLARE(subparse);  GST_PLUGIN_STATIC_DECLARE(ogg);  GST_PLUGIN_STATIC_DECLARE(theora);  GST_PLUGIN_STATIC_DECLARE(vorbis);  GST_PLUGIN_STATIC_DECLARE(opus);  GST_PLUGIN_STATIC_DECLARE(ivorbisdec);  GST_PLUGIN_STATIC_DECLARE(alaw);  GST_PLUGIN_STATIC_DECLARE(apetag);  GST_PLUGIN_STATIC_DECLARE(audioparsers);  GST_PLUGIN_STATIC_DECLARE(auparse);  GST_PLUGIN_STATIC_DECLARE(avi);  GST_PLUGIN_STATIC_DECLARE(dv);  GST_PLUGIN_STATIC_DECLARE(flac);  GST_PLUGIN_STATIC_DECLARE(flv);  GST_PLUGIN_STATIC_DECLARE(flxdec);  GST_PLUGIN_STATIC_DECLARE(icydemux);  GST_PLUGIN_STATIC_DECLARE(id3demux);  GST_PLUGIN_STATIC_DECLARE(isomp4);  GST_PLUGIN_STATIC_DECLARE(jpeg);  GST_PLUGIN_STATIC_DECLARE(lame);  GST_PLUGIN_STATIC_DECLARE(matroska);  GST_PLUGIN_STATIC_DECLARE(mpg123);  GST_PLUGIN_STATIC_DECLARE(mulaw);  GST_PLUGIN_STATIC_DECLARE(multipart);  GST_PLUGIN_STATIC_DECLARE(png);  GST_PLUGIN_STATIC_DECLARE(speex);  GST_PLUGIN_STATIC_DECLARE(taglib);  GST_PLUGIN_STATIC_DECLARE(vpx);  GST_PLUGIN_STATIC_DECLARE(wavenc);  GST_PLUGIN_STATIC_DECLARE(wavpack);  GST_PLUGIN_STATIC_DECLARE(wavparse);  GST_PLUGIN_STATIC_DECLARE(y4menc);  GST_PLUGIN_STATIC_DECLARE(adpcmdec);  GST_PLUGIN_STATIC_DECLARE(adpcmenc);  GST_PLUGIN_STATIC_DECLARE(dashdemux);  GST_PLUGIN_STATIC_DECLARE(dvbsuboverlay);  GST_PLUGIN_STATIC_DECLARE(dvdspu);  GST_PLUGIN_STATIC_DECLARE(hls);  GST_PLUGIN_STATIC_DECLARE(id3tag);  GST_PLUGIN_STATIC_DECLARE(kate);  GST_PLUGIN_STATIC_DECLARE(midi);  GST_PLUGIN_STATIC_DECLARE(mxf);  GST_PLUGIN_STATIC_DECLARE(openh264);  GST_PLUGIN_STATIC_DECLARE(opusparse);  GST_PLUGIN_STATIC_DECLARE(pcapparse);  GST_PLUGIN_STATIC_DECLARE(pnm);  GST_PLUGIN_STATIC_DECLARE(rfbsrc);  GST_PLUGIN_STATIC_DECLARE(siren);  GST_PLUGIN_STATIC_DECLARE(smoothstreaming);  GST_PLUGIN_STATIC_DECLARE(subenc);  GST_PLUGIN_STATIC_DECLARE(videoparsersbad);  GST_PLUGIN_STATIC_DECLARE(y4mdec);  GST_PLUGIN_STATIC_DECLARE(jpegformat);  GST_PLUGIN_STATIC_DECLARE(gdp);  GST_PLUGIN_STATIC_DECLARE(rsvg);  GST_PLUGIN_STATIC_DECLARE(openjpeg);  GST_PLUGIN_STATIC_DECLARE(spandsp);  GST_PLUGIN_STATIC_DECLARE(sbc);  GST_PLUGIN_STATIC_DECLARE(androidmedia);  GST_PLUGIN_STATIC_DECLARE(tcp);  GST_PLUGIN_STATIC_DECLARE(rtsp);  GST_PLUGIN_STATIC_DECLARE(rtp);  GST_PLUGIN_STATIC_DECLARE(rtpmanager);  GST_PLUGIN_STATIC_DECLARE(soup);  GST_PLUGIN_STATIC_DECLARE(udp);  GST_PLUGIN_STATIC_DECLARE(dtls);  GST_PLUGIN_STATIC_DECLARE(sdpelem);  GST_PLUGIN_STATIC_DECLARE(srtp);  GST_PLUGIN_STATIC_DECLARE(webrtc);  GST_PLUGIN_STATIC_DECLARE(rtspclientsink);  GST_PLUGIN_STATIC_DECLARE(opengl);  GST_PLUGIN_STATIC_DECLARE(opensles);  GST_PLUGIN_STATIC_DECLARE(openglmixers);  GST_PLUGIN_STATIC_DECLARE(alpha);  GST_PLUGIN_STATIC_DECLARE(alphacolor);  GST_PLUGIN_STATIC_DECLARE(audiofx);  GST_PLUGIN_STATIC_DECLARE(cairo);  GST_PLUGIN_STATIC_DECLARE(cutter);  GST_PLUGIN_STATIC_DECLARE(debug);  GST_PLUGIN_STATIC_DECLARE(deinterlace);  GST_PLUGIN_STATIC_DECLARE(dtmf);  GST_PLUGIN_STATIC_DECLARE(effectv);  GST_PLUGIN_STATIC_DECLARE(equalizer);  GST_PLUGIN_STATIC_DECLARE(gdkpixbuf);  GST_PLUGIN_STATIC_DECLARE(imagefreeze);  GST_PLUGIN_STATIC_DECLARE(interleave);  GST_PLUGIN_STATIC_DECLARE(level);  GST_PLUGIN_STATIC_DECLARE(multifile);  GST_PLUGIN_STATIC_DECLARE(replaygain);  GST_PLUGIN_STATIC_DECLARE(shapewipe);  GST_PLUGIN_STATIC_DECLARE(smpte);  GST_PLUGIN_STATIC_DECLARE(spectrum);  GST_PLUGIN_STATIC_DECLARE(videobox);  GST_PLUGIN_STATIC_DECLARE(videocrop);  GST_PLUGIN_STATIC_DECLARE(videomixer);  GST_PLUGIN_STATIC_DECLARE(accurip);  GST_PLUGIN_STATIC_DECLARE(aiff);  GST_PLUGIN_STATIC_DECLARE(audiofxbad);  GST_PLUGIN_STATIC_DECLARE(autoconvert);  GST_PLUGIN_STATIC_DECLARE(bayer);  GST_PLUGIN_STATIC_DECLARE(coloreffects);  GST_PLUGIN_STATIC_DECLARE(debugutilsbad);  GST_PLUGIN_STATIC_DECLARE(fieldanalysis);  GST_PLUGIN_STATIC_DECLARE(freeverb);  GST_PLUGIN_STATIC_DECLARE(frei0r);  GST_PLUGIN_STATIC_DECLARE(gaudieffects);  GST_PLUGIN_STATIC_DECLARE(geometrictransform);  GST_PLUGIN_STATIC_DECLARE(inter);  GST_PLUGIN_STATIC_DECLARE(interlace);  GST_PLUGIN_STATIC_DECLARE(ivtc);  GST_PLUGIN_STATIC_DECLARE(legacyrawparse);  GST_PLUGIN_STATIC_DECLARE(proxy);  GST_PLUGIN_STATIC_DECLARE(removesilence);  GST_PLUGIN_STATIC_DECLARE(segmentclip);  GST_PLUGIN_STATIC_DECLARE(smooth);  GST_PLUGIN_STATIC_DECLARE(speed);  GST_PLUGIN_STATIC_DECLARE(soundtouch);  GST_PLUGIN_STATIC_DECLARE(videofiltersbad);  GST_PLUGIN_STATIC_DECLARE(compositor);  GST_PLUGIN_STATIC_DECLARE(webrtcdsp);  GST_PLUGIN_STATIC_DECLARE(ladspa);  GST_PLUGIN_STATIC_DECLARE(audiobuffersplit);  GST_PLUGIN_STATIC_DECLARE(camerabin);  GST_PLUGIN_STATIC_DECLARE(encoding);  GST_PLUGIN_STATIC_DECLARE(asfmux);  GST_PLUGIN_STATIC_DECLARE(dtsdec);  GST_PLUGIN_STATIC_DECLARE(faad);  GST_PLUGIN_STATIC_DECLARE(mpegpsdemux);  GST_PLUGIN_STATIC_DECLARE(mpegpsmux);  GST_PLUGIN_STATIC_DECLARE(mpegtsdemux);  GST_PLUGIN_STATIC_DECLARE(mpegtsmux);  GST_PLUGIN_STATIC_DECLARE(voaacenc);  GST_PLUGIN_STATIC_DECLARE(a52dec);  GST_PLUGIN_STATIC_DECLARE(amrnb);  GST_PLUGIN_STATIC_DECLARE(amrwbdec);  GST_PLUGIN_STATIC_DECLARE(asf);  GST_PLUGIN_STATIC_DECLARE(dvdsub);  GST_PLUGIN_STATIC_DECLARE(dvdlpcmdec);  GST_PLUGIN_STATIC_DECLARE(mpeg2dec);  GST_PLUGIN_STATIC_DECLARE(xingmux);  GST_PLUGIN_STATIC_DECLARE(realmedia);  GST_PLUGIN_STATIC_DECLARE(x264);  GST_PLUGIN_STATIC_DECLARE(libav);

/* Declaration of static gio modules */
  GST_G_IO_MODULE_DECLARE(gnutls);

/* Call this function to register static plugins */
void
gst_android_register_static_plugins (void)
{
  GST_PLUGIN_STATIC_REGISTER(coreelements);  GST_PLUGIN_STATIC_REGISTER(coretracers);  GST_PLUGIN_STATIC_REGISTER(adder);  GST_PLUGIN_STATIC_REGISTER(app);  GST_PLUGIN_STATIC_REGISTER(audioconvert);  GST_PLUGIN_STATIC_REGISTER(audiomixer);  GST_PLUGIN_STATIC_REGISTER(audiorate);  GST_PLUGIN_STATIC_REGISTER(audioresample);  GST_PLUGIN_STATIC_REGISTER(audiotestsrc);  GST_PLUGIN_STATIC_REGISTER(gio);  GST_PLUGIN_STATIC_REGISTER(pango);  GST_PLUGIN_STATIC_REGISTER(rawparse);  GST_PLUGIN_STATIC_REGISTER(typefindfunctions);  GST_PLUGIN_STATIC_REGISTER(videoconvert);  GST_PLUGIN_STATIC_REGISTER(videorate);  GST_PLUGIN_STATIC_REGISTER(videoscale);  GST_PLUGIN_STATIC_REGISTER(videotestsrc);  GST_PLUGIN_STATIC_REGISTER(volume);  GST_PLUGIN_STATIC_REGISTER(autodetect);  GST_PLUGIN_STATIC_REGISTER(videofilter);  GST_PLUGIN_STATIC_REGISTER(playback);  GST_PLUGIN_STATIC_REGISTER(subparse);  GST_PLUGIN_STATIC_REGISTER(ogg);  GST_PLUGIN_STATIC_REGISTER(theora);  GST_PLUGIN_STATIC_REGISTER(vorbis);  GST_PLUGIN_STATIC_REGISTER(opus);  GST_PLUGIN_STATIC_REGISTER(ivorbisdec);  GST_PLUGIN_STATIC_REGISTER(alaw);  GST_PLUGIN_STATIC_REGISTER(apetag);  GST_PLUGIN_STATIC_REGISTER(audioparsers);  GST_PLUGIN_STATIC_REGISTER(auparse);  GST_PLUGIN_STATIC_REGISTER(avi);  GST_PLUGIN_STATIC_REGISTER(dv);  GST_PLUGIN_STATIC_REGISTER(flac);  GST_PLUGIN_STATIC_REGISTER(flv);  GST_PLUGIN_STATIC_REGISTER(flxdec);  GST_PLUGIN_STATIC_REGISTER(icydemux);  GST_PLUGIN_STATIC_REGISTER(id3demux);  GST_PLUGIN_STATIC_REGISTER(isomp4);  GST_PLUGIN_STATIC_REGISTER(jpeg);  GST_PLUGIN_STATIC_REGISTER(lame);  GST_PLUGIN_STATIC_REGISTER(matroska);  GST_PLUGIN_STATIC_REGISTER(mpg123);  GST_PLUGIN_STATIC_REGISTER(mulaw);  GST_PLUGIN_STATIC_REGISTER(multipart);  GST_PLUGIN_STATIC_REGISTER(png);  GST_PLUGIN_STATIC_REGISTER(speex);  GST_PLUGIN_STATIC_REGISTER(taglib);  GST_PLUGIN_STATIC_REGISTER(vpx);  GST_PLUGIN_STATIC_REGISTER(wavenc);  GST_PLUGIN_STATIC_REGISTER(wavpack);  GST_PLUGIN_STATIC_REGISTER(wavparse);  GST_PLUGIN_STATIC_REGISTER(y4menc);  GST_PLUGIN_STATIC_REGISTER(adpcmdec);  GST_PLUGIN_STATIC_REGISTER(adpcmenc);  GST_PLUGIN_STATIC_REGISTER(dashdemux);  GST_PLUGIN_STATIC_REGISTER(dvbsuboverlay);  GST_PLUGIN_STATIC_REGISTER(dvdspu);  GST_PLUGIN_STATIC_REGISTER(hls);  GST_PLUGIN_STATIC_REGISTER(id3tag);  GST_PLUGIN_STATIC_REGISTER(kate);  GST_PLUGIN_STATIC_REGISTER(midi);  GST_PLUGIN_STATIC_REGISTER(mxf);  GST_PLUGIN_STATIC_REGISTER(openh264);  GST_PLUGIN_STATIC_REGISTER(opusparse);  GST_PLUGIN_STATIC_REGISTER(pcapparse);  GST_PLUGIN_STATIC_REGISTER(pnm);  GST_PLUGIN_STATIC_REGISTER(rfbsrc);  GST_PLUGIN_STATIC_REGISTER(siren);  GST_PLUGIN_STATIC_REGISTER(smoothstreaming);  GST_PLUGIN_STATIC_REGISTER(subenc);  GST_PLUGIN_STATIC_REGISTER(videoparsersbad);  GST_PLUGIN_STATIC_REGISTER(y4mdec);  GST_PLUGIN_STATIC_REGISTER(jpegformat);  GST_PLUGIN_STATIC_REGISTER(gdp);  GST_PLUGIN_STATIC_REGISTER(rsvg);  GST_PLUGIN_STATIC_REGISTER(openjpeg);  GST_PLUGIN_STATIC_REGISTER(spandsp);  GST_PLUGIN_STATIC_REGISTER(sbc);  GST_PLUGIN_STATIC_REGISTER(androidmedia);  GST_PLUGIN_STATIC_REGISTER(tcp);  GST_PLUGIN_STATIC_REGISTER(rtsp);  GST_PLUGIN_STATIC_REGISTER(rtp);  GST_PLUGIN_STATIC_REGISTER(rtpmanager);  GST_PLUGIN_STATIC_REGISTER(soup);  GST_PLUGIN_STATIC_REGISTER(udp);  GST_PLUGIN_STATIC_REGISTER(dtls);  GST_PLUGIN_STATIC_REGISTER(sdpelem);  GST_PLUGIN_STATIC_REGISTER(srtp);  GST_PLUGIN_STATIC_REGISTER(webrtc);  GST_PLUGIN_STATIC_REGISTER(rtspclientsink);  GST_PLUGIN_STATIC_REGISTER(opengl);  GST_PLUGIN_STATIC_REGISTER(opensles);  GST_PLUGIN_STATIC_REGISTER(openglmixers);  GST_PLUGIN_STATIC_REGISTER(alpha);  GST_PLUGIN_STATIC_REGISTER(alphacolor);  GST_PLUGIN_STATIC_REGISTER(audiofx);  GST_PLUGIN_STATIC_REGISTER(cairo);  GST_PLUGIN_STATIC_REGISTER(cutter);  GST_PLUGIN_STATIC_REGISTER(debug);  GST_PLUGIN_STATIC_REGISTER(deinterlace);  GST_PLUGIN_STATIC_REGISTER(dtmf);  GST_PLUGIN_STATIC_REGISTER(effectv);  GST_PLUGIN_STATIC_REGISTER(equalizer);  GST_PLUGIN_STATIC_REGISTER(gdkpixbuf);  GST_PLUGIN_STATIC_REGISTER(imagefreeze);  GST_PLUGIN_STATIC_REGISTER(interleave);  GST_PLUGIN_STATIC_REGISTER(level);  GST_PLUGIN_STATIC_REGISTER(multifile);  GST_PLUGIN_STATIC_REGISTER(replaygain);  GST_PLUGIN_STATIC_REGISTER(shapewipe);  GST_PLUGIN_STATIC_REGISTER(smpte);  GST_PLUGIN_STATIC_REGISTER(spectrum);  GST_PLUGIN_STATIC_REGISTER(videobox);  GST_PLUGIN_STATIC_REGISTER(videocrop);  GST_PLUGIN_STATIC_REGISTER(videomixer);  GST_PLUGIN_STATIC_REGISTER(accurip);  GST_PLUGIN_STATIC_REGISTER(aiff);  GST_PLUGIN_STATIC_REGISTER(audiofxbad);  GST_PLUGIN_STATIC_REGISTER(autoconvert);  GST_PLUGIN_STATIC_REGISTER(bayer);  GST_PLUGIN_STATIC_REGISTER(coloreffects);  GST_PLUGIN_STATIC_REGISTER(debugutilsbad);  GST_PLUGIN_STATIC_REGISTER(fieldanalysis);  GST_PLUGIN_STATIC_REGISTER(freeverb);  GST_PLUGIN_STATIC_REGISTER(frei0r);  GST_PLUGIN_STATIC_REGISTER(gaudieffects);  GST_PLUGIN_STATIC_REGISTER(geometrictransform);  GST_PLUGIN_STATIC_REGISTER(inter);  GST_PLUGIN_STATIC_REGISTER(interlace);  GST_PLUGIN_STATIC_REGISTER(ivtc);  GST_PLUGIN_STATIC_REGISTER(legacyrawparse);  GST_PLUGIN_STATIC_REGISTER(proxy);  GST_PLUGIN_STATIC_REGISTER(removesilence);  GST_PLUGIN_STATIC_REGISTER(segmentclip);  GST_PLUGIN_STATIC_REGISTER(smooth);  GST_PLUGIN_STATIC_REGISTER(speed);  GST_PLUGIN_STATIC_REGISTER(soundtouch);  GST_PLUGIN_STATIC_REGISTER(videofiltersbad);  GST_PLUGIN_STATIC_REGISTER(compositor);  GST_PLUGIN_STATIC_REGISTER(webrtcdsp);  GST_PLUGIN_STATIC_REGISTER(ladspa);  GST_PLUGIN_STATIC_REGISTER(audiobuffersplit);  GST_PLUGIN_STATIC_REGISTER(camerabin);  GST_PLUGIN_STATIC_REGISTER(encoding);  GST_PLUGIN_STATIC_REGISTER(asfmux);  GST_PLUGIN_STATIC_REGISTER(dtsdec);  GST_PLUGIN_STATIC_REGISTER(faad);  GST_PLUGIN_STATIC_REGISTER(mpegpsdemux);  GST_PLUGIN_STATIC_REGISTER(mpegpsmux);  GST_PLUGIN_STATIC_REGISTER(mpegtsdemux);  GST_PLUGIN_STATIC_REGISTER(mpegtsmux);  GST_PLUGIN_STATIC_REGISTER(voaacenc);  GST_PLUGIN_STATIC_REGISTER(a52dec);  GST_PLUGIN_STATIC_REGISTER(amrnb);  GST_PLUGIN_STATIC_REGISTER(amrwbdec);  GST_PLUGIN_STATIC_REGISTER(asf);  GST_PLUGIN_STATIC_REGISTER(dvdsub);  GST_PLUGIN_STATIC_REGISTER(dvdlpcmdec);  GST_PLUGIN_STATIC_REGISTER(mpeg2dec);  GST_PLUGIN_STATIC_REGISTER(xingmux);  GST_PLUGIN_STATIC_REGISTER(realmedia);  GST_PLUGIN_STATIC_REGISTER(x264);  GST_PLUGIN_STATIC_REGISTER(libav);
}

/* Call this function to load GIO modules */
void
gst_android_load_gio_modules (void)
{
  GST_G_IO_MODULE_LOAD(gnutls);
}

static void
glib_print_handler (const gchar * string)
{
  __android_log_print (ANDROID_LOG_INFO, "GLib+stdout", "%s", string);
}

static void
glib_printerr_handler (const gchar * string)
{
  __android_log_print (ANDROID_LOG_ERROR, "GLib+stderr", "%s", string);
}


/* Based on GLib's default handler */
#define CHAR_IS_SAFE(wc) (!((wc < 0x20 && wc != '\t' && wc != '\n' && wc != '\r') || \
			    (wc == 0x7f) || \
			    (wc >= 0x80 && wc < 0xa0)))
#define FORMAT_UNSIGNED_BUFSIZE ((GLIB_SIZEOF_LONG * 3) + 3)
#define	STRING_BUFFER_SIZE	(FORMAT_UNSIGNED_BUFSIZE + 32)
#define	ALERT_LEVELS		(G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING)
#define DEFAULT_LEVELS (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE)
#define INFO_LEVELS (G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG)

static void
escape_string (GString * string)
{
  const char *p = string->str;
  gunichar wc;

  while (p < string->str + string->len) {
    gboolean safe;

    wc = g_utf8_get_char_validated (p, -1);
    if (wc == (gunichar) - 1 || wc == (gunichar) - 2) {
      gchar *tmp;
      guint pos;

      pos = p - string->str;

      /* Emit invalid UTF-8 as hex escapes 
       */
      tmp = g_strdup_printf ("\\x%02x", (guint) (guchar) * p);
      g_string_erase (string, pos, 1);
      g_string_insert (string, pos, tmp);

      p = string->str + (pos + 4);      /* Skip over escape sequence */

      g_free (tmp);
      continue;
    }
    if (wc == '\r') {
      safe = *(p + 1) == '\n';
    } else {
      safe = CHAR_IS_SAFE (wc);
    }

    if (!safe) {
      gchar *tmp;
      guint pos;

      pos = p - string->str;

      /* Largest char we escape is 0x0a, so we don't have to worry
       * about 8-digit \Uxxxxyyyy
       */
      tmp = g_strdup_printf ("\\u%04x", wc);
      g_string_erase (string, pos, g_utf8_next_char (p) - p);
      g_string_insert (string, pos, tmp);
      g_free (tmp);

      p = string->str + (pos + 6);      /* Skip over escape sequence */
    } else
      p = g_utf8_next_char (p);
  }
}

static void
glib_log_handler (const gchar * log_domain, GLogLevelFlags log_level,
    const gchar * message, gpointer user_data)
{
  gchar *string;
  GString *gstring;
  const gchar *domains;
  gint android_log_level;
  gchar *tag;

  if ((log_level & DEFAULT_LEVELS) || (log_level >> G_LOG_LEVEL_USER_SHIFT))
    goto emit;

  domains = g_getenv ("G_MESSAGES_DEBUG");
  if (((log_level & INFO_LEVELS) == 0) ||
      domains == NULL ||
      (strcmp (domains, "all") != 0 && (!log_domain
              || !strstr (domains, log_domain))))
    return;

emit:

  if (log_domain)
    tag = g_strdup_printf ("GLib+%s", log_domain);
  else
    tag = g_strdup ("GLib");

  switch (log_level & G_LOG_LEVEL_MASK) {
    case G_LOG_LEVEL_ERROR:
      android_log_level = ANDROID_LOG_ERROR;
      break;
    case G_LOG_LEVEL_CRITICAL:
      android_log_level = ANDROID_LOG_ERROR;
      break;
    case G_LOG_LEVEL_WARNING:
      android_log_level = ANDROID_LOG_WARN;
      break;
    case G_LOG_LEVEL_MESSAGE:
      android_log_level = ANDROID_LOG_INFO;
      break;
    case G_LOG_LEVEL_INFO:
      android_log_level = ANDROID_LOG_INFO;
      break;
    case G_LOG_LEVEL_DEBUG:
      android_log_level = ANDROID_LOG_DEBUG;
      break;
    default:
      android_log_level = ANDROID_LOG_INFO;
      break;
  }

  gstring = g_string_new (NULL);
  if (!message) {
    g_string_append (gstring, "(NULL) message");
  } else {
    GString * msg = g_string_new (message);
    escape_string (msg);
    g_string_append (gstring, msg->str);
    g_string_free (msg, TRUE);
  }
  string = g_string_free (gstring, FALSE);

  __android_log_print (android_log_level, tag, "%s", string);

  g_free (string);
  g_free (tag);
}

static void
gst_debug_logcat (GstDebugCategory * category, GstDebugLevel level,
    const gchar * file, const gchar * function, gint line,
    GObject * object, GstDebugMessage * message, gpointer unused)
{
  GstClockTime elapsed;
  gint android_log_level;
  gchar *tag;

  if (level > gst_debug_category_get_threshold (category))
    return;

  elapsed = GST_CLOCK_DIFF (_priv_gst_info_start_time,
      gst_util_get_timestamp ());

  switch (level) {
    case GST_LEVEL_ERROR:
      android_log_level = ANDROID_LOG_ERROR;
      break;
    case GST_LEVEL_WARNING:
      android_log_level = ANDROID_LOG_WARN;
      break;
    case GST_LEVEL_INFO:
      android_log_level = ANDROID_LOG_INFO;
      break;
    case GST_LEVEL_DEBUG:
      android_log_level = ANDROID_LOG_DEBUG;
      break;
    default:
      android_log_level = ANDROID_LOG_VERBOSE;
      break;
  }

  tag = g_strdup_printf ("GStreamer+%s",
      gst_debug_category_get_name (category));

  if (object) {
    gchar *obj;

    if (GST_IS_PAD (object) && GST_OBJECT_NAME (object)) {
      obj = g_strdup_printf ("<%s:%s>", GST_DEBUG_PAD_NAME (object));
    } else if (GST_IS_OBJECT (object) && GST_OBJECT_NAME (object)) {
      obj = g_strdup_printf ("<%s>", GST_OBJECT_NAME (object));
    } else if (G_IS_OBJECT (object)) {
      obj = g_strdup_printf ("<%s@%p>", G_OBJECT_TYPE_NAME (object), object);
    } else {
      obj = g_strdup_printf ("<%p>", object);
    }

    __android_log_print (android_log_level, tag,
        "%" GST_TIME_FORMAT " %p %s:%d:%s:%s %s\n",
        GST_TIME_ARGS (elapsed), g_thread_self (),
        file, line, function, obj, gst_debug_message_get (message));

    g_free (obj);
  } else {
    __android_log_print (android_log_level, tag,
        "%" GST_TIME_FORMAT " %p %s:%d:%s %s\n",
        GST_TIME_ARGS (elapsed), g_thread_self (),
        file, line, function, gst_debug_message_get (message));
  }
  g_free (tag);
}

static gboolean
get_application_dirs (JNIEnv * env, jobject context, gchar ** cache_dir,
    gchar ** files_dir)
{
  jclass context_class;
  jmethodID get_cache_dir_id, get_files_dir_id;
  jclass file_class;
  jmethodID get_absolute_path_id;
  jobject dir;
  jstring abs_path;
  const gchar *abs_path_str;

  *cache_dir = *files_dir = NULL;

  context_class = (*env)->GetObjectClass (env, context);
  if (!context_class) {
    return FALSE;
  }
  get_cache_dir_id =
      (*env)->GetMethodID (env, context_class, "getCacheDir",
      "()Ljava/io/File;");
  get_files_dir_id =
      (*env)->GetMethodID (env, context_class, "getFilesDir",
      "()Ljava/io/File;");
  if (!get_cache_dir_id || !get_files_dir_id) {
    (*env)->DeleteLocalRef (env, context_class);
    return FALSE;
  }

  file_class = (*env)->FindClass (env, "java/io/File");
  if (!file_class) {
    (*env)->DeleteLocalRef (env, context_class);
    return FALSE;
  }
  get_absolute_path_id =
      (*env)->GetMethodID (env, file_class, "getAbsolutePath",
      "()Ljava/lang/String;");
  if (!get_absolute_path_id) {
    (*env)->DeleteLocalRef (env, context_class);
    (*env)->DeleteLocalRef (env, file_class);
    return FALSE;
  }

  dir = (*env)->CallObjectMethod (env, context, get_cache_dir_id);
  if ((*env)->ExceptionCheck (env)) {
    (*env)->ExceptionDescribe (env);
    (*env)->ExceptionClear (env);
    (*env)->DeleteLocalRef (env, context_class);
    (*env)->DeleteLocalRef (env, file_class);
    return FALSE;
  }

  if (dir) {
    abs_path = (*env)->CallObjectMethod (env, dir, get_absolute_path_id);
    if ((*env)->ExceptionCheck (env)) {
      (*env)->ExceptionDescribe (env);
      (*env)->ExceptionClear (env);
      (*env)->DeleteLocalRef (env, dir);
      (*env)->DeleteLocalRef (env, context_class);
      (*env)->DeleteLocalRef (env, file_class);
      return FALSE;
    }
    abs_path_str = (*env)->GetStringUTFChars (env, abs_path, NULL);
    if ((*env)->ExceptionCheck (env)) {
      (*env)->ExceptionDescribe (env);
      (*env)->ExceptionClear (env);
      (*env)->DeleteLocalRef (env, abs_path);
      (*env)->DeleteLocalRef (env, dir);
      (*env)->DeleteLocalRef (env, context_class);
      (*env)->DeleteLocalRef (env, file_class);
      return FALSE;
    }
    *cache_dir = abs_path ? g_strdup (abs_path_str) : NULL;

    (*env)->ReleaseStringUTFChars (env, abs_path, abs_path_str);
    (*env)->DeleteLocalRef (env, abs_path);
    (*env)->DeleteLocalRef (env, dir);
  }

  dir = (*env)->CallObjectMethod (env, context, get_files_dir_id);
  if ((*env)->ExceptionCheck (env)) {
    (*env)->ExceptionDescribe (env);
    (*env)->ExceptionClear (env);
    (*env)->DeleteLocalRef (env, context_class);
    (*env)->DeleteLocalRef (env, file_class);
    return FALSE;
  }
  if (dir) {
    abs_path = (*env)->CallObjectMethod (env, dir, get_absolute_path_id);
    if ((*env)->ExceptionCheck (env)) {
      (*env)->ExceptionDescribe (env);
      (*env)->ExceptionClear (env);
      (*env)->DeleteLocalRef (env, dir);
      (*env)->DeleteLocalRef (env, context_class);
      (*env)->DeleteLocalRef (env, file_class);
      return FALSE;
    }
    abs_path_str = (*env)->GetStringUTFChars (env, abs_path, NULL);
    if ((*env)->ExceptionCheck (env)) {
      (*env)->ExceptionDescribe (env);
      (*env)->ExceptionClear (env);
      (*env)->DeleteLocalRef (env, abs_path);
      (*env)->DeleteLocalRef (env, dir);
      (*env)->DeleteLocalRef (env, context_class);
      (*env)->DeleteLocalRef (env, file_class);
      return FALSE;
    }
    *files_dir = files_dir ? g_strdup (abs_path_str) : NULL;

    (*env)->ReleaseStringUTFChars (env, abs_path, abs_path_str);
    (*env)->DeleteLocalRef (env, abs_path);
    (*env)->DeleteLocalRef (env, dir);
  }

  (*env)->DeleteLocalRef (env, file_class);
  (*env)->DeleteLocalRef (env, context_class);

  return TRUE;
}

jobject
gst_android_get_application_context (void)
{
  return _context;
}

jobject
gst_android_get_application_class_loader (void)
{
  return _class_loader;
}

JavaVM *
gst_android_get_java_vm (void)
{
  return _java_vm;
}

static gboolean
init (JNIEnv *env, jobject context)
{
  jclass context_cls = NULL;
  jmethodID get_class_loader_id = 0;

  jobject class_loader = NULL;

  context_cls = (*env)->GetObjectClass (env, context);
  if (!context_cls) {
    return FALSE;
  }

  get_class_loader_id = (*env)->GetMethodID (env, context_cls,
      "getClassLoader", "()Ljava/lang/ClassLoader;");
  if ((*env)->ExceptionCheck (env)) {
    (*env)->ExceptionDescribe (env);
    (*env)->ExceptionClear (env);
    return FALSE;
  }

  class_loader = (*env)->CallObjectMethod (env, context, get_class_loader_id);
  if ((*env)->ExceptionCheck (env)) {
    (*env)->ExceptionDescribe (env);
    (*env)->ExceptionClear (env);
    return FALSE;
  }

  if (_context) {
    (*env)->DeleteGlobalRef (env, _context);
  }
  _context = (*env)->NewGlobalRef (env, context);

  if (_class_loader) {
    (*env)->DeleteGlobalRef (env, _class_loader);
  }
  _class_loader = (*env)->NewGlobalRef (env, class_loader);

  return TRUE;
}

void
gst_android_init (JNIEnv * env, jobject context)
{
  gchar *cache_dir;
  gchar *files_dir;
  gchar *registry;
  GError *error = NULL;

  if (!init (env, context)) {
    __android_log_print (ANDROID_LOG_INFO, "GStreamer",
        "GStreamer failed to initialize");
  }

  if (gst_is_initialized ()) {
    __android_log_print (ANDROID_LOG_INFO, "GStreamer",
        "GStreamer already initialized");
    return;
  }

  if (!get_application_dirs (env, context, &cache_dir, &files_dir)) {
    __android_log_print (ANDROID_LOG_ERROR, "GStreamer",
        "Failed to get application dirs");
  }

  if (cache_dir) {
    g_setenv ("TMP", cache_dir, TRUE);
    g_setenv ("TEMP", cache_dir, TRUE);
    g_setenv ("TMPDIR", cache_dir, TRUE);
    g_setenv ("XDG_RUNTIME_DIR", cache_dir, TRUE);
    g_setenv ("XDG_CACHE_HOME", cache_dir, TRUE);
    registry = g_build_filename (cache_dir, "registry.bin", NULL);
    g_setenv ("GST_REGISTRY", registry, TRUE);
    g_free (registry);
    g_setenv ("GST_REGISTRY_REUSE_PLUGIN_SCANNER", "no", TRUE);
    /* TODO: Should probably also set GST_PLUGIN_SCANNER and GST_PLUGIN_SYSTEM_PATH */
  }
  if (files_dir) {
    gchar *fontconfig, *certs;

    g_setenv ("HOME", files_dir, TRUE);
    g_setenv ("XDG_DATA_DIRS", files_dir, TRUE);
    g_setenv ("XDG_CONFIG_DIRS", files_dir, TRUE);
    g_setenv ("XDG_CONFIG_HOME", files_dir, TRUE);
    g_setenv ("XDG_DATA_HOME", files_dir, TRUE);

    fontconfig = g_build_filename (files_dir, "fontconfig", NULL);
    g_setenv ("FONTCONFIG_PATH", fontconfig, TRUE);
    g_free (fontconfig);

    certs = g_build_filename (files_dir, "ssl", "certs", "ca-certificates.crt", NULL);
    g_setenv ("CA_CERTIFICATES", certs, TRUE);
    g_free (certs);
  }
  g_free (cache_dir);
  g_free (files_dir);

  /* Set GLib print handlers */
  g_set_print_handler (glib_print_handler);
  g_set_printerr_handler (glib_printerr_handler);
  g_log_set_default_handler (glib_log_handler, NULL);

  /* Disable this for releases if performance is important
   * or increase the threshold to get more information */
  gst_debug_set_active (TRUE);
  gst_debug_set_default_threshold (GST_LEVEL_WARNING);
  gst_debug_remove_log_function (gst_debug_log_default);
  gst_debug_add_log_function ((GstLogFunction) gst_debug_logcat, NULL, NULL);

  /* get time we started for debugging messages */
  _priv_gst_info_start_time = gst_util_get_timestamp ();

  if (!gst_init_check (NULL, NULL, &error)) {
    gchar *message = g_strdup_printf ("GStreamer initialization failed: %s",
        error && error->message ? error->message : "(no message)");
    jclass exception_class = (*env)->FindClass (env, "java/lang/Exception");
    __android_log_print (ANDROID_LOG_ERROR, "GStreamer", "%s", message);
    (*env)->ThrowNew (env, exception_class, message);
    g_free (message);
    return;
  }
  gst_android_register_static_plugins ();
  gst_android_load_gio_modules ();
  __android_log_print (ANDROID_LOG_INFO, "GStreamer",
      "GStreamer initialization complete");
}

static void
gst_android_init_jni (JNIEnv * env, jobject gstreamer, jobject context)
{
  gst_android_init (env, context);
}

static JNINativeMethod native_methods[] = {
  {"nativeInit", "(Landroid/content/Context;)V", (void *) gst_android_init_jni}
};

jint
JNI_OnLoad (JavaVM * vm, void * reserved)
{
  JNIEnv *env = NULL;
  GModule *module;

  if ((*vm)->GetEnv (vm, (void **) &env, JNI_VERSION_1_4) != JNI_OK) {
    __android_log_print (ANDROID_LOG_ERROR, "GStreamer",
        "Could not retrieve JNIEnv");
    return 0;
  }
  jclass klass = (*env)->FindClass (env, "org/freedesktop/gstreamer/GStreamer");
  if (!klass) {
    __android_log_print (ANDROID_LOG_ERROR, "GStreamer",
        "Could not retrieve class org.freedesktop.gstreamer.GStreamer");
    return 0;
  }
  if ((*env)->RegisterNatives (env, klass, native_methods,
          G_N_ELEMENTS (native_methods))) {
    __android_log_print (ANDROID_LOG_ERROR, "GStreamer",
        "Could not register native methods for org.freedesktop.gstreamer.GStreamer");
    return 0;
  }

  /* Remember Java VM */
  _java_vm = vm;

  /* Tell the androidmedia plugin about the Java VM if we can */
  module = g_module_open (NULL, G_MODULE_BIND_LOCAL);
  if (module) {
    void (*set_java_vm) (JavaVM *) = NULL;

    if (g_module_symbol (module, "gst_amc_jni_set_java_vm",
          (gpointer *) & set_java_vm) && set_java_vm) {
      set_java_vm (vm);
    }
    g_module_close (module);
  }

  return JNI_VERSION_1_4;
}

void
JNI_OnUnload (JavaVM * vm, void * reversed)
{
  JNIEnv *env = NULL;

  if ((*vm)->GetEnv (vm, (void **) &env, JNI_VERSION_1_4) != JNI_OK) {
    __android_log_print (ANDROID_LOG_ERROR, "GStreamer",
        "Could not retrieve JNIEnv");
    return;
  }

  if (_context) {
    (*env)->DeleteGlobalRef (env, _context);
    _context = NULL;
  }

  if (_class_loader) {
    (*env)->DeleteGlobalRef (env, _class_loader);
    _class_loader = NULL;
  }

  _java_vm = NULL;
}
