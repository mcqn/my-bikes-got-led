#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/event_groups.h>
#include <string.h>
#include <errno.h>
#include <set>
#include "esp_log.h"
#include "app_led.h"
#include "app_game.h"

static const char* TAG = "Game";

const int kBollardCount = 4;
const char* kBollardNames[kBollardCount] = {
    "cc:ba:97:47:69:46",
    //"e4:b3:23:03:9e:ee", // AMc dev unit
    "cc:ba:97:47:75:36",
    "cc:ba:97:47:67:aa",
    "cc:ba:97:47:93:22"
};
// Which bollard we are.  Set as index_into_kBollardNames+1; -1 is "not set"
int gBollardID = -1;

QueueHandle_t gGameTaskInbox =NULL; // Queue for messages *to* the Game task
EventGroupHandle_t gGameTaskEventGroup =NULL; // Event Group for signalling responses *from* the Game task
// Events used for comms to the task
enum eGameEvent
{
    eGameEventMessageReceived = 0
};
// Bits in the gGameTaskEventGroup
const EventBits_t kGameFlagMessageReceived    = 1 << 0;
typedef struct t_GameEvent
{
    eGameEvent iEvent;
    int iError;
    void* iData;
} tGameEvent;

typedef struct t_GameMessage
{
    char* iSender;
    int iRSSI;
    char* iMessage;
    int iMessageLen;
} tGameMessage;

class tParcelMessage
{
public:
    tParcelMessage(const char* aSender, int aRSSI, int aDestinationBollard);
    tParcelMessage(const tParcelMessage& aToCopy);
    ~tParcelMessage() { free(iSender); };
    bool Nearby() { return (iRSSI > -50); };
    bool SeenRecently() { return (esp_timer_get_time() - iTimeReceived < 5000ULL); };
    bool For(int aDestinationBollard) { return (aDestinationBollard == iDestinationBollard); };
    void Update(int aRSSI, int aDestinationBollard) { iRSSI = aRSSI; iDestinationBollard = aDestinationBollard; iTimeReceived = esp_timer_get_time(); };
    // FIXME temp public so comparison overload can access it.  We should friend that really
    char* iSender;
private:
    int iRSSI;
    int iDestinationBollard;
    time_t iTimeReceived;
};
tParcelMessage::tParcelMessage(const char* aSender, int aRSSI, int aDestinationBollard)
    : iRSSI(aRSSI), iDestinationBollard(aDestinationBollard)
{
    iSender = strdup(aSender);
    iTimeReceived = esp_timer_get_time();
}
tParcelMessage::tParcelMessage(const tParcelMessage& aToCopy)
    : iRSSI(aToCopy.iRSSI), iDestinationBollard(aToCopy.iDestinationBollard), iTimeReceived(aToCopy.iTimeReceived)
{
    iSender = strdup(aToCopy.iSender);
}
bool operator<(const tParcelMessage& L, const tParcelMessage& R)
{
    return (strcmp(L.iSender, R.iSender) < 0);
}

std::set<tParcelMessage> gParcelsSeen;

// Internal states for the game
enum eGameState
{
    eGameOff,
    eParcelsBeingDelivered,
    eCheckParcels
};

eGameState gCurrentGameState = eGameOff;

void Game_HandleMessageSet()
{
    // Reset the list of devices seen
    gParcelsSeen.clear();
    gCurrentGameState = eParcelsBeingDelivered;
    // Set the display accordingly
    LEDs_Waiting();
}

void Game_HandleMessageCheck()
{
    if (gCurrentGameState != eCheckParcels)
    {
        gCurrentGameState = eCheckParcels;
        // Loop through the list of devices we've seen
        for (tParcelMessage parcel : gParcelsSeen)
        {
            // If we've seen it recently and signal strength means it's nearby
            //parcel.Update(1,2);
            if (parcel.Nearby() && parcel.SeenRecently())
            {
                if (!parcel.For(gBollardID))
                {
                    // if it isn't for us then fail
                    LEDs_Failure();
                    return;
                }
            }
        }
        // If we get here either there weren't any parcels for us, or
        // all the parcels delivered were for us
        LEDs_Success();
    }
}

