// Приемник работает в паре с Trancmiter2d используется для отладки процедуры
// на С++ Builder
// 31.07.24 Приемник неожиданно начал зависать после приема 3-17 пакетов при этом сохраняд работоспособность в режиме отладки
// вылечилось увеличением стека TASKSTACKSIZE с 1024 до 2048 как оказалось отжирает стэк передача по UART (!)
//

#include <CC1310R.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/devices/cc13x0/driverlib/ioc.h>
#include <ti/devices/cc13x0/driverlib/udma.h>
#include <ti/devices/cc13x0/inc/hw_ints.h>
#include <ti/devices/cc13x0/inc/hw_memmap.h>

#include <ti/drivers/ADCBuf.h>
#include <ti/drivers/adcbuf/ADCBufCC26XX.h>

//Для прямого доступа к регистрам
#include <ti/devices/cc13x0/inc/hw_memmap.h>
#include <ti/devices/cc13x0/inc/hw_types.h>
#include <ti/devices/cc13x0/driverlib/sys_ctrl.h>
#include "easylink/EasyLink.h" //Библиотека функций RF

//Режим приема пакета данных
#define MASTER_ID_SC                 0xF1 //МДД КЗ
#define MASTER_ID_BASE               0xFF //МДД BASE
#define MASTER_ID_FLASH              0xFA //Прошивка
#define MASTER_ID_DEBUG_S            0xDC //Debug Slava Тензодатчик
#define MASTER_ID_DEBUG_T            0xDD //Debug пересылка payload[] в UART как текстовой строки
#define MASTER_ID_DEBUG_A            0xDE //Debug Ток в реальном времени
#define MASTER_ID_DEBUG_R           0xDB //Ретранслятор
#define MASTER_ID_DEBUG_TAcc         0xDF //Debug Данные акселерометра в реальном времени
#define GROUP_CODE                   0xA1 //вводим коды групп устройств. А1,А2,А3,А4 в группе максимум 4 устройства устройства принимают пакеты только от членов группы

/*
 *  =============================== ADC ===============================
 */
#include <ti/drivers/ADC.h>
#include <ti/drivers/adc/ADCCC26XX.h>

ADCCC26XX_Object adcCC26xxObjects[CC1310_LAUNCHXL_ADCCOUNT];

const ADCCC26XX_HWAttrs adcCC26xxHWAttrs[CC1310_LAUNCHXL_ADCCOUNT] = {
    {
        .adcDIO              = CC1310_LAUNCHXL_DIO23_ANALOG,
        .adcCompBInput       = ADC_COMPB_IN_AUXIO7,
        .refSource           = ADCCC26XX_FIXED_REFERENCE,
        .samplingDuration    = ADCCC26XX_SAMPLING_DURATION_2P7_US,
        .inputScalingEnabled = true,
        .triggerSource       = ADCCC26XX_TRIGGER_MANUAL,
        .returnAdjustedVal   = false
    },
    {
        .adcDIO              = CC1310_LAUNCHXL_DIO24_ANALOG,
        .adcCompBInput       = ADC_COMPB_IN_AUXIO6,
        .refSource           = ADCCC26XX_FIXED_REFERENCE,
        .samplingDuration    = ADCCC26XX_SAMPLING_DURATION_2P7_US,
        .inputScalingEnabled = true,
        .triggerSource       = ADCCC26XX_TRIGGER_MANUAL,
        .returnAdjustedVal   = false
    },
    {
        .adcDIO              = CC1310_LAUNCHXL_DIO25_ANALOG,
        .adcCompBInput       = ADC_COMPB_IN_AUXIO5,
        .refSource           = ADCCC26XX_FIXED_REFERENCE,
        .samplingDuration    = ADCCC26XX_SAMPLING_DURATION_2P7_US,
        .inputScalingEnabled = true,
        .triggerSource       = ADCCC26XX_TRIGGER_MANUAL,
        .returnAdjustedVal   = false
    },
    {
        .adcDIO              = CC1310_LAUNCHXL_DIO26_ANALOG,
        .adcCompBInput       = ADC_COMPB_IN_AUXIO4,
        .refSource           = ADCCC26XX_FIXED_REFERENCE,
        .samplingDuration    = ADCCC26XX_SAMPLING_DURATION_2P7_US,
        .inputScalingEnabled = true,
        .triggerSource       = ADCCC26XX_TRIGGER_MANUAL,
        .returnAdjustedVal   = false
    },
};

