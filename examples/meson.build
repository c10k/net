progs = [['name_of_exe1', ['example1.cpp', 'example2.cpp']]]

foreach p : progs
  executable(p[0], p[1], include_directories : inc, link_with : netlib)
endforeach
