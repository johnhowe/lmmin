#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Hardware library includes. */
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_sysctl.h"
#include "sysctl.h"
#include "gpio.h"
#include "utils/rit128x96x4.h"
#include "utils/ustdlib.h"
#include "timer.h"
#include "uart.h"

/* The OLED task uses the sprintf function so requires a little more stack too. */
#define mainOLED_TASK_STACK_SIZE ( configMINIMAL_STACK_SIZE + 50 )
#define ulSSI_FREQUENCY ( 3500000UL )
#define FONT_HEIGHT 8

static void OLEDLeftTask (void *pvParameters);
static void OLEDRightTask (void *pvParameters);
static void OLEDCountTask (void *pvParameters);
static void UARTTask (void *pvParameters);
static void prvSetupHardware (void);

int main (void)
{
    prvSetupHardware ();

    xTaskCreate( OLEDLeftTask, ( signed portCHAR * ) "OLED", mainOLED_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    xTaskCreate( OLEDRightTask, ( signed portCHAR * ) "OLED", mainOLED_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    xTaskCreate( OLEDCountTask, ( signed portCHAR *) "OLED", mainOLED_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    xTaskCreate( UARTTask, ( signed portCHAR *) "UART", mainOLED_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    RIT128x96x4Clear ();

    vTaskStartScheduler ();

    RIT128x96x4Clear ();
    RIT128x96x4StringDraw ("FreeRTOS scheduler", 0, 0, 11);
    RIT128x96x4StringDraw ("failed to start.", 0, FONT_HEIGHT, 11);
    for (;;)
        continue;
}

void prvSetupHardware (void)
{
    /* Set the clocking to run from the PLL at 50 MHz */
    SysCtlClockSet (SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
    RIT128x96x4Init (ulSSI_FREQUENCY);
}

void OLEDLeftTask (void *pvParameters)
{
    static char pucHello [] = "495ms ";

    for (;;)
    {
        taskENTER_CRITICAL();
        RIT128x96x4StringDraw (pucHello, 20, 20, 11);
        taskEXIT_CRITICAL();
        vTaskDelay (495 / portTICK_RATE_MS);

        taskENTER_CRITICAL();
        RIT128x96x4StringDraw (pucHello, 20, 20, ~11);
        taskEXIT_CRITICAL();
        vTaskDelay (495 / portTICK_RATE_MS);
    }
}

void OLEDRightTask (void *pvParameters)
{
    static char pucHello [] = "505ms ";

    for (;;)
    {
        taskENTER_CRITICAL();
        RIT128x96x4StringDraw (pucHello, 80, 30, 11);
        taskEXIT_CRITICAL();
        vTaskDelay (505 / portTICK_RATE_MS);

        taskENTER_CRITICAL();
        RIT128x96x4StringDraw (pucHello, 80, 30, ~11);
        taskEXIT_CRITICAL();
        vTaskDelay (505 / portTICK_RATE_MS);
    }
}

void OLEDCountTask (void *pvParameters)
{
    static portTickType count;
    static char scount [30];
    for (;;)
    {
        count = xTaskGetTickCount ();
        usprintf (scount, "%8d", count);
        taskENTER_CRITICAL();
        RIT128x96x4StringDraw (scount, 70, 88, 11);
        taskEXIT_CRITICAL();
        vTaskDelay ((1 / 999) / portTICK_RATE_MS);
    }
}

void UARTTask (void *pvParameters)
{
    //  initialise the UART
    SysCtlPeripheralEnable (SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeUART (GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk (UART0_BASE, SysCtlClockGet (), 115200,
            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTEnable (UART0_BASE);

    static portTickType ucount;
    static int strCount;
    static char sucount [8];
    for (;;)
    {
        if (UARTSpaceAvail (UART0_BASE))
        {
            ucount = xTaskGetTickCount ();
            usprintf (sucount, "%7d\n", ucount);
            strCount = 0;
            while (strCount < 8)
            {
                UARTCharPut (UART0_BASE, sucount [strCount++]);
            }
        }
        vTaskDelay(100/portTICK_RATE_MS);
    }
}
