#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>
#include <stdlib.h>
#include <string.h>


LOG_MODULE_REGISTER(pwmleds,LOG_LEVEL_DBG);


#define RLED_NODE DT_ALIAS(rled)

struct led_obj {
    struct pwm_dt_spec spec;
    const char *label;
};

static const struct led_obj rled = {
    .spec = PWM_DT_SPEC_GET(RLED_NODE),
    .label = DT_PROP(RLED_NODE,label),
};


int setPulse(const struct pwm_dt_spec *led ,uint8_t dutycycle,const char *led_name){
    int setting = pwm_set_pulse_dt(led,(led->period * dutycycle)/100);

    if (setting > 0 ){
        LOG_ERR("set duty cycle is failed led : %s , dutycycle : %d",led_name,dutycycle);
        return -1;
    }

    LOG_INF("PULSE SET led : %s , pulse : %d",led_name,dutycycle);

    return 0;
}


static int cmd_led(const struct shell *sh,
                    size_t argc , char **argv)
{
    if (argc < 3){
        shell_print(sh,"args not found Using uart$ --help");
        return -EINVAL;
    }

    const char *led_name = argv[1];
    const char *bright_level_string = argv[2];

    char *endptr;
    uint8_t bright_level_int;

    bright_level_int = strtol(bright_level_string,&endptr,10);


    if (bright_level_int < 0 || bright_level_int > 100){
        shell_print(sh,"brightness value is invalid");
        return -EINVAL;
    }

    
    if (strcmp(led_name,"red")==0){
        setPulse(&rled.spec,bright_level_int,rled.label);
        shell_print(sh,"Led : %s ,brightness Level : %d",led_name,bright_level_int);
    }else{
        shell_print(sh,"led not found !!");
    }
    
    return 0;
}

SHELL_CMD_REGISTER(led,NULL,
                "led a use : led <LED_NAME> <BRIGHTNESS LEVEL>",
                cmd_led);

int main(){

    if(!pwm_is_ready_dt(&rled.spec)){
        LOG_ERR("pwm leds are not ready");
        return -1;
    }

    LOG_INF("Starting leds Three Differnt led using PWM . using --help");

    setPulse(&rled.spec,100,rled.label);
}