const ADC_Config ADC_config[CC1310_LAUNCHXL_ADCCOUNT] = {
    {&ADCCC26XX_fxnTable, &adcCC26xxObjects[CC1310_LAUNCHXL_ADC0], &adcCC26xxHWAttrs[CC1310_LAUNCHXL_ADC0]},
    {&ADCCC26XX_fxnTable, &adcCC26xxObjects[CC1310_LAUNCHXL_ADC1], &adcCC26xxHWAttrs[CC1310_LAUNCHXL_ADC1]},
    {&ADCCC26XX_fxnTable, &adcCC26xxObjects[CC1310_LAUNCHXL_ADC2], &adcCC26xxHWAttrs[CC1310_LAUNCHXL_ADC2]},
    {&ADCCC26XX_fxnTable, &adcCC26xxObjects[CC1310_LAUNCHXL_ADC3], &adcCC26xxHWAttrs[CC1310_LAUNCHXL_ADC3]},
  //  {&ADCCC26XX_fxnTable, &adcCC26xxObjects[CC1310_LAUNCHXL_ADC4], &adcCC26xxHWAttrs[CC1310_LAUNCHXL_ADC4]},
  //  {&ADCCC26XX_fxnTable, &adcCC26xxObjects[CC1310_LAUNCHXL_ADC5], &adcCC26xxHWAttrs[CC1310_LAUNCHXL_ADC5]},
  //  {&ADCCC26XX_fxnTable, &adcCC26xxObjects[CC1310_LAUNCHXL_ADC6], &adcCC26xxHWAttrs[CC1310_LAUNCHXL_ADC6]},
  //  {&ADCCC26XX_fxnTable, &adcCC26xxObjects[CC1310_LAUNCHXL_ADC7], &adcCC26xxHWAttrs[CC1310_LAUNCHXL_ADC7]},
  //  {&ADCCC26XX_fxnTable, &adcCC26xxObjects[CC1310_LAUNCHXL_ADCDCOUPL], &adcCC26xxHWAttrs[CC1310_LAUNCHXL_ADCDCOUPL]},
  //  {&ADCCC26XX_fxnTable, &adcCC26xxObjects[CC1310_LAUNCHXL_ADCVSS], &adcCC26xxHWAttrs[CC1310_LAUNCHXL_ADCVSS]},
   // {&ADCCC26XX_fxnTable, &adcCC26xxObjects[CC1310_LAUNCHXL_ADCVDDS], &adcCC26xxHWAttrs[CC1310_LAUNCHXL_ADCVDDS]},
};

const uint_least8_t ADC_count = CC1310_LAUNCHXL_ADCCOUNT;

/*
 *  =============================== Crypto ===============================
 */
#include <ti/drivers/crypto/CryptoCC26XX.h>

CryptoCC26XX_Object cryptoCC26XXObjects[CC1310_LAUNCHXL_CRYPTOCOUNT];

const CryptoCC26XX_HWAttrs cryptoCC26XXHWAttrs[CC1310_LAUNCHXL_CRYPTOCOUNT] = {
    {
        .baseAddr       = CRYPTO_BASE,
        .powerMngrId    = PowerCC26XX_PERIPH_CRYPTO,
        .intNum         = INT_CRYPTO_RESULT_AVAIL_IRQ,
        .intPriority    = ~0,
    }
};

const CryptoCC26XX_Config CryptoCC26XX_config[CC1310_LAUNCHXL_CRYPTOCOUNT] = {
    {
         .object  = &cryptoCC26XXObjects[CC1310_LAUNCHXL_CRYPTO0],
         .hwAttrs = &cryptoCC26XXHWAttrs[CC1310_LAUNCHXL_CRYPTO0]
    }
};

/*
 *  =============================== GPIO ===============================
 */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOCC26XX.h>

/*
 * Array of Pin configurations
 * NOTE: The order of the pin configurations must coincide with what was
 *       defined in CC1310_LAUNCHXL.h
 * NOTE: Pins not used for interrupts should be placed at the end of the
 *       array. Callback entries can be omitted from callbacks array to
 *       reduce memory usage.
 */
