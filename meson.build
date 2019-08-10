project(
	'pscircle',
	'c',
	default_options : [
		'buildtype=release'
	],
	version: '1.3.1'
)

config = configuration_data()
config.set('version', meson.project_version())

psc_main = [
	'src/pscircle.c'
]

psc_sources = [
	'src/color.c',
	'src/point.c',
	'src/ppoint.c',
	'src/reals.c',
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

deps = [  ]

test_flags = [ '-D_GNU_SOURCE=1' ]

if get_option('buildtype') == 'release'
	test_flags += '-march=native'
	test_flags += '-ffast-math'
	test_flags += '-O3'
	test_flags += '-O4'
	test_flags += '-DNDEBUG=1'
elif get_option('buildtype') == 'plain'
	test_flags += '-DNDEBUG=1'
elif get_option('buildtype') == 'debug'
	# asan_dep = cc.find_library('asan', required : false)
	# if asan_dep.found()
	# 	deps += asan_dep
	# 	test_flags += '-fsanitize=address'
	# 	test_flags += '-fsanitize=leak'
	# endif
	test_flags += '-g'
	test_flags += '-ggdb'
	test_flags += '-O0'
endif

deps += dependency('cairo')
deps += dependency('libpng')
deps += cc.find_library('m', required : false)

x11_dep = dependency('x11', required : false)
if x11_dep.found() and get_option('enable-x11')
	config.set('HAVE_X11', true)
	deps += x11_dep
else
	config.set('HAVE_X11', false)
endif

cflags = [ ]

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
