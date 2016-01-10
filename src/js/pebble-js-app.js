
var VERBOSE = false;
var MSG_TYPE_STOCKS = 0;
var MSG_TYPE_WEATHER = 1;


var CONFIG_URL = 'http://vkuznet-cf-test.s3-website-us-west-2.amazonaws.com';


var Config = {stockSymbol: localStorage.snokeStockSymbol || 'GOOG',
              temperatureUnits: localStorage.snokeTemperatureUnits || 'f'};


var logger = function(msg) {

    if (VERBOSE) {
        console.log(msg);
    }

};


var sendMessageWithRetries = function (msg, retryNum) {

    if (retryNum === undefined) {
        retryNum = 0;
    }

    if (retryNum >= 3) {
        logger('Error: could not send app message to the frontend three times. Giving up.');
        return;
    }

    Pebble.sendAppMessage(msg,
        function (obj) {
            //logger('Success: ' + JSON.stringify(obj));
        },
        function (obj) {
            logger('Error: ' + JSON.stringify(obj));
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
    logger('Sending message to the frontend: ' + JSON.stringify(msg));

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

    logger('Fetching weather information, URL ' + url);

    queryWeb(url,
        function (reqObj) {
            var weatherStr;

            //logger('Weather info: ' + JSON.stringify(reqObj));
            try {
                var jsonResp = JSON.parse(reqObj.response);
                if (!jsonResp.success) {
                    logger('Unsuccessful response from server');
                    return;
                }

                var respData = jsonResp.data;

                var temp = Math.round(respData.temp);

                var tempStr = temp + 'C.';
                if (Config.temperatureUnits === 'f') {
                    temp = temp * 9/5 + 32;
                    tempStr = temp + 'F.';
                }

                weatherStr = respData.city + ', ' + tempStr;
            } catch (err) {
                logger('Could not parse server response: ' + err);
                return;
            }

            if (callback) {
                callback(weatherStr);
            }

        },
        function (reqObj) {
            logger('Error(' + reqObj.status + '): ' + reqObj.responseText);
        });
};

var getWeatherInfo = function (callback) {

    logger('Querying device geo location services.');

    navigator.geolocation.getCurrentPosition(
        function (pos) {
            var coordinates = pos.coords;
            fetchWeather(coordinates.latitude, coordinates.longitude, callback);
        },
        function (err) {
            logger('Could not query geo location services: ' + JSON.stringify(err));
        },
        {'timeout': 15000, 'maximumAge': 60000, 'enableHighAccuracy': true});
};


var getStocksInfo = function (callback) {

    if (Config.stockSymbol) {

        var stockSymbol = Config.stockSymbol.toUpperCase();
        var url = 'http://download.finance.yahoo.com/d/quotes.csv?s=' +
            encodeURI(stockSymbol) + '&f=price';

        logger('Fetching stocks information, URL ' + url);

        queryWeb(url,
            function (reqObj) {
                //logger('Received service response: ' + JSON.stringify(reqObj));
                try {
                    var price = reqObj.responseText.split(',')[0];
                    callback(stockSymbol + ': ' + price);
                } catch (err) {
                    logger('Could not parse server response: ' + err);
                }
            },
            function (reqObj) {
                logger('Error(' + reqObj.status + '): ' + reqObj.responseText);
            });
    } else {
        logger('Company stock symbol is not configured.');
    }
};


Pebble.addEventListener('appmessage',

    function (e) {
        logger('Received app message: ' + JSON.stringify(e));

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
            logger('Error: unknown message type received: ' + msg_type);
        }
    });


Pebble.addEventListener('ready',

    function (e) {
        logger('Device backend is ready.');

        getWeatherInfo(function (resp) {
            sendMessage(0, MSG_TYPE_WEATHER, resp);
        });

        getStocksInfo(function (resp) {
            sendMessage(0, MSG_TYPE_STOCKS, resp);
        });

    });


Pebble.addEventListener('showConfiguration', function() {

    logger('Showing configuration page: ' + CONFIG_URL);

    Pebble.openURL(CONFIG_URL);
});


Pebble.addEventListener('webviewclosed', function(e) {

    if (e.response) {

        var configData = JSON.parse(decodeURIComponent(e.response));
        logger('Configuration page returned: ' + JSON.stringify(configData));

        Config.stockSymbol = configData.stockSymbol;
        Config.temperatureUnits = configData.temperatureUnits;

        getWeatherInfo(function (resp) {
            sendMessage(0, MSG_TYPE_WEATHER, resp);
        });

        getStocksInfo(function (resp) {
            sendMessage(0, MSG_TYPE_STOCKS, resp);
        });
    }
});