GPIO_PinConfig gpioPinConfigs[] = {
    /* Input pins */
    //GPIOCC26XX_DIO_11 | GPIO_CFG_IN_NOPULL | GPIO_CFG_IN_INT_RISING,  // xl int

    /* Led pins */
    GPIOCC26XX_DIO_07 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,  /* Green LED */
    GPIOCC26XX_DIO_06 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,  /* Red LED */

    /* Output pins */
    GPIOCC26XX_DIO_18 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_HIGH,  //FLASH_CS
    GPIOCC26XX_DIO_01 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,/*axel power*/

  //  GPIOCC26XX_DIO_12 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,  //LMT01_POW_PIN
    GPIOCC26XX_DIO_30 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,  //Enable_4V
    GPIOCC26XX_DIO_22 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,  //GSM_PWR_KEY
    //GPIOCC26XX_DIO_28 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,  //Enable_3.3v
};

/*
 * Array of callback function pointers
 * NOTE: The order of the pin configurations must coincide with what was
 *       defined in CC1310_LAUNCH.h
 * NOTE: Pins not used for interrupts can be omitted from callbacks array to
 *       reduce memory usage (if placed at end of gpioPinConfigs array).
 */
GPIO_CallbackFxn gpioCallbackFunctions[] = {
    NULL,  /* Button 0 */
    NULL,  /* Button 1 */
};

const GPIOCC26XX_Config GPIOCC26XX_config = {
    .pinConfigs = (GPIO_PinConfig *)gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *)gpioCallbackFunctions,
    .numberOfPinConfigs = CC1310_LAUNCHXL_GPIOCOUNT,
    .numberOfCallbacks  = sizeof(gpioCallbackFunctions)/sizeof(GPIO_CallbackFxn),
    .intPriority = (~0)
};

/*
 *  =============================== GPTimer ===============================
 *  Remove unused entries to reduce flash usage both in Board.c and Board.h
 */
#include <ti/drivers/timer/GPTimerCC26XX.h>

GPTimerCC26XX_Object gptimerCC26XXObjects[CC1310_LAUNCHXL_GPTIMERCOUNT];

const GPTimerCC26XX_HWAttrs gptimerCC26xxHWAttrs[CC1310_LAUNCHXL_GPTIMERPARTSCOUNT] = {
    { .baseAddr = GPT0_BASE, .intNum = INT_GPT0A, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT0, .pinMux = GPT_PIN_0A, },
  //  { .baseAddr = GPT0_BASE, .intNum = INT_GPT0B, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT0, .pinMux = GPT_PIN_0B, },
  //  { .baseAddr = GPT1_BASE, .intNum = INT_GPT1A, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT1, .pinMux = GPT_PIN_1A, },
 //   { .baseAddr = GPT1_BASE, .intNum = INT_GPT1B, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT1, .pinMux = GPT_PIN_1B, },
  //  { .baseAddr = GPT2_BASE, .intNum = INT_GPT2A, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT2, .pinMux = GPT_PIN_2A, },
  //  { .baseAddr = GPT2_BASE, .intNum = INT_GPT2B, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT2, .pinMux = GPT_PIN_2B, },
  //  { .baseAddr = GPT3_BASE, .intNum = INT_GPT3A, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT3, .pinMux = GPT_PIN_3A, },
   // { .baseAddr = GPT3_BASE, .intNum = INT_GPT3B, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT3, .pinMux = GPT_PIN_3B, },
};

const GPTimerCC26XX_Config GPTimerCC26XX_config[CC1310_LAUNCHXL_GPTIMERPARTSCOUNT] = {
    { &gptimerCC26XXObjects[CC1310_LAUNCHXL_GPTIMER0], &gptimerCC26xxHWAttrs[CC1310_LAUNCHXL_GPTIMER0A], GPT_A },
    //{ &gptimerCC26XXObjects[CC1310_LAUNCHXL_GPTIMER0], &gptimerCC26xxHWAttrs[CC1310_LAUNCHXL_GPTIMER0B], GPT_B },
   // { &gptimerCC26XXObjects[CC1310_LAUNCHXL_GPTIMER1], &gptimerCC26xxHWAttrs[CC1310_LAUNCHXL_GPTIMER1A], GPT_A },
   // { &gptimerCC26XXObjects[CC1310_LAUNCHXL_GPTIMER1], &gptimerCC26xxHWAttrs[CC1310_LAUNCHXL_GPTIMER1B], GPT_B },
   // { &gptimerCC26XXObjects[CC1310_LAUNCHXL_GPTIMER2], &gptimerCC26xxHWAttrs[CC1310_LAUNCHXL_GPTIMER2A], GPT_A },
  //  { &gptimerCC26XXObjects[CC1310_LAUNCHXL_GPTIMER2], &gptimerCC26xxHWAttrs[CC1310_LAUNCHXL_GPTIMER2B], GPT_B },
   // { &gptimerCC26XXObjects[CC1310_LAUNCHXL_GPTIMER3], &gptimerCC26xxHWAttrs[CC1310_LAUNCHXL_GPTIMER3A], GPT_A },
   // { &gptimerCC26XXObjects[CC1310_LAUNCHXL_GPTIMER3], &gptimerCC26xxHWAttrs[CC1310_LAUNCHXL_GPTIMER3B], GPT_B },
};

