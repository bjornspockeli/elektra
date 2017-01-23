Examples and tasks for the Elektra BLE course.
-------

### Presentations
The presentations from the course can be downloaded using the links below:

[nRF52 Introduction and BLE Protocol Basics ](https://drive.google.com/open?id=0B21ni_IYbeTXSTlqUjh2T2RpZjQ)

[Nordic INtroduction and Course Agenda](https://drive.google.com/open?id=0B21ni_IYbeTXaUdDeXZmbi1xeG8)

[nRF52 & PWM](https://drive.google.com/open?id=0B21ni_IYbeTXbkZrdjNBRGpuNzQ)

[BLE Security](https://drive.google.com/open?id=0B21ni_IYbeTXUHlRWUZrbGRWRUE)


### Course Software

The course software can be found in this folder on Google Drive:

[Course Software Folder](https://drive.google.com/open?id=0B21ni_IYbeTXQ0t3cFZXRGRrR0E)

Copy the content of the folder to your computer and follow the instructions given in the readme.txt file. 


## Hands-on Tasks - Day 1 

The hands-on tasks for the first course day will cover the use of the application timer library, the button handler library and the UART library. If you have some time left after finsihing the 3 first taske, then you may try to solve the optional Temperature Sensor task.

If you have any compilation problems when using the function calls referred to in the task description then make sure that you enable the the correct modules in the skd_config.c file.

##Task 1: Application Timer
**Scope:** Use an application timer to toggle one (or several) Leds on the nRF52 DK at a given interval. 

1. Follow the [Application Timer Tutorial](https://devzone.nordicsemi.com/tutorials/19/) on DevZone to create a repeated timer.
2. Create the functions `start_timer` and `stop_timer` that will be used to start and stop the application timer.  
2. Turn on or toggle a led in the app_timer_timeout_handler using [nrf_gpio_pin_clear](http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v12.2.0/group__nrf__gpio.html#ga5c671adfb6b44f32c9d99d3156aff2b1) or [nrf_gpio_pin_toggle](http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v12.2.0/group__nrf__gpio.html#gac7f7bf539f5bb053b4a313ec51d8157e).

Note: The LEDs on the nRF52 are active low.

##Task 2: Button Handler
**Scope:** Use the buttons on the nRF52 and the button handler library(app_button) to start and stop the application timer from Task 1. The button handler library is documented on [this](http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v12.2.0/group__app__button.html?resultof=%22%62%75%74%74%6f%6e%22%20%22%68%61%6e%64%6c%65%72%22%20) Infocenter page. 

1. Create a function called `button_init()`, where you configure the buttons you want to start and stop the application timer and enable the button handler module. Hint: The pins connected to the buttons will be short to ground when the buttons are pressed and should therefore be pulled to VDD when not pressed. A button is configured using a app_button_cfg_t struct as shown below

```C
    static app_button_cfg_t button_config;
    button_config.pin_no        = 1;
    button_config.active_state  = APP_BUTTON_ACTIVE_LOW;
    button_config.pull_cfg      = NRF_GPIO_PIN_PULLUP; 
    button_config.button_handler = button_handler;
```
Since there are 4 buttons on the nRF52 DK you have to create an array that contains 4 of these structs, i.e. 

```C
   static app_button_cfg_t button_config[4]
   button_config[0].pin_no        = 1;
   ...
```

2. Create the callback function `button_handler(uint8_t pin_no, uint8_t button_action)` that will be called whenever a button is pressed or released. In this function you can check which button that was pressed and which action caused the callback, i.e. 

```C
   static void button_handler(uint8_t pin_no, uint8_t button_action)
   {
       if(pin_no == BUTTON_4 && button_action == APP_BUTTON_RELEASE)
       {
           \\ Do something if button 4 is released. 
       }
   }
```
3. Call the `start_timer`function from task 1 when Button 1 is pressed and `stop_timer`when button 2 is pressed. 



##Task 3: UART
**Scope:** Use the nRF52's UART peripheral and the UART library (app_uart) to echo data sent from a terminal. If you do not already have a favorite terminal application, then I recommend using [Termite] (http://www.compuphase.com/software_termite.htm). The UART library is documented on [this](http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v12.2.0/group__app__uart.html?resultof=%22%41%50%50%5f%55%41%52%54%5f%46%49%46%4f%5f%49%4e%49%54%22%20) Infocenter page. 

1. Create the function uart_init where you use the APP_UART_FIFO_INIT macro to initialize the UART module.
   The baudrate should be set to 115200, Flow Control should be disabled, no parity bits are used and the RX and TX buffers should be      set to 256 in size. The UART pins of the nRF52 DK are listed on the backside of the board. See the UART example in the              \examples\peripheral\uart\pca10040\blank\arm5_no_packs folder
2. Create the function uart_event_handler as shown below and add code that echoes the received data. 

```C
    void uart_event_handler(app_uart_evt_t * p_event)
    {
        /*
        You're not allowed to decleare variables inside switch-cases, 
        so any variables used in the switch-case must be declared here.
        */
        switch (p_event->evt_type)
        {
            case APP_UART_DATA_READY:
                /*  
                The received data is stored in a receive buffer and can be retrieved using app_uart_get.
                Data to be sent can be placed in the transmit buffer using app_uart_put.
                */
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
```
The APP_UART_DATA_READY event will be generated for each single byte that is received by the nRF52, which 
Most terminals append the `\n` character, also known as the Line Feed character, which indicates that the next character should be printed on a newline. 

Hint: The function app_uart_put is used to place data in the UART's transmit buffer and must be called in a for-loop if more that one byte is to be sent, i.e. 
```C
    for (uint32_t i = 0; i < strlen((const char *)data); i++)
    {
        while (app_uart_put(data[i]) != NRF_SUCCESS);
    }
```

##Task 4: Temperature Sensor (Optional)
**Scope:** Use the die temperature sensor on the nRF52 to measure the temperature in the room. 
1 . Create the function read_temperature() that returns the die temperature as a int32_t.
Hint: Take a look at the temperature example in the SDK before you start modifying your template example.
2. Send the temperature data to your terminal application using the UART. 

Hint 1: The function app_uart_put is used to place data in the UART's transmit buffer and must be called in a for-loop if more that one byte is to be sent, i.e. 
```C
    for (uint32_t i = 0; i < strlen((const char *)data); i++)
    {
        while (app_uart_put(data[i]) != NRF_SUCCESS);
    }
```
Hint 2: Use [sprintf](https://www.tutorialspoint.com/c_standard_library/c_function_sprintf.htm) to copy the content of a string into an array.

## Hands-on Tasks - DAY 2

##Task 5: PWM Sweep
**Scope:** In this task we will use the PWM library in the nRF5x SDK to control a servo.

Connecting the Servo to your nRF52 DK:

The three wires coming from the SG92R Servo are:

Brown: 	Ground 				- Should be connected to one of the pins marked GND on your nRF52 DK.

Red: 	5V 					- Should be connected to the pin marked 5V on your nRF52 DK.

Orange: PWM Control Signal 	- Should be connected to one of the unused GPIO pins of the nRF52 DK (for example P0.11, pin number 11).


1.	Create the function pwm_init() where you initialize and enable the PWM peripheral.
  * Hint: Use the pwm_library example in the SDK as a reference, you will find it in nRF5x_SDK_12.2\examples\peripheral\pwm_library\pca10040\blank\arm5_no_packs.
2.	Make sure that the correct nRF_Drivers and nRF_Libraries are enabled in the sdk_config.h file. 
  * Hint: Select the Configuration Wizard Tab in the bottom of the text window after opening sdk_config.h in the ble_app_uart example and compare it to the one in the pwm_library example.
3. Call app_pwm_channel_duty_set() to set the duty cycle 

Tips:
* Period should be 20ms (20000us) and duty cycle should vary between 1-2ms (5-10%).
* If the polarity is ‘APP_PWM_POLARITY_ACTIVE_HIGH’, the duty cycle set will tell how long the pulse is high.
* app_pwm_channel_duty_set(&PWM1, 0, value) will set the duty cycle of channel 0 to ‘value’ percent.
* while (app_pwm_channel_duty_set(&PWM1, 0, value) == NRF_ERROR_BUSY) makes sure that the code does not continue before the duty cycle is updated.

##Task 6: PWM & Buttons
**Scope:** Modify the button handler from task 1 so that the servo is placed at its minimum angle angle by pressing button 3 and its maximum angle by pressing button 4.  
