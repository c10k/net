test_sources = ['test1.cpp', 'test2.cpp']

testexe = executable('testexe', test_sources,
        include_directories : inc, link_with : netlib,
        dependencies : [gtest])

test('app test', testexe, args: '--gtest_color=yes')
