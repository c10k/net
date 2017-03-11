test_sources = ['socket_bind_test.cpp', 'socket_constructor_test.cpp',
				'socket_options_test.cpp', 'socket_getSocket_test.cpp',
                                'socket_send_test.cpp']

testexe = executable('testexe', test_sources,
        include_directories : inc, link_with : netlib,
        dependencies : [gtest])

test('app test', testexe, args: '--gtest_color=yes')
