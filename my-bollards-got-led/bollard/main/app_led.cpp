#include "Arduino.h"
#include "FastLED.h"
#include "esp_log.h"
#include "app_led.h"

static const char* TAG = "LEDs";

const int kPinLEDSideA = 5;
const int kPinLEDSideB = 6;
// How many LEDs are on each side
const int kLEDCount = 40;

CRGB gLEDsA[kLEDCount];
CRGB gLEDsB[kLEDCount];

void LEDsFadeAll()
{
    for (int i =0; i < kLEDCount; i++)
    {
        gLEDsA[i].nscale8(250);
        gLEDsB[i].nscale8(250);
    }
}


TaskHandle_t gLEDsTask =NULL;
static void led_task(void* aParam)
{
    // Get things ready for Arduino
    ESP_LOGW(TAG, "Let's Go!");
    initArduino();

    ESP_LOGW(TAG, "LED task is running");
    FastLED.addLeds<WS2812,kPinLEDSideA,RGB>(gLEDsA,kLEDCount);
    FastLED.addLeds<WS2812,kPinLEDSideB,RGB>(gLEDsB,kLEDCount);
    FastLED.setBrightness(84);

    uint8_t hue = 0;
    int idx = 0;
    bool direction = true;
    while (1) {
		// Set the i'th led to red 
		gLEDsA[idx] = CHSV(hue++, 255, 255);
		gLEDsB[idx] = CHSV(hue++, 255, 255);
        // Move to the next LED based on our direction
        if (direction)
        {
            idx++;
            if (idx >= kLEDCount)
            {
                idx = kLEDCount-1;
                direction = false;
            }
        }
        else
        {
            idx--;
            if (idx < 0)
            {
                idx = 0;
                direction = true;
            }
        }

		// Show the leds
		FastLED.show(); 
        //ESP_LOGW(TAG, "hue: %d, idx: %d, direction: %s", hue, idx, (direction? "up" : "down"));

        LEDsFadeAll();

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

int LEDs_Start()
{
    BaseType_t ret = xTaskCreate(led_task, "led_task", 8192, NULL, 1, &gLEDsTask);
    if (ret == pdPASS)
    {
        // All good
        return 0;
    }
    else
    {
        ESP_LOGE(TAG, "Failed to create LEDs task: %d", ret);
        return ret;
    }
}

int LEDs_Success()
{
    ESP_LOGE(TAG, "%s not yet implemented.", __FUNCTION__);
    return -ENOTSUP;
}

int LEDs_Failure()
{
    ESP_LOGE(TAG, "%s not yet implemented.", __FUNCTION__);
    return -ENOTSUP;
}

int LEDs_Waiting()
{
    ESP_LOGE(TAG, "%s not yet implemented.", __FUNCTION__);
    return -ENOTSUP;
}

int LEDs_Progress(uint8_t aPercentDone)
{
    ESP_LOGE(TAG, "%s not yet implemented.  Would have filled in to %d%%", __FUNCTION__, aPercentDone);
    return -ENOTSUP;
}

