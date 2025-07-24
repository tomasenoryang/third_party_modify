name = 'nmcli'

# FIXME: nmcli-completion should be renamed to nmcli
install_data(
  'nmcli-completion',
  install_dir: join_paths(nm_datadir, 'bash-completion', 'completions'),
)

sources = files(
  'agent.c',
  'common.c',
  'connections.c',
  'devices.c',
  'general.c',
  'nmcli.c',
  'polkit-agent.c',
  'settings.c',
  'utils.c',
)

deps = [
  libnmc_base_dep,
  libnmc_dep,
  readline_dep,
]

if enable_polkit_agent
  sources += nm_polkit_listener

  deps += polkit_agent_dep
endif

# Build shared library
libnmcli = shared_library(
  name,
  sources,
  dependencies: deps,
  c_args: clients_c_flags + ['-DG_LOG_DOMAIN="@0@"'.format(name), '-g', '-DBUILDING_LIBRARY'],
  version: '1.0.0',
  soversion: '1',
  install: true,
  install_dir: join_paths(nm_libdir, 'libnmcli'),
)

# Build static library (without main function)
libnmcli_static = static_library(
  name + '_static',
  sources,
  dependencies: deps,
  c_args: clients_c_flags + ['-DG_LOG_DOMAIN="@0@"'.format(name), '-g', '-DBUILDING_LIBRARY'],
  install: true,
  install_dir: join_paths(nm_libdir, 'libnmcli'),
)

# Create dependency object for the library
libnmcli_dep = declare_dependency(
  link_with: [libnmcli, libnmcli_static],
  include_directories: include_directories('.'),
  dependencies: deps,
)

# Install header files
install_headers(
  'libnmcli.h',
  subdir: 'libnmcli',
)

# Create pkg-config file
pkg = import('pkgconfig')
pkg.generate(
  libnmcli,
  name: 'libnmcli',
  description: 'NetworkManager CLI library',
  version: '1.0.0',
  requires: ['libnm', 'glib-2.0'],
  requires_private: ['readline'],
  subdirs: 'libnmcli',
  filebase: 'libnmcli',
)

# Also build executable for backward compatibility
executable(
  name + '-bin',
  sources,
  dependencies: deps,
  c_args: clients_c_flags + ['-DG_LOG_DOMAIN="@0@"'.format(name)],
  link_args: ldflags_linker_script_binary,
  link_depends: linker_script_binary,
  install: true,
)

# Build example program
# if enable_tests
#   executable(
#     name + '-example',
#     'nmcli-example.c',
#     dependencies: [libnmcli_dep],
#     install: false,
#   )
# endif
