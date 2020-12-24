#ifndef HARDWARE_ADC_H
#define HARDWARE_ADC_H

typedef enum
{
    ADC_PORT_0,
    ADC_PORT_1,
    ADC_PORT_MAX
}adc_port_e;

struct adc_config
{

};

struct adc_device
{
    adc_port_e adc_id;
    struct adc_config config;

    void (*init)(struct adc_device *adc);
    void (*convert)(struct adc_device *adc,unsigned int channel,unsigned int *data);
    unsigned long (*read)(struct adc_device *adc,void *data,unsigned long len);
    void (*enable)(struct adc_device *adc,unsigned char states);

};

void hardware_adc_init(void);
#endif // HARDWARE_ADC_H