void Game_HandleMessageBollard(tGameMessage* aMsg)
{
    if (gCurrentGameState == eParcelsBeingDelivered)
    {
        // Extract the bollard ID from the message
        int bollard;
        // FIXME We should parse this properly, but for now assume it's in character 5
        bollard = aMsg->iMessage[3] - '0';
        ESP_LOGW(TAG, "Parcel seen destined for %d", bollard);
        // I don't like how std::set won't let me update the object in the set, as this
        // will end up mallocing and freeing lots of small sender strings when we could just
        // update a couple of ints in-place.
        // Maybe there's a different STL container that would work better?
        // Maybe set::emplace or set::swap would be more efficient?
        // The set only compares on sender, so it doesn't matter what the RSSI and destination are
        gParcelsSeen.erase(tParcelMessage(aMsg->iSender, 0, 0));
        gParcelsSeen.insert(tParcelMessage(aMsg->iSender, aMsg->iRSSI, bollard));
    }
    else
    {
        ESP_LOGI(TAG, "Ignoring bollard message");
    }
}

TaskHandle_t gGameTask =NULL;
static void game_task(void* aParam)
{
    ESP_LOGW(TAG, "Game task is running");

    while (1)
    {
        // See if there are any messages for us
        tGameEvent ev;
        if (xQueueReceive(gGameTaskInbox, &ev, 10) == pdTRUE)
        {
            ESP_LOGI(TAG, "Received %d\n", ev.iEvent);
            switch (ev.iEvent)
            {
            case eGameEventMessageReceived:
            {
                tGameMessage* msg = (tGameMessage*)ev.iData;
                if ( 0 == strncmp(msg->iMessage, "CHECK", strlen("CHECK")) )
                {
                    Game_HandleMessageCheck();
                }
                else if ( 0 == strncmp(msg->iMessage, "SET", strlen("SET")) )
                {
                    Game_HandleMessageSet();
                }
                else if ( 0 == strncmp(msg->iMessage, "BOL", strlen("BOL")) )
                {
                    Game_HandleMessageBollard(msg);
                }
                else
                {
                    ESP_LOGW(TAG, "Got message %s that we didn't expect", msg->iMessage);
                }
                // Confirm the message
                xEventGroupSetBits(gGameTaskEventGroup, kGameFlagMessageReceived);
            }
            break;
            default:
                ESP_LOGI(TAG, "Unexpected event: %d", ev.iEvent);
                break;
            };
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

int Game_Start(const char* aName)
{
    // Work out which bollard we are
    gBollardID = -1;
    for (int i =0; i < kBollardCount; i++)
    {
        if (0 == strcmp(aName, kBollardNames[i]))
        {
            gBollardID = i;
            ESP_LOGW(TAG, "We're bollard %d", gBollardID);
        }
    }

    // Create the task queues for inter-task comms
    gGameTaskInbox = xQueueCreate( 5, sizeof( tGameEvent ) );
    if (gGameTaskInbox == NULL)
    {
        ESP_LOGE(TAG, "Failed to create queue");
        return -ENOMEM;
    }
    gGameTaskEventGroup = xEventGroupCreate();
    if (gGameTaskEventGroup == NULL)
    {
        ESP_LOGE(TAG, "Failed to create event group");
        return -ENOMEM;
    }
    BaseType_t ret = xTaskCreate(game_task, "game_task", 8192, NULL, 1, &gGameTask);
    if (ret == pdPASS)
    {
        // All good
        return 0;
    }
    else
    {
        ESP_LOGE(TAG, "Failed to create Game task: %d", ret);
        return ret;
    }
}

int Game_MessageReceived(char* aSender, int aRSSI, char* aMessage, int aMessageLen)
{
    // Queue the operation with the Game task
    BaseType_t ret;
    ESP_LOGI(TAG, "Sending request\n");
    tGameEvent ev;
    ev.iEvent = eGameEventMessageReceived;
    ev.iError = 0;
    tGameMessage details = { aSender, aRSSI, aMessage, aMessageLen };
    ev.iData = &details;
    ret = xQueueSendToBack(gGameTaskInbox, (const void*)&ev, 1000);
    if (ret != pdFAIL)
    {
        // Wait for the response
        bool flagged = false;
        do {
            ESP_LOGD(TAG, "About to wait for the response\n");
            if (kGameFlagMessageReceived == xEventGroupWaitBits(gGameTaskEventGroup, kGameFlagMessageReceived, pdTRUE, pdFALSE, 1000))
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

