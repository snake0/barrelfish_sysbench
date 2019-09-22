#ifndef SB_UTILS_H
#define SB_UTILS_H

enum err {
  SB_OK,
  SB_OPTION_SYNTAX,
  SB_OPTION_UNKNOWN,
  SB_TEST_UNKNOWN,
  SB_OPTION_TYPE,
  SB_MAX
};
/*
  Calculate the smallest multiple of m that is not smaller than n, when m is a
  power of 2.
*/
#define SB_ALIGN(n, m) (((n) + ((m) - 1)) & ~((m) - 1))

/*
  Calculate padding, i.e. distance from n to SB_ALIGN(n, m), where m is a power
  of 2.
*/
#define SB_PAD(n, m) (SB_ALIGN((n),(m)) - (n))

/* Calculate padding to cache line size. */
#ifndef CK_MD_CACHELINE
#define CK_MD_CACHELINE (64)
#endif
#define SB_CACHELINE_PAD(n) (SB_PAD((n), CK_MD_CACHELINE))

#define SB_UNLIKELY(x) (__builtin_expect(!!(x), 0))
/*
static char *errmsg[] = {
  [SB_OPTION_SYNTAX]="Invalid option syntax",
  [SB_OPTION_UNKNOWN]="Unknown option",
  [SB_TEST_UNKNOWN]="Unknown test",
  [SB_OPTION_TYPE]="Option type error",
};

#define ASSERT(f) do { \
  int r = (f); \
  if (r) printf("SYSBENCH: %s\n",errmsg[r]);\
} while(0)
*/
# define SB_ATTRIBUTE_UNUSED __attribute__((unused))

/* Compile-time assertion */
#define SB_COMPILE_TIME_ASSERT(expr)                                    \
  do {                                                                  \
    typedef char cta[(expr) ? 1 : -1] SB_ATTRIBUTE_UNUSED;              \
  } while(0)


#endif // SB_UTILS_H