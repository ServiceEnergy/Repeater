


#include <ti/drivers/PIN.h> //��� PIN.h �� ������� PIN_Config BoardGpioInitTable[];
extern const PIN_Config BoardGpioInitTable[];

#define RFTASKSTACKSIZE 1024
#define UARTTASKSTACKSIZE 2048

/* Mapping of pins to board signals using general board aliases
 *      <board signal alias>                  <pin mapping>   <comments>
 */

/* Analog capable DIOs */
#define CC1310_LAUNCHXL_DIO23_ANALOG          IOID_23
#define CC1310_LAUNCHXL_DIO24_ANALOG          IOID_24
#define CC1310_LAUNCHXL_DIO25_ANALOG          IOID_25
#define CC1310_LAUNCHXL_DIO26_ANALOG          IOID_26
#define CC1310_LAUNCHXL_DIO27_ANALOG          IOID_27
#define CC1310_LAUNCHXL_DIO28_ANALOG          IOID_28
#define CC1310_LAUNCHXL_DIO29_ANALOG          IOID_29
#define CC1310_LAUNCHXL_DIO30_ANALOG          IOID_30

/* Digital IOs */
#define CC1310_LAUNCHXL_DIO0                  IOID_0
#define CC1310_LAUNCHXL_DIO1                  IOID_1

//#define CC1310_LAUNCHXL_DIO12                 IOID_12
#define CC1310_LAUNCHXL_DIO15                 IOID_15
#define CC1310_LAUNCHXL_DIO16_TDO             IOID_16
#define CC1310_LAUNCHXL_DIO17_TDI             IOID_17
#define CC1310_LAUNCHXL_DIO21                 IOID_21
//#define CC1310_LAUNCHXL_DIO22                 IOID_22

/* Discrete Inputs */
#define CC1310_LAUNCHXL_PIN_BTN1              IOID_13
#define CC1310_LAUNCHXL_PIN_BTN2              IOID_14

/* GPIO */
#define CC1310_LAUNCHXL_GPIO_LED_ON           1
#define CC1310_LAUNCHXL_GPIO_LED_OFF          0

/* I2C */
#define CC1310_LAUNCHXL_I2C0_SCL0             IOID_4
#define CC1310_LAUNCHXL_I2C0_SDA0             IOID_5

/* LCD (430BOOST - Sharp96 Rev 1.1) */
#define CC1310_LAUNCHXL_LCD_CS                IOID_24  /* SPI chip select */
#define CC1310_LAUNCHXL_LCD_EXTCOMIN          IOID_12  /* External COM inversion */
#define CC1310_LAUNCHXL_LCD_ENABLE            IOID_22  /* LCD enable */
#define CC1310_LAUNCHXL_LCD_POWER             IOID_23  /* LCD power control */
#define CC1310_LAUNCHXL_LCD_CS_ON             1
#define CC1310_LAUNCHXL_LCD_CS_OFF            0

/* LEDs */
#define CC1310_LAUNCHXL_PIN_LED_ON            1
#define CC1310_LAUNCHXL_PIN_LED_OFF           0
#define CC1310_LAUNCHXL_PIN_RLED              IOID_6
#define CC1310_LAUNCHXL_PIN_GLED              IOID_7

/* PWM Outputs */
#define CC1310_LAUNCHXL_PWMPIN0               CC1310_LAUNCHXL_PIN_RLED
#define CC1310_LAUNCHXL_PWMPIN1               CC1310_LAUNCHXL_PIN_GLED
#define CC1310_LAUNCHXL_PWMPIN2               PIN_UNASSIGNED
#define CC1310_LAUNCHXL_PWMPIN3               PIN_UNASSIGNED
#define CC1310_LAUNCHXL_PWMPIN4               PIN_UNASSIGNED
#define CC1310_LAUNCHXL_PWMPIN5               PIN_UNASSIGNED
#define CC1310_LAUNCHXL_PWMPIN6               PIN_UNASSIGNED
#define CC1310_LAUNCHXL_PWMPIN7               PIN_UNASSIGNED

/* SPI */
#define CC1310_LAUNCHXL_SPI_FLASH_CS          IOID_18
#define CC1310_LAUNCHXL_FLASH_CS_ON           0
#define CC1310_LAUNCHXL_FLASH_CS_OFF          1

/* SPI Board */
#define CC1310_LAUNCHXL_SPI0_MISO             IOID_10
#define CC1310_LAUNCHXL_SPI0_MOSI             IOID_9
#define CC1310_LAUNCHXL_SPI0_CLK              IOID_8
#define CC1310_LAUNCHXL_SPI0_CSN              PIN_UNASSIGNED
#define CC1310_LAUNCHXL_SPI1_MISO             PIN_UNASSIGNED
#define CC1310_LAUNCHXL_SPI1_MOSI             PIN_UNASSIGNED
#define CC1310_LAUNCHXL_SPI1_CLK              PIN_UNASSIGNED
#define CC1310_LAUNCHXL_SPI1_CSN              PIN_UNASSIGNED

