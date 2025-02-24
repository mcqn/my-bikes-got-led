#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "Arduino.h"
#include "FastLED.h"
#include "esp_log.h"
#include "app_led.h"

static const char* TAG = "LEDs";

const int kPinLEDSideA = 5;
const int kPinLEDSideB = 6;
// How many LEDs are on each side
const int kLEDCount = 30;

CRGB gLEDsA[kLEDCount];
CRGB gLEDsB[kLEDCount];

QueueHandle_t gLEDsTaskInbox =NULL; // Queue for messages *to* the LEDs task
EventGroupHandle_t gLEDsTaskEventGroup =NULL; // Event Group for signalling responses *from* the LEDs task
// Events used for comms to the task
enum eLEDsEvent
{
    eLEDsEventSuccess = 0,
    eLEDsEventFailure,
    eLEDsEventWaiting,
    eLEDsEventProgress,
    eLEDsEventPattern
};
// Bits in the gLEDsTaskEventGroup
const EventBits_t kLEDsFlagSuccess    = 1 << 0;
const EventBits_t kLEDsFlagFailure    = 1 << 1;
const EventBits_t kLEDsFlagWaiting    = 1 << 2;
const EventBits_t kLEDsFlagProgress   = 1 << 3;
const EventBits_t kLEDsFlagPattern    = 1 << 4;
typedef struct t_LEDsEvent
{
    eLEDsEvent iEvent;
    int iError;
    void* iData;
} tLEDsEvent;
typedef struct t_LEDsPatternEvent
{
    uint8_t iRed;
    uint8_t iGreen;
    uint8_t iBlue;
    enum eLEDPattern iPattern;
} tLEDsPatternEvent;

