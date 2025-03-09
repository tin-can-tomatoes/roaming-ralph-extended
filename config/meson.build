game_conf_data = configuration_data(
  {
    'GAME_TITLE_STR': game_title_str,
    'GAME_SLUG_STR': game_slug_str,
    'GAME_VERSION_STR': game_version_str,
    'GAME_VERSION_MAJOR': game_version_major,
    'GAME_VERSION_MINOR': game_version_minor,
    'GAME_VERSION_MICRO': game_version_micro,
    'GAME_DEBUG_BUILD': get_option('debug') or (get_option('buildtype') == 'debug'),
    'GAME_SHOW_COLLIDERS': get_option('show_colliders'),
    'GAME_HAS_EIGEN3': using_eigen,
    'GAME_HAS_BULLET': using_bullet,
    'DEBUG_LOG_KEYBOARD': get_option('debug_log_keyboard'),
    'DEBUG_LOG_GAMEPAD': get_option('debug_log_gamepad')

  },
)

configure_file(
  input: 'config.h.in',
  output: 'config.h',
  configuration: game_conf_data,
)

config_includes = include_directories('.')
game_includes += config_includes
