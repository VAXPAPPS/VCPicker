glib_compile_resources = find_program('glib-compile-resources', required: true)

resources = custom_target(
  'vaxp-color-picker-resources',
  input: 'vaxp-color-picker.gresource.xml',
  output: ['vaxp_resources.c', 'vaxp_resources.h'],
  command: [
    glib_compile_resources,
    '--generate',
    '--target=@OUTPUT0@',
    '--c-name=vaxp',
    '--sourcedir=' + meson.current_source_dir(),
    '@INPUT@'
  ]
)

install_data('org.vaxp.ColorPicker.desktop',
  install_dir: get_option('datadir') / 'applications'
)

install_data('org.vaxp.ColorPicker.gschema.xml',
  install_dir: get_option('datadir') / 'glib-2.0' / 'schemas'
)

install_data('org.vaxp.ColorPicker.svg',
  install_dir: get_option('datadir') / 'icons' / 'hicolor' / 'scalable' / 'apps'
)
