#ifndef _HMI_CONFIG_H_
#define _HMI_CONFIG_H_
#include "monitor.h"
/*config TFT*/
#define     TFT_TYPE_7_0            (0)
#define     TFT_TYPE_5_0            (1)
#define     TFT_TYPE_4_3            (2)

/*config TP*/

#define     TP_TYPE_ST1633I         (0)
#define     TP_TYPE_FT5426          (1)

//∫· ˙∆¡«–ªª÷ß≥÷
#define  DIR_SWITCH_SUPPORT         (1)

/*download src */
#define  SRC_DOWNLOAD_SDIO          (0) 
#define  SRC_DOWNLOAD_USB           (1)
#define  SRC_DOWNLOAD_VUSB          (2)

#define   LED_SUPPORT               (1)




#define   SYS_DEBUG                 (0)








#define     TFT_TYPE_SIZE      		TFT_TYPE_4_3
#define     TP_TYPE_DRIVER     		TP_TYPE_ST1633I
#define     SRC_DOWNLOAD_TYPE  		SRC_DOWNLOAD_VUSB




#if   SYS_DEBUG          

#define   PRINT(format,args...)         xprintf(format,##args)

#else

#define  PRINT(format,args...)

#endif






#endif


