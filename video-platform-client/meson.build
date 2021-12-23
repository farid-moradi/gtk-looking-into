project('video-platform-client', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

gstdep = dependency('gstreamer-1.0')
gtkdep = dependency('gtk+-3.0')
gstvdep = dependency('gstreamer-video-1.0')

exe = executable('video_platform_client', 'video_platform_client.c', dependencies: [gstdep, gtkdep, gstvdep],
  install : true)

test('basic', exe)
