

#include "contiki.h"
#include "net/rime/rime.h"
#include "platform/sky/dev/light-sensor.h"
#include "dev/button-sensor.h"
#include "core/lib/sensors.h"
#include "dev/leds.h"
#include "stdio.h"
/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "Example unicast");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/
static struct unicast_conn uc;
//preinitialise to prevent dividing by 0
int values[10]={32,45,168,221,50,45,87,96,103,130};
int index=0;
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  printf("broadcast message received from %d.%d: '%s'\n",
         from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
	
	if(!strcmp("FEED ME!", (char *)packetbuf_dataptr())){
	linkaddr_t addr;
	int val=0;
	int i=0;
	while(i<10){
		val += values[i];
		
		i++;		
	}
 	val = val/10;
	//printf("Light=%dlux \n", val); 
	char output[4];
	
	sprintf(output, "%d", val);
	
    	packetbuf_copyfrom(output, 4);

	addr.u8[0] = from->u8[0];
    	addr.u8[1] = from->u8[1];

	if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
     		 unicast_send(&uc, &addr);
    	}
}	
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

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data)
{
  PROCESS_EXITHANDLER(unicast_close(&uc);)
    
  PROCESS_BEGIN();
  broadcast_open(&broadcast, 129, &broadcast_call);	//activate sensors
  SENSORS_ACTIVATE(light_sensor);
  unicast_open(&uc, 146, &unicast_callbacks);
  while(1) {
    	static struct etimer et;
    	linkaddr_t addr;
    
    	etimer_set(&et, CLOCK_SECOND);
    	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));


	values[index] = light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR);
	index++;
	
	if(index==10){index=0;}
	
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/