/* UART Board */
#define GSM_UART_RX               IOID_2          /* RXD */
#define GSM_UART_TX               IOID_3          /* TXD */

//#define GSM_UART_RX             IOID_21         /* RXD */
//#define GSM_UART_TX             IOID_20         /* TXD */

#define CC1310_LAUNCHXL_UART_CTS              PIN_UNASSIGNED       /* CTS */
#define CC1310_LAUNCHXL_UART_RTS              PIN_UNASSIGNED       /* RTS */

/*!
 *  @brief  Initialize the general board specific settings
 *
 *  This function initializes the general board specific settings.
 */
void CC1310_LAUNCHXL_initGeneral(void);

/*!
 *  @brief  Turn off the external flash on LaunchPads
 *
 */
void CC1310_LAUNCHXL_shutDownExtFlash(void);

/*!
 *  @def    CC1310_LAUNCHXL_ADCBufName
 *  @brief  Enum of ADCBufs
 */
typedef enum CC1310_LAUNCHXL_ADCBufName {
    CC1310_LAUNCHXL_ADCBUF0 = 0,

    CC1310_LAUNCHXL_ADCBUFCOUNT
} CC1310_LAUNCHXL_ADCBufName;

/*!
 *  @def    CC1310_LAUNCHXL_ADCBuf0SourceName
 *  @brief  Enum of ADCBuf channels
 */
typedef enum CC1310_LAUNCHXL_ADCBuf0ChannelName {
   // CC1310_LAUNCHXL_ADCBUF0CHANNEL0 = 0,
  //  CC1310_LAUNCHXL_ADCBUF0CHANNEL1,
  //  CC1310_LAUNCHXL_ADCBUF0CHANNEL2,
  //  CC1310_LAUNCHXL_ADCBUF0CHANNEL3,
   // CC1310_LAUNCHXL_ADCBUF0CHANNEL4,
  //  CC1310_LAUNCHXL_ADCBUF0CHANNEL5,
   // CC1310_LAUNCHXL_ADCBUF0CHANNEL6,
  // CC1310_LAUNCHXL_ADCBUF0CHANNEL7,
  //  CC1310_LAUNCHXL_ADCBUF0CHANNELVDDS,
  //  CC1310_LAUNCHXL_ADCBUF0CHANNELDCOUPL,
  //  CC1310_LAUNCHXL_ADCBUF0CHANNELVSS,

    CC1310_LAUNCHXL_ADCBUF0CHANNELCOUNT
} CC1310_LAUNCHXL_ADCBuf0ChannelName;

/*!
 *  @def    CC1310_LAUNCHXL_ADCName
 *  @brief  Enum of ADCs
 */
typedef enum CC1310_LAUNCHXL_ADCName {
    CC1310_LAUNCHXL_ADC0 = 0,
    CC1310_LAUNCHXL_ADC1,
    CC1310_LAUNCHXL_ADC2,
    CC1310_LAUNCHXL_ADC3,
   // CC1310_LAUNCHXL_ADC4,
  //  CC1310_LAUNCHXL_ADC5,
  //  CC1310_LAUNCHXL_ADC6,
  //  CC1310_LAUNCHXL_ADC7,
  //  CC1310_LAUNCHXL_ADCDCOUPL,
  //  CC1310_LAUNCHXL_ADCVSS,
  //  CC1310_LAUNCHXL_ADCVDDS,

    CC1310_LAUNCHXL_ADCCOUNT
} CC1310_LAUNCHXL_ADCName;

/*!
 *  @def    CC1310_LAUNCHXL_CryptoName
 *  @brief  Enum of Crypto names
 */
typedef enum CC1310_LAUNCHXL_CryptoName {
    CC1310_LAUNCHXL_CRYPTO0 = 0,

    CC1310_LAUNCHXL_CRYPTOCOUNT
} CC1310_LAUNCHXL_CryptoName;

/*!
 *  @def    CC1310_LAUNCHXL_GPIOName
 *  @brief  Enum of GPIO names
 */
typedef enum CC1310_LAUNCHXL_GPIOName {
   // XL_INT_PIN = 0,

    LED1,
    LED2,
    CC1310_LAUNCHXL_GPIO_SPI_FLASH_CS,
    AXEL_POWER,
    ENABLE_4V,
    GSM_PWR_KEY,
    //ENABLE_3_3V,

    CC1310_LAUNCHXL_GPIOCOUNT
} CC1310_LAUNCHXL_GPIOName;

