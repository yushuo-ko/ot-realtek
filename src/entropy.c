/*
 *  Copyright (c) 2019, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements an entropy source based on ADC.
 *
 */

#include <openthread/platform/entropy.h>

#include <openthread/platform/radio.h>

#include "platform-bee.h"
#include "utils/code_utils.h"
#include "common/logging.hpp"

void BEE_RandomInit(void)
{
}

otError otPlatEntropyGet(uint8_t *aOutput, uint16_t aOutputLength)
{
    otError error   = OT_ERROR_NONE;
    uint32_t rnd_num;
    uint32_t index = 0;
    uint32_t *pbuf;
    otEXPECT_ACTION(aOutput, error = OT_ERROR_INVALID_ARGS);
    pbuf = (uint32_t *)aOutput;

    while (aOutputLength > 0)
    {
#ifdef RT_PLATFORM_RTL8852D
        rnd_num = lc_generate_rand_number(otPlatRadioGetNow(NULL));
#else
        rnd_num = platform_random(0xffffffff);
#endif
        if (aOutputLength > sizeof(uint32_t))
        {
            *pbuf = rnd_num;
            pbuf++;
            aOutputLength -= sizeof(uint32_t);
        }
        else
        {
            index = (uint32_t)pbuf - (uint32_t)aOutput;

            while (aOutputLength > 0)
            {
                aOutput[index] = rnd_num & 0xFF;
                rnd_num = rnd_num >> 8;
                index++;
                aOutputLength--;
            }
        }
    }

exit:
    return error;
}