/*
 *  =============================== I2C ===============================
*/
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC26XX.h>

I2CCC26XX_Object i2cCC26xxObjects[CC1310_LAUNCHXL_I2CCOUNT];

const I2CCC26XX_HWAttrsV1 i2cCC26xxHWAttrs[CC1310_LAUNCHXL_I2CCOUNT] = {
    {
        .baseAddr    = I2C0_BASE,
        .powerMngrId = PowerCC26XX_PERIPH_I2C0,
        .intNum      = INT_I2C_IRQ,
        .intPriority = ~0,
        .swiPriority = 0,
        .sdaPin      = CC1310_LAUNCHXL_I2C0_SDA0,
        .sclPin      = CC1310_LAUNCHXL_I2C0_SCL0,
    }
};

const I2C_Config I2C_config[CC1310_LAUNCHXL_I2CCOUNT] = {
    {
        .fxnTablePtr = &I2CCC26XX_fxnTable,
        .object      = &i2cCC26xxObjects[CC1310_LAUNCHXL_I2C0],
        .hwAttrs     = &i2cCC26xxHWAttrs[CC1310_LAUNCHXL_I2C0]
    }
};

const uint_least8_t I2C_count = CC1310_LAUNCHXL_I2CCOUNT;

/*
 *  =============================== NVS ===============================
 */
#include <ti/drivers/NVS.h>
#include <ti/drivers/nvs/NVSSPI25X.h>
#include <ti/drivers/nvs/NVSCC26XX.h>

#define NVS_REGIONS_BASE 0x1B000
#define SECTORSIZE       0x1000
#define REGIONSIZE       (SECTORSIZE * 4)
#define VERIFYBUFSIZE    64

static uint8_t verifyBuf[VERIFYBUFSIZE];

/*
 * Place uninitialized array at NVS_REGIONS_BASE
 */
#pragma LOCATION(flashBuf, NVS_REGIONS_BASE);
#pragma NOINIT(flashBuf);
static char flashBuf[REGIONSIZE];

/* Allocate objects for NVS and NVS SPI */
NVSCC26XX_Object nvsCC26xxObjects[1];
NVSSPI25X_Object nvsSPI25XObjects[1];

/* Hardware attributes for NVS */
const NVSCC26XX_HWAttrs nvsCC26xxHWAttrs[1] = {
    {
        .regionBase = (void *)flashBuf,
        .regionSize = REGIONSIZE,
    },
};

/* Hardware attributes for NVS SPI */
const NVSSPI25X_HWAttrs nvsSPI25XHWAttrs[1] = {
    {
        .regionBaseOffset = 0,
        .regionSize = REGIONSIZE,
        .sectorSize = SECTORSIZE,
        .verifyBuf = verifyBuf,
        .verifyBufSize = VERIFYBUFSIZE,
        .spiHandle = NULL,
        .spiIndex = 0,
        .spiBitRate = 4000000,
        .spiCsnGpioIndex = CC1310_LAUNCHXL_GPIO_SPI_FLASH_CS,
    },
};

/* NVS Region index 0 and 1 refer to NVS and NVS SPI respectively */
const NVS_Config NVS_config[CC1310_LAUNCHXL_NVSCOUNT] = {
    {
        .fxnTablePtr = &NVSCC26XX_fxnTable,
        .object = &nvsCC26xxObjects[0],
        .hwAttrs = &nvsCC26xxHWAttrs[0],
    },
    {
        .fxnTablePtr = &NVSSPI25X_fxnTable,
        .object = &nvsSPI25XObjects[0],
        .hwAttrs = &nvsSPI25XHWAttrs[0],
    },
};

