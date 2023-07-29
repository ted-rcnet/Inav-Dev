/*
 * This file is part of Cleanflight.
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdint.h>

#include <platform.h>

#include "drivers/bus_spi.h"
#include "dma.h"
#include "drivers/io.h"
#include "io_impl.h"
#include "drivers/nvic.h"
#include "rcc.h"

#ifndef SPI1_SCK_PIN
#define SPI1_NSS_PIN    PA4
#define SPI1_SCK_PIN    PA5
#define SPI1_MISO_PIN   PA6
#define SPI1_MOSI_PIN   PA7
#endif

#ifndef SPI2_SCK_PIN
#define SPI2_NSS_PIN    PB12
#define SPI2_SCK_PIN    PB13
#define SPI2_MISO_PIN   PB14
#define SPI2_MOSI_PIN   PB15
#endif

#ifndef SPI3_SCK_PIN
#define SPI3_NSS_PIN    PA15
#define SPI3_SCK_PIN    PB3
#define SPI3_MISO_PIN   PB4
#define SPI3_MOSI_PIN   PB5
#endif

#ifndef SPI4_SCK_PIN
#define SPI4_NSS_PIN    PA15
#define SPI4_SCK_PIN    PB3
#define SPI4_MISO_PIN   PB4
#define SPI4_MOSI_PIN   PB5
#endif

#ifndef SPI1_NSS_PIN
#define SPI1_NSS_PIN NONE
#endif
#ifndef SPI2_NSS_PIN
#define SPI2_NSS_PIN NONE
#endif
#ifndef SPI3_NSS_PIN
#define SPI3_NSS_PIN NONE
#endif
#ifndef SPI4_NSS_PIN
#define SPI4_NSS_PIN NONE
#endif

#if defined(USE_SPI_DEVICE_1)
static const uint32_t spiDivisorMapFast[] = {
    LL_SPI_BAUDRATEPRESCALER_DIV256,    // SPI_CLOCK_INITIALIZATON      421.875 KBits/s
    LL_SPI_BAUDRATEPRESCALER_DIV32,     // SPI_CLOCK_SLOW               843.75 KBits/s
    LL_SPI_BAUDRATEPRESCALER_DIV16,     // SPI_CLOCK_STANDARD           6.75 MBits/s
    LL_SPI_BAUDRATEPRESCALER_DIV8,      // SPI_CLOCK_FAST               13.5 MBits/s
    LL_SPI_BAUDRATEPRESCALER_DIV4       // SPI_CLOCK_ULTRAFAST          27.0 MBits/s
};
#endif

#if defined(USE_SPI_DEVICE_2) || defined(USE_SPI_DEVICE_3) || defined(USE_SPI_DEVICE_4)
static const uint32_t spiDivisorMapSlow[] = {
    LL_SPI_BAUDRATEPRESCALER_DIV256,    // SPI_CLOCK_INITIALIZATON      210.937 KBits/s
    LL_SPI_BAUDRATEPRESCALER_DIV64,     // SPI_CLOCK_SLOW               843.75 KBits/s
    LL_SPI_BAUDRATEPRESCALER_DIV8,      // SPI_CLOCK_STANDARD           6.75 MBits/s
    LL_SPI_BAUDRATEPRESCALER_DIV4,      // SPI_CLOCK_FAST               13.5 MBits/s
    LL_SPI_BAUDRATEPRESCALER_DIV2       // SPI_CLOCK_ULTRAFAST          27.0 MBits/s
};
#endif

#if defined(STM32H7)
static spiDevice_t spiHardwareMap[SPIDEV_COUNT] = {
#ifdef USE_SPI_DEVICE_1
#if defined(SPI1_SCK_AF) || defined(SPI1_MISO_AF) || defined(SPI1_MOSI_AF)
#if !defined(SPI1_SCK_AF) || !defined(SPI1_MISO_AF) || !defined(SPI1_MOSI_AF)
#error SPI1: SCK, MISO and MOSI AFs should be defined together in target.h!
#endif
    { .dev = SPI1, .nss = IO_TAG(SPI1_NSS_PIN), .sck = IO_TAG(SPI1_SCK_PIN), .miso = IO_TAG(SPI1_MISO_PIN), .mosi = IO_TAG(SPI1_MOSI_PIN), .rcc = RCC_APB2(SPI1), .sckAF = SPI1_SCK_AF, .misoAF = SPI1_MISO_AF, .mosiAF = SPI1_MOSI_AF, .divisorMap = spiDivisorMapFast },
#else
    { .dev = SPI1, .nss = IO_TAG(SPI1_NSS_PIN), .sck = IO_TAG(SPI1_SCK_PIN), .miso = IO_TAG(SPI1_MISO_PIN), .mosi = IO_TAG(SPI1_MOSI_PIN), .rcc = RCC_APB2(SPI1), .sckAF = GPIO_AF5_SPI1, .misoAF = GPIO_AF5_SPI1, .mosiAF = GPIO_AF5_SPI1, .divisorMap = spiDivisorMapFast },
#endif
#else
    { .dev = NULL },    // No SPI1
#endif

#ifdef USE_SPI_DEVICE_2
#if defined(SPI2_SCK_AF) || defined(SPI2_MISO_AF) || defined(SPI2_MOSI_AF)
#if !defined(SPI2_SCK_AF) || !defined(SPI2_MISO_AF) || !defined(SPI2_MOSI_AF)
#error SPI2: SCK, MISO and MOSI AFs should be defined together in target.h!
#endif
    { .dev = SPI2, .nss = IO_TAG(SPI2_NSS_PIN), .sck = IO_TAG(SPI2_SCK_PIN), .miso = IO_TAG(SPI2_MISO_PIN), .mosi = IO_TAG(SPI2_MOSI_PIN), .rcc = RCC_APB1L(SPI2), .sckAF = SPI2_SCK_AF, .misoAF = SPI2_MISO_AF, .mosiAF = SPI2_MOSI_AF, .divisorMap = spiDivisorMapSlow },
#else
    { .dev = SPI2, .nss = IO_TAG(SPI2_NSS_PIN), .sck = IO_TAG(SPI2_SCK_PIN), .miso = IO_TAG(SPI2_MISO_PIN), .mosi = IO_TAG(SPI2_MOSI_PIN), .rcc = RCC_APB1L(SPI2), .sckAF = GPIO_AF5_SPI2, .misoAF = GPIO_AF5_SPI2, .mosiAF = GPIO_AF5_SPI2, .divisorMap = spiDivisorMapSlow },
#endif
#else
    { .dev = NULL },    // No SPI2
#endif

#ifdef USE_SPI_DEVICE_3
#if defined(SPI3_SCK_AF) || defined(SPI3_MISO_AF) || defined(SPI3_MOSI_AF)
#if !defined(SPI3_SCK_AF) || !defined(SPI3_MISO_AF) || !defined(SPI3_MOSI_AF)
#error SPI3: SCK, MISO and MOSI AFs should be defined together in target.h!
#endif
    { .dev = SPI3, .nss = IO_TAG(SPI3_NSS_PIN), .sck = IO_TAG(SPI3_SCK_PIN), .miso = IO_TAG(SPI3_MISO_PIN), .mosi = IO_TAG(SPI3_MOSI_PIN), .rcc = RCC_APB1L(SPI3), .sckAF = SPI3_SCK_AF, .misoAF = SPI3_MISO_AF, .mosiAF = SPI3_MOSI_AF, .divisorMap = spiDivisorMapSlow },
#else
    { .dev = SPI3, .nss = IO_TAG(SPI3_NSS_PIN), .sck = IO_TAG(SPI3_SCK_PIN), .miso = IO_TAG(SPI3_MISO_PIN), .mosi = IO_TAG(SPI3_MOSI_PIN), .rcc = RCC_APB1L(SPI3), .sckAF = GPIO_AF6_SPI3, .misoAF = GPIO_AF6_SPI3, .mosiAF = GPIO_AF6_SPI3, .divisorMap = spiDivisorMapSlow },
#endif
#else
    { .dev = NULL },    // No SPI3
#endif

#ifdef USE_SPI_DEVICE_4
#if defined(SPI4_SCK_AF) || defined(SPI4_MISO_AF) || defined(SPI4_MOSI_AF)
#if !defined(SPI4_SCK_AF) || !defined(SPI4_MISO_AF) || !defined(SPI4_MOSI_AF)
#error SPI4: SCK, MISO and MOSI AFs should be defined together in target.h!
#endif
    { .dev = SPI4, .nss = IO_TAG(SPI4_NSS_PIN), .sck = IO_TAG(SPI4_SCK_PIN), .miso = IO_TAG(SPI4_MISO_PIN), .mosi = IO_TAG(SPI4_MOSI_PIN), .rcc = RCC_APB2(SPI4), .sckAF = SPI4_SCK_AF, .misoAF = SPI4_MISO_AF, .mosiAF = SPI4_MOSI_AF, .divisorMap = spiDivisorMapSlow }
#else
    { .dev = SPI4, .nss = IO_TAG(SPI4_NSS_PIN), .sck = IO_TAG(SPI4_SCK_PIN), .miso = IO_TAG(SPI4_MISO_PIN), .mosi = IO_TAG(SPI4_MOSI_PIN), .rcc = RCC_APB2(SPI4), .sckAF = GPIO_AF5_SPI4, .misoAF = GPIO_AF5_SPI4, .mosiAF = GPIO_AF5_SPI4, .divisorMap = spiDivisorMapSlow }
#endif
#else
    { .dev = NULL }     // No SPI4
#endif
};
#else
static spiDevice_t spiHardwareMap[] = {
#ifdef USE_SPI_DEVICE_1
#if defined(SPI1_SCK_AF) || defined(SPI1_MISO_AF) || defined(SPI1_MOSI_AF)
#if !defined(SPI1_SCK_AF) || !defined(SPI1_MISO_AF) || !defined(SPI1_MOSI_AF)
#error SPI1: SCK, MISO and MOSI AFs should be defined together in target.h!
#endif
    { .dev = SPI1, .nss = IO_TAG(SPI1_NSS_PIN), .sck = IO_TAG(SPI1_SCK_PIN), .miso = IO_TAG(SPI1_MISO_PIN), .mosi = IO_TAG(SPI1_MOSI_PIN), .rcc = RCC_APB2(SPI1), .sckAF = SPI1_SCK_AF, .misoAF = SPI1_MISO_AF, .mosiAF = SPI1_MOSI_AF, .divisorMap = spiDivisorMapFast },
#else
    { .dev = SPI1, .nss = IO_TAG(SPI1_NSS_PIN), .sck = IO_TAG(SPI1_SCK_PIN), .miso = IO_TAG(SPI1_MISO_PIN), .mosi = IO_TAG(SPI1_MOSI_PIN), .rcc = RCC_APB2(SPI1), .sckAF = GPIO_AF5_SPI1, .misoAF = GPIO_AF5_SPI1, .mosiAF = GPIO_AF5_SPI1, .divisorMap = spiDivisorMapFast },
#endif
#else
    { .dev = NULL },    // No SPI1
#endif

#ifdef USE_SPI_DEVICE_2
#if defined(SPI2_SCK_AF) || defined(SPI2_MISO_AF) || defined(SPI2_MOSI_AF)
#if !defined(SPI2_SCK_AF) || !defined(SPI2_MISO_AF) || !defined(SPI2_MOSI_AF)
#error SPI2: SCK, MISO and MOSI AFs should be defined together in target.h!
#endif
    { .dev = SPI2, .nss = IO_TAG(SPI2_NSS_PIN), .sck = IO_TAG(SPI2_SCK_PIN), .miso = IO_TAG(SPI2_MISO_PIN), .mosi = IO_TAG(SPI2_MOSI_PIN), .rcc = RCC_APB1(SPI2), .sckAF = SPI2_SCK_AF, .misoAF = SPI2_MISO_AF, .mosiAF = SPI2_MOSI_AF, .divisorMap = spiDivisorMapSlow },
#else
    { .dev = SPI2, .nss = IO_TAG(SPI2_NSS_PIN), .sck = IO_TAG(SPI2_SCK_PIN), .miso = IO_TAG(SPI2_MISO_PIN), .mosi = IO_TAG(SPI2_MOSI_PIN), .rcc = RCC_APB1(SPI2), .sckAF = GPIO_AF5_SPI2, .misoAF = GPIO_AF5_SPI2, .mosiAF = GPIO_AF5_SPI2, .divisorMap = spiDivisorMapSlow },
#endif
#else
    { .dev = NULL },    // No SPI2
#endif

#ifdef USE_SPI_DEVICE_3
#if defined(SPI3_SCK_AF) || defined(SPI3_MISO_AF) || defined(SPI3_MOSI_AF)
#if !defined(SPI3_SCK_AF) || !defined(SPI3_MISO_AF) || !defined(SPI3_MOSI_AF)
#error SPI3: SCK, MISO and MOSI AFs should be defined together in target.h!
#endif
    { .dev = SPI3, .nss = IO_TAG(SPI3_NSS_PIN), .sck = IO_TAG(SPI3_SCK_PIN), .miso = IO_TAG(SPI3_MISO_PIN), .mosi = IO_TAG(SPI3_MOSI_PIN), .rcc = RCC_APB1(SPI3), .sckAF = SPI3_SCK_AF, .misoAF = SPI3_MISO_AF, .mosiAF = SPI3_MOSI_AF, .divisorMap = spiDivisorMapSlow },
#else
    { .dev = SPI3, .nss = IO_TAG(SPI3_NSS_PIN), .sck = IO_TAG(SPI3_SCK_PIN), .miso = IO_TAG(SPI3_MISO_PIN), .mosi = IO_TAG(SPI3_MOSI_PIN), .rcc = RCC_APB1(SPI3), .sckAF = GPIO_AF6_SPI3, .misoAF = GPIO_AF6_SPI3, .mosiAF = GPIO_AF6_SPI3, .divisorMap = spiDivisorMapSlow },
#endif
#else
    { .dev = NULL },    // No SPI3
#endif

#ifdef USE_SPI_DEVICE_4
#if defined(SPI4_SCK_AF) || defined(SPI4_MISO_AF) || defined(SPI4_MOSI_AF)
#if !defined(SPI4_SCK_AF) || !defined(SPI4_MISO_AF) || !defined(SPI4_MOSI_AF)
#error SPI3: SCK, MISO and MOSI AFs should be defined together in target.h!
#endif
    { .dev = SPI4, .nss = IO_TAG(SPI4_NSS_PIN), .sck = IO_TAG(SPI4_SCK_PIN), .miso = IO_TAG(SPI4_MISO_PIN), .mosi = IO_TAG(SPI4_MOSI_PIN), .rcc = RCC_APB2(SPI4), .sckAF = SPI4_SCK_AF, .misoAF = SPI4_MISO_AF, .mosiAF = SPI4_MOSI_AF, .divisorMap = spiDivisorMapSlow }
#else
    { .dev = SPI4, .nss = IO_TAG(SPI4_NSS_PIN), .sck = IO_TAG(SPI4_SCK_PIN), .miso = IO_TAG(SPI4_MISO_PIN), .mosi = IO_TAG(SPI4_MOSI_PIN), .rcc = RCC_APB2(SPI4), .sckAF = GPIO_AF5_SPI4, .misoAF = GPIO_AF5_SPI4, .mosiAF = GPIO_AF5_SPI4, .divisorMap = spiDivisorMapSlow }
#endif
#else
    { .dev = NULL }     // No SPI4
#endif
};
#endif

SPIDevice spiDeviceByInstance(SPI_TypeDef *instance)
{
    if (instance == SPI1)
        return SPIDEV_1;

    if (instance == SPI2)
        return SPIDEV_2;

    if (instance == SPI3)
        return SPIDEV_3;

    if (instance == SPI4)
        return SPIDEV_4;

    return SPIINVALID;
}

void spiTimeoutUserCallback(SPI_TypeDef *instance)
{
    SPIDevice device = spiDeviceByInstance(instance);
    if (device == SPIINVALID) {
        return;
    }

    spiHardwareMap[device].errorCount++;
}

bool spiInitDevice(SPIDevice device, bool leadingEdge)
{
    spiDevice_t *spi = &(spiHardwareMap[device]);

    if (!spi->dev) {
        return false;
    }

    if (spi->initDone) {
        return true;
    }

    // Enable SPI clock
    RCC_ClockCmd(spi->rcc, ENABLE);
    RCC_ResetCmd(spi->rcc, DISABLE);

    IOInit(IOGetByTag(spi->sck),  OWNER_SPI, RESOURCE_SPI_SCK,  device + 1);
    IOInit(IOGetByTag(spi->miso), OWNER_SPI, RESOURCE_SPI_MISO, device + 1);
    IOInit(IOGetByTag(spi->mosi), OWNER_SPI, RESOURCE_SPI_MOSI, device + 1);

    if (leadingEdge) {
        IOConfigGPIOAF(IOGetByTag(spi->sck), SPI_IO_AF_SCK_CFG_LOW, spi->sckAF);
    } else {
        IOConfigGPIOAF(IOGetByTag(spi->sck), SPI_IO_AF_SCK_CFG_HIGH, spi->sckAF);
    }
    IOConfigGPIOAF(IOGetByTag(spi->miso), SPI_IO_AF_MISO_CFG, spi->misoAF);

    IOConfigGPIOAF(IOGetByTag(spi->mosi), SPI_IO_AF_CFG, spi->mosiAF);

    if (spi->nss) {
        IOInit(IOGetByTag(spi->nss),  OWNER_SPI, RESOURCE_SPI_CS,  device + 1);
        IOConfigGPIO(IOGetByTag(spi->nss), SPI_IO_CS_CFG);
    }

    LL_SPI_Disable(spi->dev);
    LL_SPI_DeInit(spi->dev);

    LL_SPI_InitTypeDef init =
    {
        .TransferDirection = SPI_DIRECTION_2LINES,
        .Mode = SPI_MODE_MASTER,
        .DataWidth = SPI_DATASIZE_8BIT,
        .ClockPolarity = leadingEdge ? SPI_POLARITY_LOW : SPI_POLARITY_HIGH,
        .ClockPhase = leadingEdge ? SPI_PHASE_1EDGE : SPI_PHASE_2EDGE,
        .NSS = SPI_NSS_SOFT,
        .BaudRate = SPI_BAUDRATEPRESCALER_8,
        .BitOrder = SPI_FIRSTBIT_MSB,
        .CRCPoly = 7,
        .CRCCalculation = SPI_CRCCALCULATION_DISABLE,
    };

#if defined(STM32H7)
    // Prevent glitching when SPI is disabled
    LL_SPI_EnableGPIOControl(spi->dev);

    LL_SPI_SetFIFOThreshold(spi->dev, LL_SPI_FIFO_TH_01DATA);
    LL_SPI_Init(spi->dev, &init);
#else
    LL_SPI_SetRxFIFOThreshold(spi->dev, SPI_RXFIFO_THRESHOLD_QF);

    LL_SPI_Init(spi->dev, &init);
    LL_SPI_Enable(spi->dev);

    SET_BIT(spi->dev->CR2, SPI_RXFIFO_THRESHOLD);
#endif

    if (spi->nss) {
        IOHi(IOGetByTag(spi->nss));
    }

    spi->initDone = true;
    return true;
}

uint8_t spiTransferByte(SPI_TypeDef *instance, uint8_t txByte)
{
    uint8_t value = 0xFF;
    if (!spiTransfer(instance, &value, &txByte, 1)) {
        return 0xFF;
    }
    return value;
}

/**
 * Return true if the bus is currently in the middle of a transmission.
 */
