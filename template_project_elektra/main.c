/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
* @brief Example template project.
* @defgroup nrf_templates_example Example Template
*
*/

#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"
#include "nrf_gpio.h"
#include "nordic_common.h"
#include "nrf_delay.h"
#include "app_error.h"
#include "app_timer.h"
#include "app_util_platform.h"
#include "boards.h"
//#include "bsp.h"          Remove as is interfers with the app_button library.
#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "app_button.h"
#include "app_pwm.h"
#include "app_uart.h"
#include "nrf_temp.h"

// General application timer settings.
#define APP_TIMER_PRESCALER             15      // Value of the RTC1 PRESCALER register.
#define APP_TIMER_OP_QUEUE_SIZE         3       // Size of timer operation queues.

#define UART_TX_BUF_SIZE                256     /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256     /**< UART RX buffer size. */

APP_TIMER_DEF(m_led_a_timer_id);                // Create 
APP_PWM_INSTANCE(PWM1,1);                       // Create the instance "PWM1" using TIMER1.

//static volatile bool ready_flag;                // A flag indicating PWM status.

// Function starting the internal LFCLK oscillator.
// This is needed by RTC1 which is used by the application timer
// (When SoftDevice is enabled the LFCLK is always running and this is not needed).
static void lfclk_request(void)
{
    uint32_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);
}

static int32_t read_temperature()
{
    NRF_TEMP->TASKS_START = 1; /** Start the temperature measurement. */
    
    while (NRF_TEMP->EVENTS_DATARDY == 0)
    {
        // Do nothing.
    }
    NRF_TEMP->EVENTS_DATARDY = 0;
    
    NRF_TEMP->TASKS_STOP = 1; /** Stop the temperature measurement. */
    
    return (nrf_temp_read() / 4);
}


// Timeout handler for the timer
static void timer_handler(void * p_context)
{
    // Toggle led 1
    nrf_gpio_pin_toggle(LED_1);
    
    // Read the temperature and send to the terminal using the UART.
    volatile int temp;
    uint8_t data[32];
    temp = read_temperature();
    sprintf((char *)data, "Temperature: %d \n\r", temp);
    
    // Print temperature measurement on the UART
    for (uint32_t i = 0; i < strlen((const char *)data); i++)
    {
        while (app_uart_put(data[i]) != NRF_SUCCESS);
    }

}


static void create_timer()
{   
    uint32_t err_code;

    // Create timers
    err_code = app_timer_create(&m_led_a_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                timer_handler);
    APP_ERROR_CHECK(err_code);
}

static void start_timer()
{
    uint32_t err_code;

    // Start timer
    err_code = app_timer_start(m_led_a_timer_id,
                                APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER),
                                NULL);
    APP_ERROR_CHECK(err_code);
}

static void stop_timer()
{
     uint32_t err_code;

    // Stop timer
    err_code = app_timer_stop(m_led_a_timer_id);
    APP_ERROR_CHECK(err_code);
}





/** UART TASK CODE START */

void uart_event_handler(app_uart_evt_t * p_event)
{
    static uint8_t data_array[32];
    static uint8_t index = 0;

    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY:
            app_uart_get(&data_array[index]);
            index++;

            if (data_array[index - 1] == '\n') 
            {
                for (uint32_t i = 0; i < strlen((const char *)data_array); i++)
                {
                    while (app_uart_put(data_array[i]) != NRF_SUCCESS);
                }
                memset(data_array,0,sizeof(data_array));
                index = 0;
            }
            break;

        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}

