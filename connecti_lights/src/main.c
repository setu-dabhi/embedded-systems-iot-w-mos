#include<string.h>
#include<stdlib.h>
#include "mgos.h"
#include "mgos_mqtt.h"

#define R_PIN 23
#define G_PIN 22
#define B_PIN 21
#define PRG_BTN 0

void button_handler (int pin, void *arg) {
  LOG(LL_INFO, ("Toggling ... "));
  mgos_gpio_set_mode(R_PIN, MGOS_GPIO_MODE_OUTPUT);
  mgos_gpio_toggle(R_PIN);
  LOG(LL_INFO, ("Going to send a publish message to all connected lights ... "));
  //  TODO: 
  mgos_mqtt_pub("v1/devices/me/rpc/request/54", "{}");
  (void) arg;
}

static void sub(struct mg_connection *c, const char *fmt, ...) {
  char buf[100];
  struct mg_mqtt_topic_expression te = {.topic = buf, .qos = 1};
  uint16_t sub_id = mgos_mqtt_get_packet_id();
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  mg_mqtt_subscribe(c, &te, 1, sub_id);
  LOG(LL_INFO, ("Subscribing to %s (id %u)", buf, sub_id));
}

static void pub(struct mg_connection *c, const char *fmt, ...) {
  char msg[200];
  struct json_out jmo = JSON_OUT_BUF(msg, sizeof(msg));
  va_list ap;
  int n;
  va_start(ap, fmt);
  n = json_vprintf(&jmo, fmt, ap);
  va_end(ap);
  mg_mqtt_publish(c, "v1/devices/me/telemetry", mgos_mqtt_get_packet_id(), MG_MQTT_QOS(1), msg, n);
  LOG(LL_INFO, ("%s -> %s", "v1/devices/me/telemetry", msg));
}

static void ev_handler(struct mg_connection *c, int ev, void *p, void *user_data) {
  struct mg_mqtt_message *msg = (struct mg_mqtgt_message *) p;

  if (ev == MG_EV_MQTT_CONNACK) {
    LOG(LL_INFO, ("CONNACK: %d", msg->connack_ret_code));
    // pub(c, "{lohit: %Q}", "started");
    sub(c, "%s", "v1/devices/me/rpc/request/+");
    LOG(LL_INFO, ("Subscribed to RPC Handler --> \"v1/devices/me/rpc/request/+\""));
  } else if (ev == MG_EV_MQTT_SUBACK) {
    LOG(LL_INFO, ("Subscription %u acknowledged: [%.*s]", msg->message_id, (int) (msg->topic).len, (msg->topic).p));
  } else if (ev == MG_EV_MQTT_PUBLISH) {
    struct mg_str *s = &msg->payload;
    int pin, method;
    LOG(LL_INFO, ("Got RPC command: [%.*s]", (int) s->len, s->p));
    /* Our subscription is at QoS 1, we must acknowledge messages sent to us. */
    mg_mqtt_puback(c, msg->message_id);
    if (json_scanf(s->p, s->len, "{method: %Q, params: {pin: %d}}", &method, &pin) == 2) {
      if (strcmp(method, "toggleLED")==0) {
        /* Set GPIO pin to a given state */
        mgos_gpio_set_mode(pin, MGOS_GPIO_MODE_OUTPUT);
        mgos_gpio_toggle(pin);
        
        // TODO:
        // send a response
        // v1/devices/me/rpc/response/$request_id
        // mgos_mqtt_pub(strcat("v1/devices/me/rpc/response/", ), const void *message, size_t len, int qos, bool retain);
        // pub(c, "{type: %Q, pin: %d}", method, pin);
      }
    } 
    else {
      pub(c, "{error: {code: %d, message: %Q}}", 500, "unknown command");
    }
  }
  (void) user_data;
}

void 
enum mgos_app_init_result mgos_app_init(void) {
  // mgos_gpio_set_mode(PRG_BTN, MGOS_GPIO_MODE_INPUT);
  // mgos_gpio_set_int_handler(PRG_BTN, MGOS_GPIO_INT_EDGE_NEG, button_handler, NULL);
  // mgos_gpio_enable_int(PRG_BTN);
  
  // mgos_set_timer(1000, false, sendData, NULL);
  // mgos_mqtt_add_global_handler(ev_handler, NULL);
  mgos_mqtt_add_global_handler(ev_handler, NULL);
  return MGOS_APP_INIT_SUCCESS;
}