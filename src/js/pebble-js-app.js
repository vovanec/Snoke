

var MSG_TYPE_STOCKS = 0;
var MSG_TYPE_WEATHER = 1;


var sendMessageWithRetries = function (msg, retryNum) {

    if (retryNum === undefined) {
        retryNum = 0;
    }

    if (retryNum >= 3) {
        console.log('Error: could not send app message to the frontend three times. Giving up.');
        return;
    }

    Pebble.sendAppMessage(msg,
        function (obj) {
            //console.log('Success: ' + JSON.stringify(obj));
        },
        function (obj) {
            console.log('Error: ' + JSON.stringify(obj));
            setTimeout(function () {
                sendMessageWithRetries(msg, retryNum + 1);
            }, 500);
        });
};


var sendMessage = function (status, message_type, message) {
    var msg = {
        'status': status,
        'message_type': message_type,
        'message': message
    };
    console.log('Sending message to the frontend: ' + JSON.stringify(msg));

    sendMessageWithRetries(msg);

    // PRO TIP: If you are sending more than one message, or a complex set of messages,
    // it is important that you setup an ackHandler and a nackHandler and call
    // Pebble.sendAppMessage({ /* Message here */ }, ackHandler, nackHandler), which
    // will designate the ackHandler and nackHandler that will be called upon the Pebble
    // ack-ing or nack-ing the message you just sent. The specified nackHandler will
    // also be called if your message send attempt times out.
};

var queryWeb = function (url, on_success, on_error) {

    var req = new XMLHttpRequest();
    req.open('GET', url, true);

    req.onload = function (e) {
        if (req.readyState == 4) {
            if (req.status == 200) {
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


var fetchWeather = function (latitude, longitude, callback) {
    var url = 'https://6c3md7anye.execute-api.us-west-2.amazonaws.com/prod/weather?' +
              'lat=' + latitude + '&long=' + longitude;

    console.log("Fetching weather information, URL " + url);

    queryWeb(url,
        function (reqObj) {
            var weatherStr;

            //console.log('Weather info: ' + JSON.stringify(reqObj));
            try {
                var jsonResp = JSON.parse(reqObj.response);
                if (!jsonResp.success) {
                    console.log('Unsuccessful response from server');
                    return;
                }

                var respData = jsonResp.data;

                var temp = Math.round(respData.temp);
                var city = respData.city;

                weatherStr = city + ', ' + temp + 'C.';
            } catch (err) {
                console.log('Could not parse server response: ' + err);
                return;
            }

            if (callback) {
                callback(weatherStr);
            }

        },
        function (reqObj) {
            console.log('Error(' + reqObj.status + '): ' + reqObj.responseText);
        });
};

var getWeatherInfo = function (callback) {
    console.log('Querying device geolocation services.');

    navigator.geolocation.getCurrentPosition(
        function (pos) {
            var coordinates = pos.coords;
            fetchWeather(coordinates.latitude, coordinates.longitude, callback);
        },
        function (err) {
            console.log('Could not query geolocation services: ' + JSON.stringify(err));
        },
        {'timeout': 15000, 'maximumAge': 60000, 'enableHighAccuracy': true});
};


var getStocksInfo = function (callback) {

    var url = 'http://download.finance.yahoo.com/d/quotes.csv?s=jnpr&f=price';
    console.log("Fetching stocks information, URL " + url);

    queryWeb(url,
        function (reqObj) {
            //console.log('Received service response: ' + JSON.stringify(reqObj));
            try {
                callback(reqObj.responseText.split(',')[0]);
            } catch (err) {
                console.log('Could not parse server response: ' + err);
            }
        },
        function (reqObj) {
            console.log('Error(' + reqObj.status + '): ' + reqObj.responseText);
        });
};


Pebble.addEventListener("appmessage",
    function (e) {
        console.log("Received app message: " + JSON.stringify(e));

        var msg_type = e.payload.message_type;

        if (msg_type === MSG_TYPE_STOCKS) {
            getStocksInfo(function (resp) {
                sendMessage(0, MSG_TYPE_STOCKS, resp);
            });
        } else if (msg_type === MSG_TYPE_WEATHER) {
            getWeatherInfo(function (resp) {
                sendMessage(0, MSG_TYPE_WEATHER, resp);
            });
        } else {
            console.log('Error: unknown message type received: ' + msg_type);
        }
    });


Pebble.addEventListener("ready",
    function (e) {
        console.log("Device backend is ready.");

        getWeatherInfo(function (resp) {
            sendMessage(0, MSG_TYPE_WEATHER, resp);
        });

        getStocksInfo(function (resp) {
            sendMessage(0, MSG_TYPE_STOCKS, resp);
        });

    });
