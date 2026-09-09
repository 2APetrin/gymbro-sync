#ifndef GYMBRO_LOGGING_HH
#define GYMBRO_LOGGING_HH

#include <array>
#include <cstdio>
#include <format>
#include <string_view>
#include <utility>

// Logging functionality for gymbro-sync project
//
// There are two parameters:
// 1. comp-time parameter GYMBRO_LOG_LEVEL_MIN { debug, info, warning, error }
//    set it via -DGYMBRO_LOG_LEVEL_MIN=<level>
//    debug by default
// 2. run-time parameter which tags are enabled for logging
//    set it via Log::turn_on<Log::core>() and Log::turn_off<Log::core>()
//
// NOTE: Logging via macros because we dont want to evaluate arguments when the
//       level is excluded at compile-time or the tag is off
//
// TODO: it would be nice to have a way to log into a file
// TODO: to make it faster we can buffer the logs and flush them periodically
// TODO: if we want to go mutltithreaded, we should increment buffer atomically

namespace gymbro::detail::log {

#ifndef GYMBRO_LOG_LEVEL_MIN
#define GYMBRO_LOG_LEVEL_MIN debug
#endif

class Logger {
  static constexpr std::array<std::string_view, 4> level_prefixes = {
      "debug", "info", "warning", "error"};

  static constexpr std::array<std::string_view, 2> tag_prefixes = {"core",
                                                                   "db"};

  // NOTE: may face false sharing if we go multithreaded but its ok for now
  static inline bool enabled_tags[tag_prefixes.size()] = {false, false};

 public:
  enum class Level { debug, info, warning, error, off };
  enum class Tag { core, db };
  static constexpr Level min_level = Level::GYMBRO_LOG_LEVEL_MIN;

  template <Tag T, typename... Args>
  static void debug(std::format_string<Args...> fmt, Args&&... args) {
    if constexpr (min_level > Level::debug) return;
    log_at<Level::debug, T>(fmt, std::forward<Args>(args)...);
  }

  template <Tag T, typename... Args>
  static void info(std::format_string<Args...> fmt, Args&&... args) {
    if constexpr (min_level > Level::info) return;
    log_at<Level::info, T>(fmt, std::forward<Args>(args)...);
  }

  template <Tag T, typename... Args>
  static void warning(std::format_string<Args...> fmt, Args&&... args) {
    if constexpr (min_level > Level::warning) return;
    log_at<Level::warning, T>(fmt, std::forward<Args>(args)...);
  }

  template <Tag T, typename... Args>
  static void error(std::format_string<Args...> fmt, Args&&... args) {
    if constexpr (min_level > Level::error) return;
    log_at<Level::error, T>(fmt, std::forward<Args>(args)...);
  }

  template <Tag T>
  static bool is_on() {
    return enabled_tags[static_cast<unsigned>(T)];
  }

  template <Tag T>
  static void turn_on() {
    enabled_tags[static_cast<unsigned>(T)] = true;
  }

  template <Tag T>
  static void turn_off() {
    enabled_tags[static_cast<unsigned>(T)] = false;
  }

 private:
  template <Level L, Tag T, typename... Args>
  static void log_at(std::format_string<Args...> fmt, Args&&... args) {
    if (!enabled_tags[static_cast<unsigned>(T)]) return;
    const auto body = std::format(fmt, std::forward<Args>(args)...);
    std::fprintf(stdout, "[%s][%s] %s\n",
                 level_prefixes[static_cast<unsigned>(L)].data(),
                 tag_prefixes[static_cast<unsigned>(T)].data(), body.c_str());
  }
};

}  // namespace gymbro::detail::log

namespace gymbro::log {

using enum detail::log::Logger::Tag;

template <detail::log::Logger::Tag T>
inline void turn_on() {
  detail::log::Logger::turn_on<T>();
}

template <detail::log::Logger::Tag T>
inline void turn_off() {
  detail::log::Logger::turn_off<T>();
}

}  // namespace gymbro::log

// Forced to use macros because we want zero-cost logging when the level is
// excluded at compile-time or the tag is off
#define GYMBRO_LOG_AT(level, tag, fmt, ...)                          \
  do {                                                               \
    if constexpr (!(::gymbro::detail::log::Logger::min_level >       \
                    ::gymbro::detail::log::Logger::Level::level)) {  \
      if (::gymbro::detail::log::Logger::is_on<tag>()) {             \
        ::gymbro::detail::log::Logger::level<tag>(fmt __VA_OPT__(, ) \
                                                      __VA_ARGS__);  \
      }                                                              \
    }                                                                \
  } while (0)

#define log_debug(tag, ...) GYMBRO_LOG_AT(debug, tag, __VA_ARGS__)
#define log_info(tag, ...) GYMBRO_LOG_AT(info, tag, __VA_ARGS__)
#define log_warning(tag, ...) GYMBRO_LOG_AT(warning, tag, __VA_ARGS__)
#define log_error(tag, ...) GYMBRO_LOG_AT(error, tag, __VA_ARGS__)

#endif  // GYMBRO_LOGGING_HH