const uint_least8_t NVS_count = CC1310_LAUNCHXL_NVSCOUNT;


/*
 *  =============================== PIN ===============================
 */
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>

const PIN_Config BoardGpioInitTable[] = {

    CC1310_LAUNCHXL_PIN_RLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MIN,       /* LED initially off          */
    CC1310_LAUNCHXL_PIN_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MIN,       /* LED initially off          */
    CC1310_LAUNCHXL_DIO1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,           /*axel power*/

    CC1310_LAUNCHXL_SPI_FLASH_CS | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MIN,  /* External flash chip select */

    GSM_UART_RX | PIN_INPUT_EN | PIN_PULLDOWN,                                              /* UART RX via debugger back channel */
    GSM_UART_TX | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL,                        /* UART TX via debugger back channel */
    CC1310_LAUNCHXL_SPI0_MOSI | PIN_INPUT_EN | PIN_PULLDOWN,                                            /* SPI master out - slave in */
    CC1310_LAUNCHXL_SPI0_MISO | PIN_INPUT_EN | PIN_PULLDOWN,                                            /* SPI master in - slave out */
    CC1310_LAUNCHXL_SPI0_CLK | PIN_INPUT_EN | PIN_PULLDOWN,                                             /* SPI clock */

    PIN_TERMINATE
};

const PINCC26XX_HWAttrs PINCC26XX_hwAttrs = {
    .intPriority = ~0,
    .swiPriority = 0
};

/*
 *  =============================== Power ===============================
 */
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

const PowerCC26XX_Config PowerCC26XX_config = {
    .policyInitFxn      = NULL,
    .policyFxn          = &PowerCC26XX_standbyPolicy,
    .calibrateFxn       = &PowerCC26XX_calibrate,
    .enablePolicy       = true,
    .calibrateRCOSC_LF  = true,
    .calibrateRCOSC_HF  = true,
};


/*
 *  =============================== RF Driver ===============================
 */
#include <ti/drivers/rf/RF.h>

const RFCC26XX_HWAttrsV2 RFCC26XX_hwAttrs = {
    .hwiPriority        = ~0,       /* Lowest HWI priority */
    .swiPriority        = 0,        /* Lowest SWI priority */
    .xoscHfAlwaysNeeded = true,     /* Keep XOSC dependency while in standby */
    .globalCallback     = NULL,     /* No board specific callback */
    .globalEventMask    = 0         /* No events subscribed to */
};

/*
 *  =============================== SPI DMA ===============================
 */
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPICC26XXDMA.h>

SPICC26XXDMA_Object spiCC26XXDMAObjects[CC1310_LAUNCHXL_SPICOUNT];

const SPICC26XXDMA_HWAttrsV1 spiCC26XXDMAHWAttrs[CC1310_LAUNCHXL_SPICOUNT] = {
    {
        .baseAddr           = SSI0_BASE,
        .intNum             = INT_SSI0_COMB,
        .intPriority        = ~0,
        .swiPriority        = 0,
        .powerMngrId        = PowerCC26XX_PERIPH_SSI0,
        .defaultTxBufValue  = 0,
        .rxChannelBitMask   = 1<<UDMA_CHAN_SSI0_RX,
        .txChannelBitMask   = 1<<UDMA_CHAN_SSI0_TX,
        .mosiPin            = CC1310_LAUNCHXL_SPI0_MOSI,
        .misoPin            = CC1310_LAUNCHXL_SPI0_MISO,
        .clkPin             = CC1310_LAUNCHXL_SPI0_CLK,
        .csnPin             = CC1310_LAUNCHXL_SPI0_CSN,
        .minDmaTransferSize = 10
    },
};

const SPI_Config SPI_config[CC1310_LAUNCHXL_SPICOUNT] = {
    {
         .fxnTablePtr = &SPICC26XXDMA_fxnTable,
         .object      = &spiCC26XXDMAObjects[CC1310_LAUNCHXL_SPI0],
         .hwAttrs     = &spiCC26XXDMAHWAttrs[CC1310_LAUNCHXL_SPI0]
    },
};

const uint_least8_t SPI_count = CC1310_LAUNCHXL_SPICOUNT;

/*
 *  =============================== UART ===============================
 */
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC26XX.h>

UARTCC26XX_Object uartCC26XXObjects[CC1310_LAUNCHXL_UARTCOUNT];

