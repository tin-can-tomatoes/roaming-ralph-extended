# just for the sake of example,
# we're not gonna traverse into the subdirs here,
# instead just reference them from here.
drunken_includes = include_directories('include')
drunken_sources = files(
  'src/cActor.cpp',
  'src/cActorInterval.cpp',
  'src/cBufferViewer.cpp',
  'src/cOnscreenText.cpp',
  'src/cSoundInterval.cpp',
  'src/genericFunctionInterval.cpp',
  'src/sleepWhenMinimized.cpp',
)

# I suppose one could build a dynamic_library if desired,
# I don't think it would be much different here
drunken_lib = static_library(
  'drunken-octo-robot',
  drunken_sources,
  include_directories: drunken_includes,
  dependencies: panda_dep,
  cpp_args: ['-Wno-reorder'] # Suppress warnings again,,,,,
)

# declare the dependency
drunken_dep = declare_dependency(
  include_directories: drunken_includes,
  link_with: drunken_lib,
  dependencies: panda_dep,
)

# add the dependency to the game's dep list
game_deps += drunken_dep
