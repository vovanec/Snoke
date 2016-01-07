// Function to send a message to the Pebble using AppMessage API
function sendMessage(status, message_type, message) {
  Pebble.sendAppMessage({'status': status,
                         'message_type': message_type,
                         'message': message});
	
	// PRO TIP: If you are sending more than one message, or a complex set of messages, 
	// it is important that you setup an ackHandler and a nackHandler and call 
	// Pebble.sendAppMessage({ /* Message here */ }, ackHandler, nackHandler), which 
	// will designate the ackHandler and nackHandler that will be called upon the Pebble 
	// ack-ing or nack-ing the message you just sent. The specified nackHandler will 
	// also be called if your message send attempt times out.
}

var locationOptions = {"timeout": 15000,
                       "maximumAge": 60000}; 
var MSG_TYPE_STOCKS = 0;

var queryWeb = function (url, on_success, on_error) {
  
  var req = new XMLHttpRequest();
  req.open('GET', url, true);
  
  req.onload = function(e) {
    if (req.readyState == 4) {
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

function fetchWeather(latitude, longitude) {
  var url = "http://www.mirz.com/Chunk2/Yahoo.php?lat=" + latitude + "&long=" + longitude + "&v=24&units=c";
  console.log("Fetching Weather, URL " + url);
  
  queryWeb(url,
           function(reqObj) {
             console.log('Weather info: ' + JSON.stringify(reqObj));
           },
           function(reqObj) {
             console.log('Error(' + reqObj.status + '): ' + reqObj.responseText);
           }
  );
}



var locationSuccess = function (pos) {
    var coordinates = pos.coords;
    fetchWeather(coordinates.latitude, coordinates.longitude);
};

var locationError = function (err) {
  console.log('Could not query location services: ' + JSON.stringify(err));
};

function getWeatherInfo() {
  console.log('Querying location services.');
  navigator.geolocation.getCurrentPosition(locationSuccess,
                                           locationError,
                                           locationOptions);		
}


var getStocksInfo = function (callback) {
  console.log('Querying for latest stock prices.');
  
  queryWeb('http://download.finance.yahoo.com/d/quotes.csv?s=jnpr&f=price',
          function(reqObj){
            console.log('Received service response: ' + JSON.stringify(reqObj));
            try {
              callback(reqObj.responseText.split(',')[0]);
            } catch(err) {
              console.log('Could not parse server response: ' + err);
            }
          },
          function(reqObj){
            console.log('Error(' + reqObj.status + '): ' + reqObj.responseText);
          });
};


// Called when incoming message from the Pebble is received
Pebble.addEventListener("appmessage",
							function(e) {
								console.log("Received app message: " + JSON.stringify(e));
                var msg_type = e.payload.message_type;
                
                if (msg_type === MSG_TYPE_STOCKS) {
                  getStocksInfo(function(resp) {
    								sendMessage(0, MSG_TYPE_STOCKS, resp);                   
                  });
                  
                } else {
                  console.log('Error: unknown message type received: ' + msg_type);
                }
							});


// Called when JS is ready
Pebble.addEventListener("ready",
							function(e) {
                console.log("JS is ready!!!");
                
                getWeatherInfo();
                
                getStocksInfo(function(resp) {
                  sendMessage(0, MSG_TYPE_STOCKS, resp);                   
                });                
                
							});