bool spiIsBusBusy(SPI_TypeDef *instance)
{
#if defined(STM32H7)
    UNUSED(instance);
    // H7 doesnt really have a busy flag. its should be done when the transfer is.
    return false;
#else
    return (LL_SPI_GetTxFIFOLevel(instance) != LL_SPI_TX_FIFO_EMPTY) || LL_SPI_IsActiveFlag_BSY(instance);
#endif
}

bool spiTransfer(SPI_TypeDef *instance, uint8_t *rxData, const uint8_t *txData, int len)
{
#if defined(STM32H7)
    LL_SPI_SetTransferSize(instance, len);
    LL_SPI_Enable(instance);
    LL_SPI_StartMasterTransfer(instance);
    while (len) {
        int spiTimeout = 1000;
        while(!LL_SPI_IsActiveFlag_TXP(instance)) {
            if ((spiTimeout--) == 0) {
                spiTimeoutUserCallback(instance);
                return false;
            }
        }
        uint8_t b = txData ? *(txData++) : 0xFF;
        LL_SPI_TransmitData8(instance, b);

        spiTimeout = 1000;
        while (!LL_SPI_IsActiveFlag_RXP(instance)) {
            if ((spiTimeout--) == 0) {
                spiTimeoutUserCallback(instance);
                return false;
            }
        }
        b = LL_SPI_ReceiveData8(instance);
        if (rxData) {
            *(rxData++) = b;
        }
        --len;
    }
    while (!LL_SPI_IsActiveFlag_EOT(instance));
    LL_SPI_ClearFlag_TXTF(instance);
    LL_SPI_Disable(instance);
#else
    SET_BIT(instance->CR2, SPI_RXFIFO_THRESHOLD);

    while (len) {
        int spiTimeout = 1000;
        while (!LL_SPI_IsActiveFlag_TXE(instance)) {
            if ((spiTimeout--) == 0) {
                spiTimeoutUserCallback(instance);
                return false;
            }
        }
        uint8_t b = txData ? *(txData++) : 0xFF;
        LL_SPI_TransmitData8(instance, b);

        spiTimeout = 1000;
        while (!LL_SPI_IsActiveFlag_RXNE(instance)) {
            if ((spiTimeout--) == 0) {
                spiTimeoutUserCallback(instance);
                return false;
            }
        }
        b = LL_SPI_ReceiveData8(instance);
        if (rxData) {
            *(rxData++) = b;
        }
        --len;
    }
#endif

    return true;
}

void spiSetSpeed(SPI_TypeDef *instance, SPIClockSpeed_e speed)
{
    SPIDevice device = spiDeviceByInstance(instance);
    LL_SPI_Disable(instance);
    LL_SPI_SetBaudRatePrescaler(instance, spiHardwareMap[device].divisorMap[speed]);
    LL_SPI_Enable(instance);
}

SPI_TypeDef * spiInstanceByDevice(SPIDevice device)
{
    return spiHardwareMap[device].dev;
}