/*!
 *  @def    CC1310_LAUNCHXL_GPTimerName
 *  @brief  Enum of GPTimer parts
 */
typedef enum CC1310_LAUNCHXL_GPTimerName {
   CC1310_LAUNCHXL_GPTIMER0A = 0,
   //CC1310_LAUNCHXL_GPTIMER0B,
   //CC1310_LAUNCHXL_GPTIMER1A,
  //  CC1310_LAUNCHXL_GPTIMER1B,
  //  CC1310_LAUNCHXL_GPTIMER2A,
  //  CC1310_LAUNCHXL_GPTIMER2B,
  //  CC1310_LAUNCHXL_GPTIMER3A,
   // CC1310_LAUNCHXL_GPTIMER3B,

    CC1310_LAUNCHXL_GPTIMERPARTSCOUNT
} CC1310_LAUNCHXL_GPTimerName;

/*!
 *  @def    CC1310_LAUNCHXL_GPTimers
 *  @brief  Enum of GPTimers
 */
typedef enum CC1310_LAUNCHXL_GPTimers {
    CC1310_LAUNCHXL_GPTIMER0 = 0,
   // CC1310_LAUNCHXL_GPTIMER1,
   // CC1310_LAUNCHXL_GPTIMER2,
   // CC1310_LAUNCHXL_GPTIMER3,

    CC1310_LAUNCHXL_GPTIMERCOUNT
} CC1310_LAUNCHXL_GPTimers;

/*!
 *  @def    CC1310_LAUNCHXL_I2CName
 *  @brief  Enum of I2C names
 */
typedef enum CC1310_LAUNCHXL_I2CName {
    CC1310_LAUNCHXL_I2C0 = 0,

    CC1310_LAUNCHXL_I2CCOUNT
} CC1310_LAUNCHXL_I2CName;

/*!
 *  @def    CC1310_LAUNCHXL_NVSName
 *  @brief  Enum of NVS names
 */
typedef enum CC1310_LAUNCHXL_NVSName {
    CC1310_LAUNCHXL_NVSCC26XX0 = 0,
    CC1310_LAUNCHXL_NVSSPI25X0,

    CC1310_LAUNCHXL_NVSCOUNT
} CC1310_LAUNCHXL_NVSName;

/*!
 *  @def    CC1310_LAUNCHXL_PWM
 *  @brief  Enum of PWM outputs
 */
typedef enum CC1310_LAUNCHXL_PWMName {
   // CC1310_LAUNCHXL_PWM0 = 0,
  //  CC1310_LAUNCHXL_PWM1,
  //  CC1310_LAUNCHXL_PWM2,
  //  CC1310_LAUNCHXL_PWM3,
  //  CC1310_LAUNCHXL_PWM4,
  //  CC1310_LAUNCHXL_PWM5,
 //   CC1310_LAUNCHXL_PWM6,
  //  CC1310_LAUNCHXL_PWM7,

    CC1310_LAUNCHXL_PWMCOUNT
} CC1310_LAUNCHXL_PWMName;

/*!
 *  @def    CC1310_LAUNCHXL_SPIName
 *  @brief  Enum of SPI names
 */
typedef enum CC1310_LAUNCHXL_SPIName {
    CC1310_LAUNCHXL_SPI0 = 0,
   // CC1310_LAUNCHXL_SPI1,

    CC1310_LAUNCHXL_SPICOUNT
} CC1310_LAUNCHXL_SPIName;

/*!
 *  @def    CC1310_LAUNCHXL_UARTName
 *  @brief  Enum of UARTs
 */
typedef enum CC1310_LAUNCHXL_UARTName {
    CC1310_LAUNCHXL_UART0 = 0,
    //CC1310_LAUNCHXL_UART1,
    //CC1310_LAUNCHXL_UART2,

    CC1310_LAUNCHXL_UARTCOUNT
} CC1310_LAUNCHXL_UARTName;

/*!
 *  @def    CC1310_LAUNCHXL_UDMAName
 *  @brief  Enum of DMA buffers
 */
typedef enum CC1310_LAUNCHXL_UDMAName {
    CC1310_LAUNCHXL_UDMA0 = 0,

    CC1310_LAUNCHXL_UDMACOUNT
} CC1310_LAUNCHXL_UDMAName;

/*!
 *  @def    CC1310_LAUNCHXL_WatchdogName
 *  @brief  Enum of Watchdogs
 */
typedef enum CC1310_LAUNCHXL_WatchdogName {
    CC1310_LAUNCHXL_WATCHDOG0 = 0,

    CC1310_LAUNCHXL_WATCHDOGCOUNT
} CC1310_LAUNCHXL_WatchdogName;
