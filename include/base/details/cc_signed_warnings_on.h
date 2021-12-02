/*
 * Useful to avoid dependency on type traits - for example, if integers are used
 * as template arguments.
 */
#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable: 4389)
#elif defined(__clang__) && defined(__has_warning)
#  if __has_warning("-Wsign-compare")
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wsign-compare"
#  endif
#elif defined(__GNUC__) && !(defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || defined(__ECC)) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 406
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wsign-compare"
#endif
