# just for the sake of example,
# we're not gonna traverse into the subdirs here,
# instead just reference them from here.
gamelib_includes = [include_directories('include'), config_includes]
gamelib_sources = files(
  'src/config_loader.cpp',
  'src/input_control.cpp',
  'src/input_controller.cpp',
  'src/input_controller_static.cpp',
  'src/orbit_camera.cpp',
)

# I suppose one could build a dynamic_library if desired,
# I don't think it would be much different here
game_lib = static_library(
  game_slug_str + '_lib',
  gamelib_sources,
  include_directories: gamelib_includes,
  dependencies: panda_dep,
  cpp_args: ['-Wno-reorder'] # Suppress warnings again,,,,,,,
)

# declare the dependency
gamelib_dep = declare_dependency(
  include_directories: gamelib_includes,
  link_with: game_lib,
  dependencies: panda_dep,
)

# add the dependency to the game's dep list
game_deps += gamelib_dep