uint8_t uartCC26XXRingBuffer[CC1310_LAUNCHXL_UARTCOUNT][32];

const UARTCC26XX_HWAttrsV2 uartCC26XXHWAttrs[CC1310_LAUNCHXL_UARTCOUNT] = {
    {
        .baseAddr       = UART0_BASE,
        .powerMngrId    = PowerCC26XX_PERIPH_UART0,
        .intNum         = INT_UART0_COMB,
        .intPriority    = ~0,
        .swiPriority    = 0,
        .txPin          = GSM_UART_TX,  //Назначаем PIN на котором будет TX UART
        .rxPin          = GSM_UART_RX,
        .ctsPin         = PIN_UNASSIGNED,
        .rtsPin         = PIN_UNASSIGNED,
        .ringBufPtr     = uartCC26XXRingBuffer[CC1310_LAUNCHXL_UART0],
        .ringBufSize    = sizeof(uartCC26XXRingBuffer[CC1310_LAUNCHXL_UART0]),
        .txIntFifoThr   = UARTCC26XX_FIFO_THRESHOLD_1_8,
                .rxIntFifoThr   = UARTCC26XX_FIFO_THRESHOLD_4_8,
                .errorFxn       = NULL
    },
};

const UART_Config UART_config[CC1310_LAUNCHXL_UARTCOUNT] = {
    {
        .fxnTablePtr = &UARTCC26XX_fxnTable,
        .object      = &uartCC26XXObjects[CC1310_LAUNCHXL_UART0],
        .hwAttrs     = &uartCC26XXHWAttrs[CC1310_LAUNCHXL_UART0]
    },
};

const uint_least8_t UART_count = CC1310_LAUNCHXL_UARTCOUNT;

/*
 *  =============================== UDMA ===============================
 */
#include <ti/drivers/dma/UDMACC26XX.h>

UDMACC26XX_Object udmaObjects[CC1310_LAUNCHXL_UDMACOUNT];

const UDMACC26XX_HWAttrs udmaHWAttrs[CC1310_LAUNCHXL_UDMACOUNT] = {
    {
        .baseAddr    = UDMA0_BASE,
        .powerMngrId = PowerCC26XX_PERIPH_UDMA,
        .intNum      = INT_DMA_ERR,
        .intPriority = ~0
    }
};

const UDMACC26XX_Config UDMACC26XX_config[CC1310_LAUNCHXL_UDMACOUNT] = {
    {
         .object  = &udmaObjects[CC1310_LAUNCHXL_UDMA0],
         .hwAttrs = &udmaHWAttrs[CC1310_LAUNCHXL_UDMA0]
    },
};

/*
 *  =============================== Watchdog ===============================
 */
#include <ti/drivers/Watchdog.h>
#include <ti/drivers/watchdog/WatchdogCC26XX.h>

WatchdogCC26XX_Object watchdogCC26XXObjects[CC1310_LAUNCHXL_WATCHDOGCOUNT];

const WatchdogCC26XX_HWAttrs watchdogCC26XXHWAttrs[CC1310_LAUNCHXL_WATCHDOGCOUNT] = {
    {
        .baseAddr    = WDT_BASE,
        .reloadValue = 1000 /* Reload value in milliseconds */
    },
};

const Watchdog_Config Watchdog_config[CC1310_LAUNCHXL_WATCHDOGCOUNT] = {
    {
        .fxnTablePtr = &WatchdogCC26XX_fxnTable,
        .object      = &watchdogCC26XXObjects[CC1310_LAUNCHXL_WATCHDOG0],
        .hwAttrs     = &watchdogCC26XXHWAttrs[CC1310_LAUNCHXL_WATCHDOG0]
    },
};

const uint_least8_t Watchdog_count = CC1310_LAUNCHXL_WATCHDOGCOUNT;

/*
 *  ======== CC1310_LAUNCHXL_sendExtFlashByte ========
 */
void CC1310_LAUNCHXL_sendExtFlashByte(PIN_Handle pinHandle, uint8_t byte)
{
    PIN_setOutputValue(pinHandle, CC1310_LAUNCHXL_SPI_FLASH_CS, 0);
    uint8_t i;
    for(i = 0; i < 8; i++){
        PIN_setOutputValue(pinHandle, CC1310_LAUNCHXL_SPI0_CLK, 0);
        PIN_setOutputValue(pinHandle, CC1310_LAUNCHXL_SPI0_MOSI, (byte >> (7 - i)) & 0x01);
        PIN_setOutputValue(pinHandle, CC1310_LAUNCHXL_SPI0_CLK, 1);
    }
    PIN_setOutputValue(pinHandle, CC1310_LAUNCHXL_SPI_FLASH_CS, 1);
}

