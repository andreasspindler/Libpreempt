/*
 * Undo signed_warnings_on.h
 *
 * Reenable "signed" warnings
 */
#if defined(_MSC_VER)
#  pragma warning(pop)
#elif defined(__clang__) && defined(__has_warning)
#  if __has_warning("-Wsign-compare")
#  pragma clang diagnostic pop
#  endif
#elif defined(__GNUC__) && !(defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || defined(__ECC)) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 406
#  pragma GCC diagnostic pop
#endif
