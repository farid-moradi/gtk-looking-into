project('slide-color', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

exe = executable('slide_color', 'slide_color.c', dependencies: dependency('gtk+-3.0'),
  install : true)

test('basic', exe)