/*
 *  ======== CC1310_LAUNCHXL_shutDownExtFlash ========
 */
void CC1310_LAUNCHXL_shutDownExtFlash(void)
{
    PIN_Config extFlashPinTable[] = {
        CC1310_LAUNCHXL_SPI_FLASH_CS | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_INPUT_DIS | PIN_DRVSTR_MED,
        CC1310_LAUNCHXL_SPI0_CLK | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_INPUT_DIS | PIN_DRVSTR_MED,
        CC1310_LAUNCHXL_SPI0_MOSI | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_INPUT_DIS | PIN_DRVSTR_MED,
        CC1310_LAUNCHXL_SPI0_MISO | PIN_INPUT_EN | PIN_PULLDOWN,
        PIN_TERMINATE
    };
    PIN_State extFlashPinState;
    PIN_Handle extFlashPinHandle = PIN_open(&extFlashPinState, extFlashPinTable);

    uint8_t extFlashStartup = 0xAB;
    uint8_t extFlashShutdown = 0xB9;

    CC1310_LAUNCHXL_sendExtFlashByte(extFlashPinHandle, extFlashStartup);
    CC1310_LAUNCHXL_sendExtFlashByte(extFlashPinHandle, extFlashShutdown);

    PIN_close(extFlashPinHandle);
}

//Переменная для определения RSSI
extern RF_Handle rfHandle; //Определена в EasyLink.c. Убрать тип static/

//RTOS Variables
Task_Params rxTaskParams;
Task_Struct rxTask;
Char rxTaskStack[RFTASKSTACKSIZE];

Task_Params txUARTParams;
Task_Struct txUARTTask;
Char txUARTStack[UARTTASKSTACKSIZE];

Semaphore_Handle ReadyForUART;
Semaphore_Struct ReadyForUARTStruct;


//Easy Link Variables
EasyLink_RxPacket rxPacket =  { {0}, 0, 0, {0} };     // Пакет данных для передачи по радиоканалу
EasyLink_TxPacket txPacket =  { {0}, 0, 0, {0} };     // Пакет данных для передачи по радиоканалу

uint32_t DeviceID;                                    // Присваивается в процедуре GSM_GetSIMNumber() модуля sim800_hub.c

//UART Variables
UART_Handle uart;
UART_Params uartParams;

//GLOBAL Variable
int16_t AccX[20],AccY[20],AccZ[20];
int8_t rssi1 ;
//GLOBAL Variable
unsigned int ADCValue[40],ADCValue2[40];
signed int adc_value1;
uint16_t size_of_print;
char str[800];

void RFModule_init(void);
void UARTModule_init(void);
void rfEasyLinkRxFnx(UArg arg0, UArg arg1);
void txUARTFnx(UArg arg0, UArg arg1);

//         >>>>>>>>>>>>>>>>>>>> main <<<<<<<<<<<<<<<<<<<<<<<<<<<<
int main()
{
    Task_Params rxTaskParams;
    Task_Params txUARTaskParams;
    Semaphore_Params ReadyForUARTParams;

    //Init Section
    Power_init(); //Это было в процедуре board_initGeneral()

    if (PIN_init(BoardGpioInitTable) != PIN_SUCCESS) {while (1);} //Установка семафоров для GPIO_init()
    GPIO_init();        //Аппаратная настройка портов ВВ
    UARTModule_init();  //UART init

    //Определяем Задачу в которой крутится функция приема пакета по радио rfEasyLinkTxFnx()
    Task_Params_init(&rxTaskParams);
    rxTaskParams.stackSize = RFTASKSTACKSIZE;
    rxTaskParams.priority = 1;
    rxTaskParams.stack = &rxTaskStack;
    Task_construct(&rxTask, (Task_FuncPtr)rfEasyLinkRxFnx, &rxTaskParams, NULL); //Поцедура rfEasyLinkRxFnx()

    //Определяем Задачу в которой крутится функция передачи пакета по UART
    Task_Params_init(&txUARTaskParams);
    txUARTaskParams.stackSize = UARTTASKSTACKSIZE;
    txUARTaskParams.priority = 1;
    txUARTaskParams.stack = &txUARTStack;
    Task_construct(&txUARTTask, (Task_FuncPtr)txUARTFnx, &txUARTaskParams, NULL); //Поцедура rfEasyLinkRxFnx()

    Semaphore_Params_init(&ReadyForUARTParams);   //Семафор готовности передачи по RF каналу
    ReadyForUARTParams.mode=Semaphore_Mode_BINARY;
    Semaphore_construct(&ReadyForUARTStruct, 0, &ReadyForUARTParams);
    ReadyForUART = Semaphore_handle(&ReadyForUARTStruct);

    BIOS_start(); //Передаем управление RTOS
}

