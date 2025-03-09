project(
  'roaming-ralph-extended',
  'cpp',
  version: '0.1.0',
  meson_version: '>=1.1',
  default_options: ['c_std=c23', 'cpp_std=c++23'],
)

# This project demonstrates using the meson build system to compile an example Panda3d game

# This was created on an Arch Linux system where Panda3d was installed from the AUR
# after installing a number of optional dependencies.
# Your mileage with this may vary on other systems or configurations.

# before we get started we'll store some information about the project
game_title_str = 'Roaming Ralph Extended Example'
game_slug_str = meson.project_name()
game_version_str = meson.project_version()
game_version_arr = game_version_str.split('-')[0].split('.')
game_version_major = game_version_arr[0].to_int()
game_version_minor = game_version_arr[1].to_int()
game_version_micro = game_version_arr[2].to_int()

message('Configuring ', game_title_str)

# First important step - we gotta get dependencies together
# Unfortunately panda3d doesn't come with a pkgconfig file.
# If it did, we could simply retrieve it with:
#   panda_dep = dependency('panda3d', version: '>=1.10.0')
# And that would be a much more reliable solution, given
# we'd be checking the version is compatible as well.

# Instead, we're gonna do it manually, and by default we're assuming we're
# going to find everything in the system install prefix on Arch Linux, /usr/.

# This can be changed by configuring the panda3d_headers_prefix and
# panda3d_libraries_prefix when configuring the project with meson

# If you're trying to get this to work on other systems and run into problems
# with missing headers or linker failures, the options file, along with this
# section might be a good place to start looking.

# We'll soon use the compiler to test for the presence of panda3d libs
# If pkg-config was an option, we wouldn't to do all this.
cpp = meson.get_compiler('cpp')

panda_headers_prefix = get_option('panda3d_headers_prefix')
panda_includes = [include_directories(panda_headers_prefix / 'panda3d')]

# To test for the libraries, meson's compiler.find_library function is used
# It's more or less equivalent to `g++ -L<lib_dir> -l<libName>`
panda_lib_dir = get_option('panda3d_libraries_prefix') / 'panda3d'
panda_libs = [
  cpp.find_library('p3framework', dirs: panda_lib_dir, required: true),
  cpp.find_library('panda', dirs: panda_lib_dir, required: true),
  cpp.find_library('pandafx', dirs: panda_lib_dir, required: true),
  cpp.find_library('pandaexpress', dirs: panda_lib_dir, required: true),
  cpp.find_library('p3dtoolconfig', dirs: panda_lib_dir, required: true),
  cpp.find_library('p3dtool', dirs: panda_lib_dir, required: true),
  cpp.find_library('p3direct', dirs: panda_lib_dir, required: true),
]
using_eigen = false
using_bullet = false
if get_option('check_eigen3_depend')
  eigen3_dep = dependency('eigen3', required: get_option('check_eigen3_required'))
  if eigen3_dep.found()
    panda_libs += eigen3_dep
    using_eigen = true
  endif
endif
if get_option('check_bullet_depend')
  bullet_dep = dependency('bullet', required: get_option('check_bullet_required'))
  if bullet_dep.found()
    using_bullet = true
    panda_libs += bullet_dep
  endif
endif

# Now that we've found the libraries and include dirs, we'll wrap em in a
# dependency object to make it easier to consume later.
panda_dep = declare_dependency(include_directories: panda_includes, dependencies: panda_libs)

# declare some arrays that we will append to as meson traverses the subdirectories
game_includes = [include_directories('include')]
game_sources = []
game_deps = [panda_dep]

# before we start compiling anything, let's assemble a configuration file
# go check out the meson.build file inside the config subdir to see how it works
subdir('config')

# Process the meson.build file in the 'ext' subdirectory, which in our case
# contains instructions to build a static library with some additional functionality
# and then declares a dependency to wrap it, addint the dep to game_deps
subdir('ext')
subdir('lib')

# Now do the 'src' subdirectory, which in our case adds the cpp files to game_sources
subdir('src')

# Now that all the sources, includes, and deps are together, finally build the executable.
# Since this statement is made in the root meson.build file, the resultant program
# will be placed in the root of the build dir.
# Many projects using meson will actually do this inside subdirectories, but
# I don't like when the programs are buried deep in the build dir.
executable(
  game_slug_str,
  game_sources,
  dependencies: game_deps,
  include_directories: game_includes,
  # Panda3d's includes generate a lot of reorder warnings for some reason.
  # It doesn't seem to bother this project so I suppressed em.
  cpp_args: ['-Wno-reorder'],
)
