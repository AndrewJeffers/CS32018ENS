

#include "contiki.h"
#include "net/rime/rime.h"
#include "platform/sky/dev/light-sensor.h""
#include "dev/button-sensor.h"
#include "core/lib/sensors.h"
#include "dev/leds.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "Example unicast");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  printf("broadcast message received from %d.%d: '%s'\n",
         from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
	
	
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  printf("unicast message received from %d.%d\n",
	 from->u8[0], from->u8[1]);
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data)
{
  PROCESS_EXITHANDLER(unicast_close(&uc);)
    
  PROCESS_BEGIN();

  broadcast_open(&broadcast, 129, &broadcast_call);

  int val;	//activate sensors
  SENSORS_ACTIVATE(light_sensor);

  unicast_open(&uc, 146, &unicast_callbacks);

  while(1) {
    	static struct etimer et;
    	linkaddr_t addr;
    
    	etimer_set(&et, CLOCK_SECOND*4);
    
    	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

	val = light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR);
	printf("Light=%dlux \n", val); 

	char output[4];
	

	sprintf(output, "%d", val);
	

    	packetbuf_copyfrom(output, 4);
    	addr.u8[0] = 1;
    	addr.u8[1] = 0;
    	if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
     		 unicast_send(&uc, &addr);
    	}

  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/