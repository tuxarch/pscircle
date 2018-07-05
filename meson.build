project(
	'pscircle',
	'c',
	default_options : [
		'buildtype=release'
	]
)

config = configuration_data()
config.set('version', '1.0.3')
config.set('NDEBUG', not get_option('buildtype').startswith('debug'))

psc_main = [
	'src/pscircle.c'
]

psc_sources = [
	'src/color.c',
	'src/point.c',
	'src/ppoint.c',
	'src/node.c',
	'src/pnode.c',
	'src/timing.c',
	'src/painter.c',
	'src/procs.c',
	'src/proc_linux.c',
	'src/proc_stream.c',
	'src/cfg.c',
	'src/tree_visualizer.c',
	'src/toplist_visualizer.c',
	'src/argparser.c',
	'src/utils.c',
]

incdir = [
	include_directories('.'),
	include_directories('include')
]

cc = meson.get_compiler('c')

deps = [
	dependency('cairo'),
	dependency('libpng'),
	cc.find_library('m', required : false),
]

x11_dep = dependency('x11', required : false)
x11_opt = get_option('enable-x11')

if x11_dep.found() and x11_opt
	config.set('HAVE_X11', true)
	deps += x11_dep
else
	config.set('HAVE_X11', false)
endif

test_flags = [
	'-march=native',
	'-ffast-math'
]

cflags = []

foreach flag : test_flags
	if cc.has_argument(flag)
		cflags += flag
	endif
endforeach

test_functions = [
	'sincos'
]

foreach func : test_functions
	if cc.has_function(func)
		config.set('HAVE_' + func.underscorify().to_upper(), true)
	endif
endforeach

configure_file(
	input : 'config.h.meson',
	output : 'config.h',
	configuration : config
)

psc_library = static_library(
	'pscircle',
	sources : psc_sources,
	include_directories : incdir,
	dependencies : deps,
	c_args : cflags
)

executable(
	'pscircle',
	sources : psc_main,
	include_directories : incdir,
	link_with : psc_library,
	dependencies : deps,
	c_args : cflags,
	install: true
)

if get_option('buildtype').startswith('debug')
	subdir('tests')
endif
