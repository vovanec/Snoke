// Function to send a message to the Pebble using AppMessage API
function sendMessage(status, message) {
  Pebble.sendAppMessage({'status': status, 'message': message});
	
	// PRO TIP: If you are sending more than one message, or a complex set of messages, 
	// it is important that you setup an ackHandler and a nackHandler and call 
	// Pebble.sendAppMessage({ /* Message here */ }, ackHandler, nackHandler), which 
	// will designate the ackHandler and nackHandler that will be called upon the Pebble 
	// ack-ing or nack-ing the message you just sent. The specified nackHandler will 
	// also be called if your message send attempt times out.
}


// Called when JS is ready
Pebble.addEventListener("ready",
							function(e) {
                console.log("JS is ready!!!");
							});

var MSG_TYPE_STOCKS = 0;

var queryWeb = function (url, on_success, on_error) {
  
  var req = new XMLHttpRequest();
  req.open('GET', url, true);
  
  req.onload = function(e) {
    if (req.readyState == 4 && req.status == 200) {
      if(req.status == 200) {
        if (on_success) {
          on_success(req);
        }
      } else {
        if (on_error) {
          on_error(req);
        }
      }
    }
  };
  req.send(null);
};


var getStocksInfo = function (callback) {
  queryWeb('http://download.finance.yahoo.com/d/quotes.csv?s=jnpr&f=price',
          function(resp){
            console.log('Received service response: ' + JSON.stringify(resp));
            try {
              callback(resp.responseText.split(',')[0]);
            } catch(err) {
              console.log('Could not parse server response: ' + err);
            }
          },
          function(resp){
            console.log('Error(' + resp.status + '): ' + resp.responseText);
          });
};

// Called when incoming message from the Pebble is received
Pebble.addEventListener("appmessage",
							function(e) {
								console.log("Received app message: " + JSON.stringify(e));
                var msg_type = e.payload.message;
                if (msg_type === MSG_TYPE_STOCKS) {
                  getStocksInfo(function(resp) {
    								sendMessage(0, resp);                   
                  });
                }
							});