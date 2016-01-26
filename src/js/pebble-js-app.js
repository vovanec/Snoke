
var CONFIG_URL = 'http://vkuznet-cf-test.s3-website-us-west-2.amazonaws.com',
    WEATHER_URL = 'https://6c3md7anye.execute-api.us-west-2.amazonaws.com/prod/weather';

var gConfig = {},
    gCachedStockInfo = Cached('cachedStockInfo', 60 * 60 * 1000),
    gCachedWeatcherInfo = Cached('cachedWeatherInfo', 10 * 60 * 1000);


var saveConfig = function (config) {

    localStorage.setItem('stockSymbol', config.stockSymbol);
    localStorage.setItem('temperatureUnits', config.temperatureUnits);

    loadConfig();
};


var objToURLParams = function (obj) {

    var queryParams = [];

    for (var name in obj) {
        if (obj.hasOwnProperty(name)) {
            var objVal = obj[name];
            if (typeof objVal === 'string') {
                objVal = objVal.replace(/^\s+|\s+$/g, '');
            }
            queryParams.push(name + '=' + objVal);
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


var sendMessage = function (status, message_type, message) {
    var msg = {
        'status': status,
        'message_type': message_type,
        'message': message
    };
    logger('Sending message to the frontend: ' + JSON.stringify(msg));

    sendMessageWithRetries(msg);
};


var fetchWeather = function (latitude, longitude) {

    var url = WEATHER_URL + '?lat=' + latitude + '&long=' + longitude,
        p = new Promise();

    logger('Fetching weather information, URL ' + url);

    getURL(url).then(
        function (reqObj) {

            try {
                var jsonResp = JSON.parse(reqObj.response);
                if (!jsonResp.success) {
                    return p.reject('Error: unsuccessful response from server: ' + JSON.stringify(jsonResp));
                }

                var respData = jsonResp.data,
                    temp = Math.round(respData.temp),
                    tempStr = temp + 'C.';

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

    var cachedInfo = gCachedWeatcherInfo.get(),
        p = new Promise();

    if (cachedInfo) {
        return p.resolve(cachedInfo);
    }

    logger('Querying device geo location services.');

    navigator.geolocation.getCurrentPosition(
        function (pos) {
            var lat = pos.coords.latitude, //.toFixed(6),
                lon = pos.coords.longitude; //.toFixed(6);

            fetchWeather(lat, lon).then(
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

    var stockSymbol = gConfig.stockSymbol,
        p = new Promise();

    if (stockSymbol) {

        var cachedInfo = gCachedStockInfo.get();
        if (cachedInfo) {
            return p.resolve(cachedInfo);
        }

        stockSymbol = stockSymbol.toUpperCase();
        var url = 'http://download.finance.yahoo.com/d/quotes.csv?s=' +
            encodeURI(stockSymbol) + '&f=price';

        logger('Fetching stocks information, URL ' + url);

        getURL(url).then(
            function (reqObj) {
                try {
                    var price = reqObj.responseText.split(',')[0],
                        stockInfo = stockSymbol + ': ' + price;

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


var apiFunctions = [getStocksInfo,   // MSG_TYPE_STOCKS = 0
                    getWeatherInfo]; // MSG_TYPE_WEATHER = 1


var callApi = function (apiFunc, appMsgType) {

    apiFunc().then(
        function (resp) {
            if (resp) {
                sendMessage(0, appMsgType, resp);
            } else {
                console.log('Empty reply from server.');
            }
        }, console.log);
};

var callAllApis = Array.prototype.forEach.bind(apiFunctions, callApi);


Pebble.addEventListener('appmessage', function (e) {

    //logger('Received app message: ' + JSON.stringify(e));

    var appMsgType = e.payload.message_type,
        apiFunc = apiFunctions[appMsgType];

    if (apiFunc) {
        callApi(apiFunc, appMsgType);
    } else {
        logger('Error: unknown message type received: ' + appMsgType);
    }
});


Pebble.addEventListener('ready', function () {

    logger('Device backend is ready.');

    loadConfig();

    callAllApis();
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

        gCachedStockInfo.clear();
        gCachedWeatcherInfo.clear();

        callAllApis();
    }
});
