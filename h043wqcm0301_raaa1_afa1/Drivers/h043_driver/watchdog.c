#include "watchdog.h"
//#include "lpc177x_8x_pinsel.h"
//#include "lpc177x_8x_gpio.h"
//#include "lpc177x_8x_wwdt.h"
#include "timer.h"
#include "hmi_config.h"
//#include "debug_frmwrk.h"
#include "gpio.h"


/*******************************************************************************
* Function     : watchdog_init
* Description  : ���Ź���ʼ��
* Input Para   : NULL
* Output Para  : NULL
* Return Value : NULL
*******************************************************************************/
void watchdog_init(void)
{
#if 0
    /* Initialize debug via UART0
     * ?115200bps
     * ?8 data bit
     * ?No parity
     * ?1 stop bit
     * ?No flow control
     */
    //debug_frmwrk_init();

    // print welcome screen
    //_DBG(menu1);
    
    //_DBG_("MUC Reset!\n\r");
    
    // Read back TimeOut flag to determine previous timeout reset
    if (WWDT_GetStatus(WWDT_TIMEOUT_FLAG))
    {
        //_DBG_("Last MCU reset caused by WDT TimeOut!\n\r");
        
        // Clear WDT TimeOut
        WWDT_ClrTimeOutFlag();
    } 
    else
    {
        //_DBG_("Last MCU reset caused by External!\n\r");
    }

    // Initialize WDT, IRC OSC, interrupt mode, timeout = 5000000us = 5s
    WWDT_Init(30000000);

    WWDT_Enable(ENABLE);

    WWDT_SetMode(WWDT_RESET_MODE, ENABLE);
    
    // Start watchdog with timeout given
    WWDT_Start(25000000);

    WWDT_Feed();
    
    //infinite loop to wait chip reset from WDT
//    while(1)
//    {
//        delay_xms(1000);
//        WWDT_Feed();//ι��
//        
//    }
#endif
}

/*******************************************************************************
* Function     : watchdog_task
* Description  : ���Ź�����
* Input Para   : 
-->pvParameters:
* Output Para  : NULL
* Return Value : NULL
*******************************************************************************/
void watchdog_task(void *pvParameters)
{
    watchdog_init();
    while(1)
    {
        //PRINT("watchdog run\r\n");
		WWDT_Feed();
        delay_xms(2000);
    }
}

