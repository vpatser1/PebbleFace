#include <pebble.h>
#include <math.h>
  
Window *window;	

TextLayer *text_layer;
char *text_buffer;	
int xValue = 0;
int yValue = 0;
int zValue = 0;
int smoothx = 0;
int smoothy = 0;
int smoothz = 0;
float k = .925;

char *printVal = "bbbb";
int initialAccel = -1;
int liftCount = 0;

bool lifting = 0;
bool up = 0;
const int threshold_up_start = 140;
const int threshold_up_stop = 60;

const int threshold_down_start = -140;
const int threshold_down_stop = -60;


// Key values for AppMessage Dictionary
enum {
	LIFTS,
};

#define iter1(N) try = root + (1 << (N)); if (n >= try << (N)) { n -= try << (N); root |= 2 << (N); }

uint32_t Wilco_sqrt(uint32_t n)
{
	uint32_t root = 0, try;
	iter1 (15); iter1 (14); iter1 (13); iter1 (12); iter1 (11);
	iter1 (10); iter1 ( 9); iter1 ( 8); iter1 ( 7); iter1 ( 6);
	iter1 ( 5); iter1 ( 4); iter1 ( 3); iter1 ( 2); iter1 ( 1); iter1 ( 0);
	return root >> 1;
}

int magnitude(int a, int b, int c)
{
  return Wilco_sqrt(a * a + b * b + c * c);
}



void print_accel_data(int16_t accel_x, int16_t accel_y, int16_t accel_z) {
  /*if(abs(xValue)>abs(yValue) && abs(xValue)>abs(zValue)){
    if(xValue>0){
      printVal = "Right";
    }
    else{
      printVal = "Left";
    }
  }
  else if(abs(yValue)>abs(xValue) && abs(yValue)>abs(zValue)){
    if(yValue>0){
      printVal = "Back";
    }
    else{
      printVal = "Front";
    }
  }
  else{
    if(zValue>0){
      printVal = "Bottom";
    }
    else{
      printVal = "Top";
    }
  }*/
  //try to get velocity
  
  
  
  //int vel = abs(magnitude(xValue, yValue, zValue) - initialAccel); //this is the magnitude of acceleration, not the velocity!
   //int velsmooth = abs(magnitude(smoothx, smoothy, smoothz) - initialAccel);
  
  int vel = (magnitude(xValue, yValue, zValue) - initialAccel );
  int velsmooth = (magnitude(smoothx, smoothy, smoothz) - initialAccel );
  
  /*
  if(!lifting && velsmooth > threshold_high)
    lifting = 1;
  if(lifting && velsmooth < threshold_low)
  {
    lifting = 0;
    printVal = "DOWN";
    up = !up;
    if(!up){
      liftCount++;
      printVal = "UP";
    }
  }*/
  
  if(up)
  {
    if(velsmooth > threshold_up_start && !lifting)
      lifting = true;
    else if (velsmooth < threshold_up_stop && lifting)
    {
      lifting = false;
      up = false;
    }
  } else {
    if(velsmooth < threshold_down_start && !lifting)
      lifting = true;
    else if(velsmooth > threshold_down_stop && lifting)
    {
      lifting = false;    
      up = true;
      liftCount++;
    }
  }
  
  if(up)
     printVal = "UP";
  else
    printVal = "DOWN";
    
    
  
  
	//snprintf(text_buffer, 100, "x:%i, y:%i, z:%i \nInitial: %i", accel_x, accel_y, accel_z, initialAccel);
  snprintf(text_buffer, 100, "C: %i \nS: %i \nI: %i\nLift: %i\n%s", vel, velsmooth, initialAccel, liftCount, printVal);
	text_layer_set_text(text_layer, text_buffer);
}



// Write message to buffer & send
void send_message(int16_t lifts){
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	dict_write_uint8(iter, LIFTS, lifts);
	dict_write_end(iter);
  app_message_outbox_send();
}

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {	
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}

void accel_data_handler(AccelData *data, uint32_t num_samples) {
//	send_message(data->x, data->y, data->z);
	print_accel_data(data->x, data->y, data->z);
  xValue = data->x;
  yValue = data->y;
  zValue = data->z;
  
  smoothx = k * smoothx + (1.0 - k) * xValue;
  smoothy = k * smoothy + (1.0 - k) * yValue;
  smoothz = k * smoothz + (1.0 - k) * zValue;
  
  

  if(initialAccel == -1){
    initialAccel = magnitude(xValue, yValue, zValue);
    
  }
}

void window_load(Window *window) {
	text_buffer = malloc(100);	
	text_layer = text_layer_create(GRect(0,0,144,154));
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));
}

void window_unload(Window *window) {
	text_layer_destroy(text_layer);
	free(text_buffer);
}


void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  initialAccel = magnitude(xValue, yValue, zValue);
}

void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  send_message(liftCount);
}

void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  liftCount = 0;
}

void config_provider(Window *window) {
 // single click / repeat-on-hold config:
  window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);  

}


void init(void) {
  printVal = malloc(7);
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
				.load = window_load,
				.unload = window_unload
	});
	window_stack_push(window, true);
	
  
  window_set_click_config_provider(window, (ClickConfigProvider) config_provider);
  
  
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
	accel_data_service_subscribe(1, accel_data_handler);
  
  
}

void deinit(void) {
  free(printVal);
	app_message_deregister_callbacks();
	window_destroy(window);
}

int main( void ) {
	init();
	app_event_loop();
	deinit();
}