// Init RF Module
//---------------------------------------------------------------------------------------
void RFModule_init(void)
{
  //  uint8_t addrFilter[EASYLINK_MAX_ADDR_SIZE * EASYLINK_MAX_ADDR_FILTERS] = {0xAA,0xA1};
    //uint8_t addrFilter[EASYLINK_MAX_ADDR_SIZE * EASYLINK_MAX_ADDR_FILTERS] = {0xAA, 0xE9,0xA1,0xA2,0xA3,0xA4,0xA5};
    EasyLink_init(EasyLink_Phy_Custom);
  //   EasyLink_setRfPwr(12);
  //  EasyLink_enableRxAddrFilter(addrFilter, 1, 2); //Для первой строчки 2, для второй 7
}

// Init UART Module
//---------------------------------------------------------------------------------------
void UARTModule_init(void)
{
    UART_init();
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.stopBits = UART_STOP_ONE;
    uartParams.baudRate = 115200;
    uartParams.readMode = UART_MODE_BLOCKING;
    //uartParams.writeCallback = writeCallback; //Прерывание
    uart = UART_open(0, &uartParams);
    if (uart == NULL) { while (1); }
}


// Эта функция определена как задача BIOS, при первом запуске инициализация RF модуля
// потом вечный цикл прослушки эфира, после приема пакета выставляет семафор rxDoneSem
// Данные акселерометра по 2 байта на компоненту старшие байты добиваем нулями
// В payload принимаем 20 троек AccX,AccY,AccZ по 2 байта
//---------------------------------------------------------------------------------------
void rfEasyLinkRxFnx(UArg arg0, UArg arg1)
{
//uint32_t DeviceID_rcv;
    RFModule_init(); //Инициализация RF модуля До запуска BIOS - не работает, поскольку пользуется семаформами
    while(1){
        //EasyLink_receiveAsync(rxDoneCb, 0); //Функция EasyLink API. Вызывает функцию rxDoneCb()
        if(EasyLink_receive(&rxPacket)==EasyLink_Status_Success) {
            GPIO_toggle(LED1);
            //Здесь можно прочесть RSSI
            rssi1  = rxPacket.rssi;
            //Режим приема пакета текстовых данных
            if(rxPacket.payload[0] == MASTER_ID_DEBUG_S ){
                //DeviceID_rcv = rxPacket.payload[3] | (rxPacket.payload[2]<<8) | (rxPacket.payload[1]<<16); //Собираем DeviceID_rcv
                Semaphore_post(ReadyForUART);
                Task_sleep(10000); //10 mS Время на передачу
            }
        }
    }
}

//Передаем принятые и декодированные данные в UART
//Сначала собираем Payload в общий буфер, возможно, в последствии будем фильтровать данные
//Потом пересылаем в UART в виде текстовых строк
//---------------------------------------------------------------------------------------
void txUARTFnx(UArg arg0, UArg arg1)
{
int i;
    while(1){
//Здесь
        Semaphore_pend(ReadyForUART, BIOS_WAIT_FOREVER);//Ждём семафора
        for(i=0; i<rxPacket.len; i++ )
        {
            txPacket.payload[i] = rxPacket.payload[i];
        }

        txPacket.payload[0] = MASTER_ID_DEBUG_R;
               txPacket.len        = 128; //
               txPacket.dstAddr[0] = GROUP_CODE;
               txPacket.absTime = 0;

               EasyLink_transmit(&txPacket); //Функция библиотеки EasyLink

               //Semaphore_post(ReadyForBlink);
               Task_sleep(100000);

         Task_sleep(10000); //10 mS Время на передачу
    }//End of while
}





