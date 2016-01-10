
var submitHandler = function () {

    var $submitButton = $('#submitButton');

    $submitButton.on('click', function () {

        var configData = JSON.stringify(getAndStoreConfigData());
        var return_to = getQueryParam('return_to', 'pebblejs://close#');

        document.location = return_to + encodeURIComponent(configData);
    });
};


var loadOptions = function () {

    var $stockSymbol = $('#stockSymbol'),
        $temperatureUnits = $('#temperatureUnits');

    if (localStorage.snokeStockSymbol) {
        var stockSymbol = localStorage.snokeStockSymbol.toUpperCase();
        $stockSymbol.val(stockSymbol);
    }

    if (localStorage.snokeTemperatureUnits) {
        $temperatureUnits.val(localStorage.snokeTemperatureUnits);
    }
};


var getAndStoreConfigData = function () {

    var $stockSymbol = $('#stockSymbol'),
        $temperatureUnits = $('#temperatureUnits');

    var stockSymbolStr = $stockSymbol.val().toUpperCase();
    var options = {
        stockSymbol: stockSymbolStr || null,
        temperatureUnits: $temperatureUnits.val()
    };

    localStorage.snokeStockSymbol = stockSymbolStr;
    localStorage.snokeTemperatureUnits = options.temperatureUnits;

    return options;
};


var getQueryParam = function (variable, defaultValue) {

    var query = location.search.substring(1);
    var vars = query.split('&');
    for (var i = 0; i < vars.length; i++) {
        var pair = vars[i].split('=');
        if (pair[0] === variable) {
            return decodeURIComponent(pair[1]);
        }
    }
    return defaultValue || false;
};


(function () {
    loadOptions();
    submitHandler();
})();