static void uart_init(void)
{
    uint32_t err_code;
    const app_uart_comm_params_t comm_params =
    {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_DISABLED,
        false,
        UART_BAUDRATE_BAUDRATE_Baud115200
    };

    APP_UART_FIFO_INIT( &comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handler,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
    APP_ERROR_CHECK(err_code);
}

/** UART TASK CODE END */

static void button_handler(uint8_t pin_no, uint8_t button_action)
{
    if(pin_no == BUTTON_1 && button_action == APP_BUTTON_PUSH)
    {
        start_timer();
    }
    if(pin_no == BUTTON_2 && button_action == APP_BUTTON_PUSH)
    {
        stop_timer();
    }
    if(pin_no == BUTTON_3 && button_action == APP_BUTTON_PUSH)
    {
        while (app_pwm_channel_duty_set(&PWM1, 0, 5) == NRF_ERROR_BUSY);
    }
    if(pin_no == BUTTON_4 && button_action == APP_BUTTON_PUSH)
    {
        while (app_pwm_channel_duty_set(&PWM1, 0, 10) == NRF_ERROR_BUSY);
    }
}

static void button_init()
{
    uint32_t err_code;
    
    static app_button_cfg_t button_config[4];
    button_config[0].pin_no        = BUTTON_1;
    button_config[0].active_state  = APP_BUTTON_ACTIVE_LOW;
    button_config[0].pull_cfg      = NRF_GPIO_PIN_PULLUP; 
    button_config[0].button_handler = button_handler;
    
    button_config[1].pin_no        = BUTTON_2;
    button_config[1].active_state  = APP_BUTTON_ACTIVE_LOW;
    button_config[1].pull_cfg      = NRF_GPIO_PIN_PULLUP; 
    button_config[1].button_handler = button_handler;
    
    button_config[2].pin_no        = BUTTON_3;
    button_config[2].active_state  = APP_BUTTON_ACTIVE_LOW;
    button_config[2].pull_cfg      = NRF_GPIO_PIN_PULLUP; 
    button_config[2].button_handler = button_handler;
    
    button_config[3].pin_no        = BUTTON_4;
    button_config[3].active_state  = APP_BUTTON_ACTIVE_LOW;
    button_config[3].pull_cfg      = NRF_GPIO_PIN_PULLUP; 
    button_config[3].button_handler = button_handler;
    
    err_code = app_button_init(button_config, 4, 10);
    APP_ERROR_CHECK(err_code);
    
    err_code = app_button_enable();
    APP_ERROR_CHECK(err_code);
}
/*
void pwm_ready_callback(uint32_t pwm_id)    // PWM callback function
{
    ready_flag = true;
}
*/
static void pwm_init()
{
    //uint32_t pins[APP_PWM_CHANNELS_PER_INSTANCE] = {4, APP_PWM_NOPIN};
    //app_pwm_polarity_t pin_polarity[APP_PWM_CHANNELS_PER_INSTANCE] = {APP_PWM_POLARITY_ACTIVE_HIGH,APP_PWM_POLARITY_ACTIVE_HIGH};
    
    app_pwm_config_t pwm_config = {
        .pins               = {4, APP_PWM_NOPIN},
        .pin_polarity       = {APP_PWM_POLARITY_ACTIVE_HIGH, APP_PWM_POLARITY_ACTIVE_LOW}, 
        .num_of_channels    = 1,                                                          
        .period_us          = 20000L                                                
    };
    
    //pwm_config.pin_polarity[0]  = APP_PWM_POLARITY_ACTIVE_HIGH;

    /* Initialize and enable PWM. */
    uint32_t err_code;
    err_code = app_pwm_init(&PWM1,&pwm_config,NULL);
    APP_ERROR_CHECK(err_code);
    
    app_pwm_enable(&PWM1);
    
    //ready_flag = true;
    //while (!ready_flag);
    //APP_ERROR_CHECK(app_pwm_channel_duty_set(&PWM1, 0, 10));
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
/** APP TIMER TASK CODE START */
    
    // Start the LFCLK
    lfclk_request();
    
    // Set LED_1 pin as output
    nrf_gpio_cfg_output(LED_1);
    nrf_gpio_pin_clear(LED_1);
    
    // Initialize the application timer module.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
    
    create_timer();
    start_timer();
    
 /** APP TIMER TASK CODE END */
    
/** BUTTON HANDLER TASK CODE START */   
    button_init();

/** BUTTON HANDLER TASK CODE END */
   
/** PWM TASK CODE START */
    pwm_init();
/** PWM TASK CODE END */

/** UART TASK CODE START */
    uart_init();
/** UART TASK CODE END */

/** TEMP TASK CODE START */
    nrf_temp_init();
    
    volatile int temp;
    uint8_t data[32];
    temp = read_temperature();
    sprintf((char *)data, "Temperature: %d \n\r", temp);
    
    // Print temperature measurement on the UART
    for (uint32_t i = 0; i < strlen((const char *)data); i++)
    {
        while (app_uart_put(data[i]) != NRF_SUCCESS);
    }
    
    // Printf requires retarget.c to be added and RETARGET_ENABLED must be added to the sdk_config.h file
    //printf("Temperature: %d \n\r", (int)temp);
    
/** TEMP TASK CODE END */

    while (true)
    {
        while (app_pwm_channel_duty_set(&PWM1, 0, 5) == NRF_ERROR_BUSY);
        nrf_delay_ms(1000);
        while (app_pwm_channel_duty_set(&PWM1, 0, 10) == NRF_ERROR_BUSY); // Do nothing.
    }
}
/** @} */
