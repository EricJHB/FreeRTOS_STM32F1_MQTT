#ifndef _MQTT_APP_H_
#define _MQTT_APP_H_


#include "mqtt.h"




struct MqttSampleContext
{
//    int epfd;
//    int mqttfd;
    uint32_t sendedbytes;
    struct MqttContext mqttctx[1];
    struct MqttBuffer mqttbuf[1];

    const char *host;
    unsigned short port;
	
    const char *proid;
    const char *devid;
    const char *apikey;

    int dup;
    enum MqttQosLevel qos;
    int retain;

    uint16_t pkt_to_ack;
    char cmdid[128];
};



int MqttSample_Init(struct MqttSampleContext *ctx);

int MqttSample_ReInit(struct MqttSampleContext *ctx);

int MqttSample_Connect(struct MqttSampleContext *ctx, char *proid, char *auth_info, const char *devid, int keep_alive, int clean_session);

int MqttSample_Subscribe(struct MqttSampleContext *ctx, char **topic, int num);

int MqttSample_Unsubscribe(struct MqttSampleContext *ctx, char **topics, int num);

int MqttSample_Savedata(struct MqttSampleContext *ctx, char *t_json);

int MqttSample_Publish(struct MqttSampleContext *ctx, char *payload);


#endif
