#ifndef __REDEFINED_H
#define __REDEFINED_H


/* arm compiler */
#if defined(__CC_ARM)
    #define DEF_SECTION(x)                  __attribute__((section(x)))
    #define DEF_USED                        __attribute__((used))
    #define DEF_UNUSED                      __attribute__((unused))
    #define DEF_ALIGN(n)                    __attribute__((aligned(n))
    #define DEF_WEAK                        __attribute__((weak))
    #define DEF_INLINE                      static __inline
#elif defined (__IAR_SYSTEMS_ICC__)     /* for IAR Compiler */
    #include <stdarg.h>
    #define DEF_SECTION(x)                  @ x
    #define DEF_UNUSED
    #define DEF_USED                     __root
    #define DEF_PRAGMA(x)                   _Pragma(#x)
    #define DEF_ALIGN(n)                    PRAGMA(data_alignment=n)
    #define DEF_WEAK                     __weak
    #define DEF_INLINE                   static inline
#endif

/**
 * @ingroup
 *
 * @def ALIGN(size, align)
 * Return the most contiguous size aligned at specified width. RT_ALIGN(13, 4)
 * would return 16.
 */
#define __ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))

/**
 * @ingroup
 *
 * @def ALIGN_DOWN(size, align)
 * Return the down number of aligned at specified width. RT_ALIGN_DOWN(13, 4)
 * would return 12.
 */
#define ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

typedef int (*init_fn_t)(void);
#define __INIT_EXPORT(fn, level)       \
            DEF_USED const init_fn_t __board_init__##fn DEF_SECTION(".board__fn."level) = fn

#define STM32_INIT_BOARD_EXPORT(fn)           __INIT_EXPORT(fn,"1")



#endif // __REDEFINED_H
