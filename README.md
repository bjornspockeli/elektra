Examples and tasks for the Elektra BLE course.
-------

### Presentations
The presentations from the course can be downloaded using the links below:

[Nordic Introduction and Course Agenda](https://drive.google.com/open?id=0B21ni_IYbeTXaUdDeXZmbi1xeG8)

[nRF52 Introduction and BLE Protocol Basics ](https://drive.google.com/open?id=0B21ni_IYbeTXSTlqUjh2T2RpZjQ)

[nRF52 & PWM](https://drive.google.com/open?id=0B21ni_IYbeTXbkZrdjNBRGpuNzQ)

[BLE Security](https://drive.google.com/open?id=0B21ni_IYbeTXUHlRWUZrbGRWRUE)


### Course Software

The course software can be found in this folder on Google Drive:

[Course Software Folder](https://drive.google.com/open?id=0B21ni_IYbeTXQ0t3cFZXRGRrR0E)

Copy the content of the folder to your computer and follow the instructions given in the readme.txt file. 


## Hands-on Tasks - Day 1 

During the first day of the course we will use the nRF52832 as 

The tasks should be performed in the order that they are given and you should use the Template Project found in  nRF5_SDK_12.2.0\examples\peripheral\template_project.

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
    button_config.pin_no        = BUTTON_1;
    button_config.active_state  = APP_BUTTON_ACTIVE_LOW;
    button_config.pull_cfg      = NRF_GPIO_PIN_PULLUP; 
    button_config.button_handler = button_handler;
```
Since there are 4 buttons on the nRF52 DK you have to create an array that contains 4 of these structs, i.e. 

```C
    static app_button_cfg_t button_config[4];
    button_config[0].pin_no        = BUTTON_1 ;
    button_config[0].active_state  = APP_BUTTON_ACTIVE_LOW;
    button_config[0].pull_cfg      = NRF_GPIO_PIN_PULLUP; 
    button_config[0].button_handler = button_handler;
    
    button_config[1].pin_no        = BUTTON_2 ;
    button_config[1].active_state  = APP_BUTTON_ACTIVE_LOW;
    button_config[1].pull_cfg      = NRF_GPIO_PIN_PULLUP; 
    button_config[1].button_handler = button_handler;
   ...
```

After creating the app_button_cfg_t button_config[4] array, you have to call [app_button_init](http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v12.2.0/group__app__button.html#ga5948c3d8e3e385820dc7d8df0976e8eb) and [app_button_enable](http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v12.2.0/group__app__button.html#gab498b436a38ebb03393e1129a6daffc9)
in that order.

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
The APP_UART_DATA_READY event will be generated for each single byte that is received by the nRF52, which means that [app_uart_get](http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v12.2.0/group__app__uart.html#gacddb5b7b711ef104f9eb181a13bc4503) must be called everytime the event is received. Since the [app_uart_get](http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v12.2.0/group__app__uart.html#gacddb5b7b711ef104f9eb181a13bc4503) function takes the pointer to a uint8_t, we need an array to store the received bytes and and index variable to keep track of how many bytes we have received, i.e.
```C
    static uint8_t data_array[32];
    static uint8_t index = 0;
```
Most terminals append the `\n` character, also known as the Line Feed character, to the end of the string that is sent. The `\n`  indicates that the next character should be printed on a newline. Therefore it makes sense to receive bytes until we see the `\n` character and then send the entire string back to the terminal using [app_uart_put](http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v12.2.0/group__app__uart.html#ga2e4c8407274a151e72ed5a226529dc36). 

```C
  if (data_array[index - 1] == '\n') 
  {
    // Call app_uart_put to sent the bytes stored in data_array back to the terminal.
  }
```

Hint: The function app_uart_put used to place data in the UART's transmit buffer must be called in a for-loop if more that one byte is to be sent, i.e. 
```C
    for (uint32_t i = 0; i < strlen((const char *)data_array); i++)
    {
        while (app_uart_put(data_array[i]) != NRF_SUCCESS);
    }
```

##Task 4: Temperature Sensor (Optional)
**Scope:** Use the die temperature sensor on the nRF52 to measure the temperature in the room. 

1. Create the function read_temperature() that returns the die temperature as a int32_t.
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

##Task 7: Control LEDs using the nRF Toolbox App
**Scope:** Modify the ble_app_uart example to recognise specific commands sent from the nRF Toolbox app and turn on a LED when one of these commands are received.

1. Open the ble_app_uart example found in the nRF5_SDK_12.2.0\examples\ble_peripheral\ble_app_uart\pca10040\s132\arm5_no_packs folder.

2. We need a variable to keep track of the current command that the nRF52 should handle. We can do this by creating an enumeration, which is basically a list of commands that are assigned a number from 0 and upwards. We create an enumeration  like this
```C    
    typedef enum {
        COMMAND_1,
        COMMAND_2,
        COMMAND_3,
        NO_COMMAND
    } uart_command_t;
```
Every variable of the uart_command_t type can be set to one of the commands in the list.  We've  added a `NO_COMMAND` command which is going to be the default state when no command has been received or the last command has been completed. After declaring the enumeration type `uart_command_t` we need to create a variable `m_command` of the `uart_command_t` type and initialize it to `NO_COMMAND`, i.e.

```C  
    uart_command_t m_command = NO_COMMAND;
```

3. Find the function `nus_data_handler`. This function is called when data is sent to the Nordic UART Service(NUS) from the nRF Toolbox app and this is where we have to look for the specific commands. The data that has been received is stored in a array pointed to by the `p_data` pointer and we need to store it a local array for later use. This can be done by using the [memcpy](https://www.tutorialspoint.com/c_standard_library/c_function_memcpy.htm) function. It will copy the content from cell 0 to `length` in the array pointed to by p_data into the uart_string.      

```C  
    static char uart_string[BLE_NUS_MAX_DATA_LEN];
    memcpy( uart_string, p_data, length);
```

4. Now that we have copied the received data into the `uart_string` array we want to compare the content of `uart_string` with a known command. This can be done by using the [strcmp](https://www.tutorialspoint.com/c_standard_library/c_function_strcmp.htm) function, which will return 0 if `uart_string` is equal to the 

```C  
    if(strcmp(uart_string,"COMMAND_1") == 0 )
    {
        m_command = COMMAND_1;    
    }
    else if(!strcmp(uart_string,"COMMAND_2") == 0 )
    {
        m_command = COMMAND_2;
    }
    else if(!strcmp(uart_string,"COMMAND_3") == 0)
    {
        m_command = COMMAND_3;
    }
    else
    {
        m_command = NO_COMMAND;
    }
```
If the uart_string that we received is equal to the known "COMMAND_1" string, then we set the `m_command` variable to the corresponding command in enumeration we created in step 1.   

5. Now that the `m_command` variable is set to the correct command if the correct string is received, the last thing we need is a function that checks these commands at a regular interval and runs the code we have assosiated with that command. We'll call this function `uart_command_handler` and it takes the pointer to the m_command variable as an input. Inside the function we have a [switch](https://www.tutorialspoint.com/cprogramming/switch_statement_in_c.htm) statement, which is very useful when comparing a variable against a list of values, like our `uart_command_t` enumeration. The `uart_command_handler` should look something like this

```C  
    void uart_command_handler(uart_command_t * m_command)
    {
        uint32_t err_code = NRF_SUCCESS;

        switch(*m_command)
        {
            case COMMAND_1:
                // Put Action to COMMAND_1 here.
                break;

            case COMMAND_2:
                // Put Action to COMMAND_2 here.
                break;

            case COMMAND_3:
                // Put Action to COMMAND_3 here.
                break;

            case NO_COMMAND:
                // No command has been received -> Do nothing
                break;
                
            default:
                // Invalid command -> Do nothing.
                break;
        }
        /* Reset the command variable to NO_COMMAND after a command has been handled */
        *m_command = NO_COMMAND;

        // Check for errors 
        APP_ERROR_CHECK(err_code);
    }
```
After declaring the `uart_command_handler` we add the uart_command_handler to the infinite for-loop in main as shown below. 
```C 
    for (;;)
        {
            uart_command_handler(&m_command);
            power_manage();
        }
```
6. Now we want to toggle a led when we receive "COMMAND_1". Since the ble_app_uart example uses LED_1 on the nRF52 DK to indicate if the device is advertising or connected to central, so we have to toggle LED_4 instead. 
```C 
    case COMMAND_1:
        nrf_gpio_pin_toogle(LED_4) 
        break;
```
We also have to configure the pin connected to LED_4 as an output so make sure that you add to main()
```C 
    nrf_gpio_cfg_output(LED_4); 
```

7. Compile the project and flash it to the nRF52 DK. Make sure that you've also flashed the S132 v3.0 SoftDevice to your board. LED 1 on the nRF52 DK should start blinking, indicating that its advertising.  We've now completed the configuration on the nRF52 side  

8. Install the nRF Toolbox app on you Android/iOS phone. You can find the app [here](https://www.nordicsemi.com/eng/Products/Nordic-mobile-Apps/nRF-Toolbox-App) on Google Play Store and [here](https://itunes.apple.com/us/app/nrf-toolbox/id820906058?mt=8) on Apple App Store. Open the nRF Toolbox app and click the UART symbol.  


<!--- ![Image of nRF Toolbox](https://github.com/bjornspockeli/elektra/blob/master/images/nrf_toolbox.png | width=100) --->
<img src="https://github.com/bjornspockeli/elektra/blob/master/images/nrf_toolbox.png" width="250">

##Task 8: Control the Servo using the nRF Toolbox App
**Scope:** 

##Task 9: Control the Servo using the nRF Toolbox App
**Scope:** 
