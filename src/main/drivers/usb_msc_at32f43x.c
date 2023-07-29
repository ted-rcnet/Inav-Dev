/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Cleanflight and Betaflight are distributed in the hope that they
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Author: Chris Hockuba (https://github.com/conkerkh)
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "platform.h"

#if defined(USE_USB_MSC)

#include "build/build_config.h"

#include "common/utils.h"

#include "blackbox/blackbox.h"
#include "blackbox/blackbox_io.h"

#include "drivers/io.h"
#include "drivers/light_led.h"
#include "drivers/nvic.h"
#include "drivers/persistent.h"
#include "drivers/sdcard/sdmmc_sdio.h"
#include "drivers/time.h"
#include "drivers/usb_msc.h"

#include "usb_conf.h"
#include "usb_core.h"
#include "usbd_int.h"
#include "msc_class.h"
#include "msc_desc.h"
#include "usb_io.h"

#define DEBOUNCE_TIME_MS 20

extern otg_core_type otg_core_struct;


#if defined(MSC_USE_BUTTON)
static IO_t mscButton;
#endif

void mscInit(void)
{
#if defined(MSC_USE_BUTTON)
    if (usbDevConfig()->mscButtonPin) {
        mscButton = IOGetByTag(usbDevConfig()->mscButtonPin);
        IOInit(mscButton, OWNER_USB_MSC_PIN, 0);
        if (usbDevConfig()->mscButtonUsePullup) {
            IOConfigGPIO(mscButton, IOCFG_IPU);
        } else {
            IOConfigGPIO(mscButton, IOCFG_IPD);
        }
    }
#endif
}


/**
  * @brief  this function config gpio.
  * @param  none
  * @retval none
  */
void msc_usb_gpio_config(void)
{
  gpio_init_type gpio_init_struct;

  crm_periph_clock_enable(OTG_PIN_GPIO_CLOCK, TRUE);
  gpio_default_para_init(&gpio_init_struct);

  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;

  /* dp and dm */
  gpio_init_struct.gpio_pins = OTG_PIN_DP | OTG_PIN_DM;
  gpio_init(OTG_PIN_GPIO, &gpio_init_struct);

  gpio_pin_mux_config(OTG_PIN_GPIO, OTG_PIN_DP_SOURCE, OTG_PIN_MUX);
  gpio_pin_mux_config(OTG_PIN_GPIO, OTG_PIN_DM_SOURCE, OTG_PIN_MUX);

#ifdef USB_SOF_OUTPUT_ENABLE
  crm_periph_clock_enable(OTG_PIN_SOF_GPIO_CLOCK, TRUE);
  gpio_init_struct.gpio_pins = OTG_PIN_SOF;
  gpio_init(OTG_PIN_SOF_GPIO, &gpio_init_struct);
  gpio_pin_mux_config(OTG_PIN_GPIO, OTG_PIN_SOF_SOURCE, OTG_PIN_MUX);
#endif

  /* otgfs use vbus pin */
#ifndef USB_VBUS_IGNORE
  gpio_init_struct.gpio_pins = OTG_PIN_VBUS;
  gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
  gpio_pin_mux_config(OTG_PIN_GPIO, OTG_PIN_VBUS_SOURCE, OTG_PIN_MUX);
  gpio_init(OTG_PIN_GPIO, &gpio_init_struct);
#endif


}

/**
  * @brief  usb 48M clock select
  * @param  clk_s:USB_CLK_HICK, USB_CLK_HEXT
  * @retval none
  */
void msc_usb_clock48m_select(usb_clk48_s clk_s)
{
  if(clk_s == USB_CLK_HICK)
  {
    crm_usb_clock_source_select(CRM_USB_CLOCK_SOURCE_HICK);

    /* enable the acc calibration ready interrupt */
    crm_periph_clock_enable(CRM_ACC_PERIPH_CLOCK, TRUE);

    /* update the c1\c2\c3 value */
    acc_write_c1(7980);
    acc_write_c2(8000);
    acc_write_c3(8020);
#if (USB_ID == 0)
    acc_sof_select(ACC_SOF_OTG1);
#else
    acc_sof_select(ACC_SOF_OTG2);
#endif
    /* open acc calibration */
    acc_calibration_mode_enable(ACC_CAL_HICKTRIM, TRUE);
  }
  else
  {
    switch(system_core_clock)
    {
      /* 48MHz */
      case 48000000:
        crm_usb_clock_div_set(CRM_USB_DIV_1);
        break;

      /* 72MHz */
      case 72000000:
        crm_usb_clock_div_set(CRM_USB_DIV_1_5);
        break;

      /* 96MHz */
      case 96000000:
        crm_usb_clock_div_set(CRM_USB_DIV_2);
        break;

      /* 120MHz */
      case 120000000:
        crm_usb_clock_div_set(CRM_USB_DIV_2_5);
        break;

      /* 144MHz */
      case 144000000:
        crm_usb_clock_div_set(CRM_USB_DIV_3);
        break;

      /* 168MHz */
      case 168000000:
        crm_usb_clock_div_set(CRM_USB_DIV_3_5);
        break;

      /* 192MHz */
      case 192000000:
        crm_usb_clock_div_set(CRM_USB_DIV_4);
        break;

      /* 216MHz */
      case 216000000:
        crm_usb_clock_div_set(CRM_USB_DIV_4_5);
        break;

      /* 240MHz */
      case 240000000:
        crm_usb_clock_div_set(CRM_USB_DIV_5);
        break;

      /* 264MHz */
      case 264000000:
        crm_usb_clock_div_set(CRM_USB_DIV_5_5);
        break;

      /* 288MHz */
      case 288000000:
        crm_usb_clock_div_set(CRM_USB_DIV_6);
        break;

      default:
        break;

    }
  }
}


uint8_t mscStart(void)
{
    ledInit(false);

    // Start USB
    usbGenerateDisconnectPulse();

    IOInit(IOGetByTag(IO_TAG(PA11)), OWNER_USB, 0, 0);
    IOInit(IOGetByTag(IO_TAG(PA12)), OWNER_USB, 0, 0);

    // The SD card is not supported 
    /* switch (blackboxConfig()->device) {
    #ifdef USE_SDCARD
        case BLACKBOX_DEVICE_SDCARD:
            USBD_STORAGE_fops = &USBD_MSC_MICRO_SDIO_fops;
            break;
    #endif

    #ifdef USE_FLASHFS
        case BLACKBOX_DEVICE_FLASH:
            USBD_STORAGE_fops = &USBD_MSC_EMFAT_fops;
            break;
    #endif
        default:
            return 1;
        }
    */

    /* init usb */
    /* usb gpio config */
     msc_usb_gpio_config();

     /* enable otgfs clock */
     crm_periph_clock_enable(OTG_CLOCK, TRUE);

     /* select usb 48m clcok source */
     msc_usb_clock48m_select(USB_CLK_HEXT);

     /* enable otgfs irq,Priority NVIC_PRIO_USB must be used */
     nvic_irq_enable(OTG_IRQ, NVIC_PRIO_USB, 0);

     usbd_init(&otg_core_struct,
              USB_FULL_SPEED_CORE_ID,
              USB_ID,
              &msc_class_handler,
              &msc_desc_handler);

    // NVIC configuration for SYSTick
    nvic_irq_disable(SysTick_IRQn);

    nvic_irq_enable(SysTick_IRQn, 0, 0);
    return 0;
}

bool mscCheckButton(void)
{
    bool result = false;
#if defined(MSC_USE_BUTTON)
    if (mscButton) {
        uint8_t state = IORead(mscButton);
        if (usbDevConfig()->mscButtonUsePullup) {
            result = state == 0;
        } else {
            result = state == 1;
        }
    }
#endif
    return result;
}

void mscWaitForButton(void)
{
    // In order to exit MSC mode simply disconnect the board, or push the button again.
    while (mscCheckButton());
    delay(DEBOUNCE_TIME_MS);
    while (true) {
        asm("NOP");
        if (mscCheckButton()) {
            delay(1);
            NVIC_SystemReset();
        }
    }
}
#endif
