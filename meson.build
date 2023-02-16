project('esthrower', 'cpp', default_options : ['cpp_std=c++17'])
gtkdep = dependency('gtkmm-3.0')
curldep = dependency('libcurl')
archivedep = dependency('libarchive')
#Check the platform and add a install_dir for the executable appropriate for the platform
if host_machine.system() == 'windows'
    install_dir = 'C:/Program Files/ESThrower'
elif host_machine.system() == 'linux'
    install_dir = '/usr/bin'
elif host_machine.system() == 'darwin'
    install_dir = '/usr/bin'
else #Generic UNIX
    install_dir = '/usr/bin'
endif
executable('esthrower', ['src/main.cpp'], dependencies : [gtkdep, curldep, archivedep], install : true, install_dir : install_dir)