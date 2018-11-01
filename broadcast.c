
#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"
#include "core/lib/sensors.h"
#include "dev/button-sensor.h"
#include "platform/sky/dev/light-sensor.h""


#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  printf("broadcast message received from %d.%d: '%s'\n",
         from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  printf("unicast message received from %d.%d\n",
	 from->u8[0], from->u8[1]);
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_broadcast_process, ev, data)
{
  static struct etimer et;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();

  broadcast_open(&broadcast, 129, &broadcast_call);

  unicast_open(&uc, 146, &unicast_callbacks);

  int val;	//activate sensors
	SENSORS_ACTIVATE(light_sensor);

	printf("You have been yootent to!\n");	//pointless Start print
	printf("Yeet!");

  while(1) {

    	/* Delay 2-4 seconds */
    	etimer_set(&et, (CLOCK_SECOND * 4 + random_rand() % (CLOCK_SECOND * 4)));

    	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

	val = light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR);
	//printf("Light=%dlux \n", val); 

	char output[4];
	

	sprintf(output, "%d", val);

	
   	packetbuf_copyfrom("FEED ME!", 10);
    	broadcast_send(&broadcast);
    	printf("broadcast message sent\n");
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/