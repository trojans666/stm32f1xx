#include "bsp_redefined.h"
#include "bsp_board.h"

static int stm32_board_start(void)
{
    return 0;
}
__INIT_EXPORT(stm32_board_start,"0.end");

static int stm32_board_end(void)
{
    return 0;
}
__INIT_EXPORT(stm32_board_end,"1.end");

void bsp_board_init(void)
{
    const init_fn_t *fn_ptr;
    for(fn_ptr = &__board_init__stm32_board_start; fn_ptr < &__board_init__stm32_board_end;fn_ptr++)
    {
        (*fn_ptr)();
    }
}
