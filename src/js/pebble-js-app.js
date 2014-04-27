function test1(e){
 var req = new XMLHttpRequest();
  req.open('POST', 'https://api.sendgrid.com/api/mail.send.json'+
            '?api_user='+'vpatser1'+
            '&api_key='+'itoldyou1'+
            '&to='+'tweet@tweetymail.com'+
            '&toname='+'Tweety'+
            '&subject='+'Ayy! '+
            '&text='+ 
            'Do you even lift bro?! I just lifted ' + e +
            '! @HackBinghamton'+
            '&from='+'vpatser1@binghamton.edu', true);
  req.onload = function(e) {
    if (req.readyState == 4 && req.status == 200) {
      if(req.status == 200) {
        var response = JSON.parse(req.responseText);
        if (response.message == 'success'){
          console.log('It worked!');
        }
        else{
          console.log('It broke!');
        }
        //var temperature = response.list[0].main.temp;
        //var icon = response.list[0].main.icon;
       // Pebble.sendAppMessage({ "icon":icon, "temperature":temperature + "\u00B0C"});
      } else { console.log("Error"); }
    }
  };
  req.send(null);
}
// Called when JS is ready
Pebble.addEventListener("ready",
							function(e) {
                console.log("ready! from JS");
							});
												
// Called when incoming message from the Pebble is received
Pebble.addEventListener("appmessage",
							function(e) {
								//console.log("X: " + e.payload.accel_x_data + ", Y: " + e.payload.accel_y_data + ", Z: " + e.payload.accel_z_data);
                console.log("appmessage received " + e.payload.lifts);
                test1(e.payload.lifts);
							});
