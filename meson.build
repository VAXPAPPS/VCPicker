project('vaxp-color-picker', 'c',
          version: '0.1.0',
    meson_version: '>= 0.62.0',
  default_options: [ 'warning_level=2', 'werror=false', 'c_std=gnu11', ],
)

i18n = import('i18n')

cc = meson.get_compiler('c')

dependency('gtk4', version: '>= 4.10.0')
dependency('libadwaita-1', version: '>= 1.4.0')
dependency('glib-2.0')
dependency('gio-2.0')
dependency('gio-unix-2.0')
m_dep = cc.find_library('m', required : true)

vaxp_deps = [
  dependency('gtk4'),
  dependency('libadwaita-1'),
  dependency('glib-2.0'),
  dependency('gio-2.0'),
  dependency('gio-unix-2.0'),
  m_dep
]

subdir('data')
subdir('src')