// Internal states for the display
enum eDisplayState
{
    eDisplayOff,
    eDisplaySuccess,
    eDisplayFailure,
    eDisplayWaiting,
    eDisplayProgress,
    eDisplayPattern
};

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
    FastLED.addLeds<WS2812,kPinLEDSideA,GRB>(gLEDsA,kLEDCount);
    FastLED.addLeds<WS2812,kPinLEDSideB,GRB>(gLEDsB,kLEDCount);
    FastLED.setBrightness(84);

    uint8_t hue = 0;
    int idx = 0;
    bool direction = true;
    eDisplayState currentState = eDisplayOff;
    uint8_t progressLevel = 0;
    enum eLEDPattern currentPattern = ePatternNone;
    CRGB currentPatternColour;
    while (1)
    {
        // See if there are any messages for us
        tLEDsEvent ev;
        if (xQueueReceive(gLEDsTaskInbox, &ev, 10) == pdTRUE)
        {
            ESP_LOGI(TAG, "Received %d\n", ev.iEvent);
            switch (ev.iEvent)
            {
            case eLEDsEventSuccess:
            {
                currentState = eDisplaySuccess;
                // Confirm the message
                xEventGroupSetBits(gLEDsTaskEventGroup, kLEDsFlagSuccess);
            }
            break;
            case eLEDsEventFailure:
            {
                currentState = eDisplayFailure;
                // Confirm the message
                xEventGroupSetBits(gLEDsTaskEventGroup, kLEDsFlagFailure);
            }
            break;
            case eLEDsEventWaiting:
            {
                currentState = eDisplayWaiting;
                // Confirm the message
                xEventGroupSetBits(gLEDsTaskEventGroup, kLEDsFlagWaiting);
            }
            break;
            case eLEDsEventProgress:
            {
                currentState = eDisplayProgress;
                progressLevel = (uint32_t)(ev.iData) & 0xff;
                ESP_LOGW(TAG, "Progress level is %d%%", progressLevel);
                // Confirm the message
                xEventGroupSetBits(gLEDsTaskEventGroup, kLEDsFlagProgress);
            }
            break;
            case eLEDsEventPattern:
            {
                currentState = eDisplayPattern;
                tLEDsPatternEvent* pattern = (tLEDsPatternEvent*)ev.iData;
                currentPattern = pattern->iPattern;
                currentPatternColour = CRGB(pattern->iRed, pattern->iGreen, pattern->iBlue);
                // Confirm the message
                xEventGroupSetBits(gLEDsTaskEventGroup, kLEDsFlagPattern);
            }
            break;
            default:
                ESP_LOGI(TAG, "Unexpected event: %d", ev.iEvent);
                break;
            };
        }
        // Now animate the LEDs based on our state
        switch (currentState)
        {
        case eDisplayOff:
        {
            LEDsFadeAll();
            // Show the leds
            FastLED.show();
        }
        break;
        case eDisplaySuccess:
        {
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
        }
        break;
        case eDisplayFailure:
        {
            // We're going to abuse "hue" as the brightness level
            for (int i =0; i < kLEDCount; i++)
            {
                gLEDsA[i] = CHSV(0, 255, hue);
                gLEDsB[i] = CHSV(0, 255, hue);
            }
            // Move to the next brightness based on our direction
            if (direction)
            {
                hue++;
                if (hue == 255)
                {
                    direction = false;
                }
            }
            else
            {
                hue--;
                if (hue == 0)
                {
                    direction = true;
                }
            }

            // Show the leds
            FastLED.show();
        }
        break;
        case eDisplayWaiting:
        {
            // Set the i'th led to yellow 
            gLEDsA[idx] = CHSV(60, 255, 255);
            gLEDsB[idx] = CHSV(60, 255, 255);
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
            LEDsFadeAll();

            // This should run slower than the fast animations
            vTaskDelay(6 / portTICK_PERIOD_MS);
        }
        break;
        case eDisplayProgress:
        {
            // Fade out any extra LEDs rather than just turn them off
            LEDsFadeAll();
            LEDsFadeAll();
            for (int i =0; i < progressLevel*kLEDCount/100; i++)
            {
                gLEDsA[i] = CHSV(195, 255, 255);
                gLEDsB[i] = CHSV(195, 255, 255);
            }
		    // Show the leds
            FastLED.show();
        }
        break;
        case eDisplayPattern:
        {
            switch (currentPattern)
            {
            break;
            case ePatternLarson:
            {
                // Set the i'th led colour
                gLEDsA[idx] = currentPatternColour;
                gLEDsB[idx] = currentPatternColour;
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
                LEDsFadeAll();

                // This should run a bit slower than the fast animations
                vTaskDelay(2 / portTICK_PERIOD_MS);
            }
            break;
            case ePatternAlternate:
            {
                for (int i =0; i < kLEDCount; i++)
                {
                    // Do pairs of LEDs and alternate them
                    if ((i/2 % 2) == direction)
                    {
                        gLEDsA[i] = CRGB(0, 0, 0);
                        gLEDsB[i] = CRGB(0, 0, 0);
                    }
                    else
                    {
                        gLEDsA[i] = currentPatternColour;
                        gLEDsB[i] = currentPatternColour;
                    }
                }
                direction = !direction;
                FastLED.show();
                // This should run a bit slower than the fast animations
                vTaskDelay(500 / portTICK_PERIOD_MS);
            }
            break;
            default:
                ESP_LOGE(TAG, "Unexpected pattern %d", (int)currentPattern);
                /* Intentional drop through */
            case ePatternNone:
                LEDsFadeAll();
                // Show the leds
                FastLED.show();
            break;
            };
        }
        break;
        default:
            ESP_LOGW(TAG, "Got into an unknown state: %d", currentState);
            break;
        };

        // Don't need to wait here, because we have a delay in the "check the message queue" call
        //vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

int LEDs_Start()
{
    // Create the task queues for inter-task comms
    gLEDsTaskInbox = xQueueCreate( 5, sizeof( tLEDsEvent ) );
    if (gLEDsTaskInbox == NULL)
    {
        ESP_LOGE(TAG, "Failed to create queue");
        return -ENOMEM;
    }
    gLEDsTaskEventGroup = xEventGroupCreate();
    if (gLEDsTaskEventGroup == NULL)
    {
        ESP_LOGE(TAG, "Failed to create event group");
        return -ENOMEM;
    }
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
    // Queue the operation with the LEDs task
    BaseType_t ret;
    ESP_LOGI(TAG, "Sending %s request", __FUNCTION__);
    tLEDsEvent ev;
    ev.iEvent = eLEDsEventSuccess;
    ev.iError = 0;
    ev.iData = NULL;
    ret = xQueueSendToBack(gLEDsTaskInbox, (const void*)&ev, 1000);
    if (ret != pdFAIL)
    {
        // Wait for the response
        bool flagged = false;
        do {
            ESP_LOGD(TAG, "About to wait for the response\n");
            if (kLEDsFlagSuccess == xEventGroupWaitBits(gLEDsTaskEventGroup, kLEDsFlagSuccess, pdTRUE, pdFALSE, 1000))
            {
                // Correct bit set
                flagged = true;
            }
        } while(flagged == false);
        ESP_LOGI(TAG, "Got a response: %d\n", ev.iError);
        return ev.iError;
    }
    else
    {
        ESP_LOGE(TAG, "Failed to queue message\n");
        return pdFAIL;
    }
    return -1;
}

int LEDs_Failure()
{
    // Queue the operation with the LEDs task
    BaseType_t ret;
    ESP_LOGI(TAG, "Sending %s request", __FUNCTION__);
    tLEDsEvent ev;
    ev.iEvent = eLEDsEventFailure;
    ev.iError = 0;
    ev.iData = NULL;
    ret = xQueueSendToBack(gLEDsTaskInbox, (const void*)&ev, 1000);
    if (ret != pdFAIL)
    {
        // Wait for the response
        bool flagged = false;
        do {
            ESP_LOGD(TAG, "About to wait for the response\n");
            if (kLEDsFlagFailure == xEventGroupWaitBits(gLEDsTaskEventGroup, kLEDsFlagFailure, pdTRUE, pdFALSE, 1000))
            {
                // Correct bit set
                flagged = true;
            }
        } while(flagged == false);
        ESP_LOGI(TAG, "Got a response: %d\n", ev.iError);
        return ev.iError;
    }
    else
    {
        ESP_LOGE(TAG, "Failed to queue message\n");
        return pdFAIL;
    }
    return -1;
}

int LEDs_Waiting()
{
    // Queue the operation with the LEDs task
    BaseType_t ret;
    ESP_LOGI(TAG, "Sending %s request", __FUNCTION__);
    tLEDsEvent ev;
    ev.iEvent = eLEDsEventWaiting;
    ev.iError = 0;
    ev.iData = NULL;
    ret = xQueueSendToBack(gLEDsTaskInbox, (const void*)&ev, 1000);
    if (ret != pdFAIL)
    {
        // Wait for the response
        bool flagged = false;
        do {
            ESP_LOGD(TAG, "About to wait for the response\n");
            if (kLEDsFlagWaiting == xEventGroupWaitBits(gLEDsTaskEventGroup, kLEDsFlagWaiting, pdTRUE, pdFALSE, 1000))
            {
                // Correct bit set
                flagged = true;
            }
        } while(flagged == false);
        ESP_LOGI(TAG, "Got a response: %d\n", ev.iError);
        return ev.iError;
    }
    else
    {
        ESP_LOGE(TAG, "Failed to queue message\n");
        return pdFAIL;
    }
    return -1;
}

int LEDs_Progress(uint8_t aPercentDone)
{
    // Queue the operation with the LEDs task
    BaseType_t ret;
    ESP_LOGI(TAG, "Sending %s request", __FUNCTION__);
    tLEDsEvent ev;
    ev.iEvent = eLEDsEventProgress;
    ev.iError = 0;
    ev.iData = (void*)aPercentDone;
    ret = xQueueSendToBack(gLEDsTaskInbox, (const void*)&ev, 1000);
    if (ret != pdFAIL)
    {
        // Wait for the response
        bool flagged = false;
        do {
            ESP_LOGD(TAG, "About to wait for the response\n");
            if (kLEDsFlagProgress == xEventGroupWaitBits(gLEDsTaskEventGroup, kLEDsFlagProgress, pdTRUE, pdFALSE, 1000))
            {
                // Correct bit set
                flagged = true;
            }
        } while(flagged == false);
        ESP_LOGI(TAG, "Got a response: %d\n", ev.iError);
        return ev.iError;
    }
    else
    {
        ESP_LOGE(TAG, "Failed to queue message\n");
        return pdFAIL;
    }
    return -1;
}

int LEDs_Pattern(uint8_t aRed, uint8_t aGreen, uint8_t aBlue, enum eLEDPattern aPattern)
{
    // Queue the operation with the LEDs task
    BaseType_t ret;
    ESP_LOGI(TAG, "Sending %s request", __FUNCTION__);
    tLEDsEvent ev;
    tLEDsPatternEvent pattern = { aRed, aGreen, aBlue, aPattern };
    ev.iEvent = eLEDsEventPattern;
    ev.iError = 0;
    ev.iData = (void*)&pattern;
    ret = xQueueSendToBack(gLEDsTaskInbox, (const void*)&ev, 1000);
    if (ret != pdFAIL)
    {
        // Wait for the response
        bool flagged = false;
        do {
            ESP_LOGD(TAG, "About to wait for the response\n");
            if (kLEDsFlagPattern == xEventGroupWaitBits(gLEDsTaskEventGroup, kLEDsFlagPattern, pdTRUE, pdFALSE, 1000))
            {
                // Correct bit set
                flagged = true;
            }
        } while(flagged == false);
        ESP_LOGI(TAG, "Got a response: %d\n", ev.iError);
        return ev.iError;
    }
    else
    {
        ESP_LOGE(TAG, "Failed to queue message\n");
        return pdFAIL;
    }
    return -1;
}

