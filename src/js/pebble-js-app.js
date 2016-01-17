
var MSG_TYPE_STOCKS = 0;
var MSG_TYPE_WEATHER = 1;

var CONFIG_URL = 'http://vkuznet-cf-test.s3-website-us-west-2.amazonaws.com';
var WEATHER_URL = 'https://6c3md7anye.execute-api.us-west-2.amazonaws.com/prod/weather';

var gConfig = {};
var gCachedStockInfo = Cached('cachedStockInfo', 60 * 60 * 1000);
var gCachedWeatcherInfo = Cached('cachedWeatherInfo', 20 * 60 * 1000);


var saveConfig = function (config) {

    localStorage.setItem('stockSymbol', config.stockSymbol);
    localStorage.setItem('temperatureUnits', config.temperatureUnits);

    loadConfig();
};


var objToURLParams = function (obj) {

    var queryParams = [];

    for (var name in obj) {
        if (obj.hasOwnProperty(name)) {
            queryParams.push(name + '=' + obj[name]);
        }
    }

    return queryParams.join('&');
};


var loadConfig = function () {

    gConfig.stockSymbol = localStorage.getItem('stockSymbol');
    gConfig.temperatureUnits = localStorage.getItem('temperatureUnits');

    if(!gConfig.stockSymbol || gConfig.stockSymbol == 'undefined') {
        gConfig.stockSymbol = 'GOOG';
    }

    if(!gConfig.temperatureUnits || gConfig.temperatureUnits == 'undefined') {
        gConfig.temperatureUnits = 'f';
    }

    logger('Loaded local config: ' + JSON.stringify(gConfig));

    return gConfig;
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

    req.onload = function () {
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


var fetchWeather = function (latitude, longitude) {
    
    var url = WEATHER_URL + '?lat=' + latitude + '&long=' + longitude;

    logger('Fetching weather information, URL ' + url);

    var p = new Promise();

    queryWeb(url,
        function (reqObj) {

            //logger('Weather info: ' + JSON.stringify(reqObj));
            try {
                var jsonResp = JSON.parse(reqObj.response);
                if (!jsonResp.success) {
                    return p.reject('Error: unsuccessful response from server');
                }

                var respData = jsonResp.data;

                var temp = Math.round(respData.temp);

                var tempStr = temp + 'C.';
                if (gConfig.temperatureUnits === 'f') {
                    temp = temp * 9/5 + 32;
                    tempStr = temp + 'F.';
                }

                p.resolve(respData.city + ', ' + tempStr);
            } catch (err) {
                p.reject('Error - could not parse server response: ' + err);
            }
        },
        function (reqObj) {
            p.reject('Error(' + reqObj.status + '): ' + reqObj.responseText);
        });

    return p;
};


var getWeatherInfo = function () {

    var p = new Promise();

    var cachedInfo = gCachedWeatcherInfo.get();
    if (cachedInfo) {
        return p.resolve(cachedInfo);
    }

    logger('Cached weather information is out of date. Querying device geo location services.');

    navigator.geolocation.getCurrentPosition(
        function (pos) {
            var coordinates = pos.coords;
            fetchWeather(coordinates.latitude, coordinates.longitude).then(
                function (weatherStr) {
                    gCachedWeatcherInfo.set(weatherStr);
                    p.resolve(weatherStr);
                }, console.log);
        },
        function (err) {
            p.reject('Could not query geo location services: ' + JSON.stringify(err));
        },
        {'timeout': 15000, 'maximumAge': 60000, 'enableHighAccuracy': true});

    return p;
};


var getStocksInfo = function () {

    var p = new Promise();

    var stockSymbol = gConfig.stockSymbol;

    if (stockSymbol) {

        var cachedInfo = gCachedStockInfo.get();
        if (cachedInfo) {
            return p.resolve(cachedInfo);
        }

        stockSymbol = stockSymbol.toUpperCase();
        var url = 'http://download.finance.yahoo.com/d/quotes.csv?s=' +
            encodeURI(stockSymbol) + '&f=price';

        logger('Cached stock information is out of date. Fetching stocks information, URL ' + url);

        queryWeb(url,
            function (reqObj) {
                //logger('Received service response: ' + JSON.stringify(reqObj));
                try {
                    var price = reqObj.responseText.split(',')[0];
                    var stockInfo = stockSymbol + ': ' + price;
                    gCachedStockInfo.set(stockInfo);

                    p.resolve(stockInfo);
                } catch (err) {
                    p.reject('Error - Could not parse server response: ' + err);
                }
            },
            function (reqObj) {
                p.reject('Error(' + reqObj.status + '): ' + reqObj.responseText);
            });
    } else {
        logger('Company stock symbol is not configured.');

        p.resolve(null);
    }

    return p;
};


Pebble.addEventListener('appmessage', function (e) {

        //logger('Received app message: ' + JSON.stringify(e));

        var msg_type = e.payload.message_type;

        if (msg_type === MSG_TYPE_STOCKS) {
            getStocksInfo().then(
                function (resp) {
                    if (resp) sendMessage(0, MSG_TYPE_STOCKS, resp);
                }, console.log);
        } else if (msg_type === MSG_TYPE_WEATHER) {
            getWeatherInfo().then(function (resp) {
                sendMessage(0, MSG_TYPE_WEATHER, resp);
            }, console.log);
        } else {
            logger('Error: unknown message type received: ' + msg_type);
        }
    });


Pebble.addEventListener('ready', function () {

    logger('Device backend is ready.');

    loadConfig();

    getWeatherInfo().then(function (resp) {
        sendMessage(0, MSG_TYPE_WEATHER, resp);
    }, console.log);

    getStocksInfo().then(
        function (resp) {
            if (resp) sendMessage(0, MSG_TYPE_STOCKS, resp);
        }, console.log);

});


Pebble.addEventListener('showConfiguration', function() {

    var fullURL = CONFIG_URL + '?' + objToURLParams(loadConfig());

    logger('Showing configuration page: ' + fullURL);

    Pebble.openURL(fullURL);
});


Pebble.addEventListener('webviewclosed', function(e) {

    if (e.response) {
        var configData = JSON.parse(decodeURIComponent(e.response));
        logger('Configuration data returned: ' + JSON.stringify(configData));

        saveConfig(configData);

        getWeatherInfo().then(function (resp) {
            sendMessage(0, MSG_TYPE_WEATHER, resp);
        }, console.log);

        getStocksInfo().then(
            function (resp) {
                if (resp) sendMessage(0, MSG_TYPE_STOCKS, resp);
            }, console.log);
    }
});
