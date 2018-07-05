tests = [
	['color', ['color.cc']],
	['point', ['point.cc']],
	['ppoint', ['ppoint.cc']],
	['node', ['node.cc']],
	['procs', ['procs.cc']],
	['argparser', ['argparser.cc']],
	['cfg', ['cfg.cc']],
]

add_languages('cpp')

gtest_dep = dependency('gtest', main : true, required : false)

if gtest_dep.found()
	deps += gtest_dep

	foreach t : tests
		exe = executable(
			t[0],
			t[1],
			include_directories : incdir,
			link_with : psc_library,
			dependencies : deps,
		)

		test(t[0], exe)
	endforeach
endif

