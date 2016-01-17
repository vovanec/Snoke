
var getParameterByName = function (url, name) {

    name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
    var regex = new RegExp("[\\?&]" + name + "=([^&#]*)"),
        results = regex.exec(url);
    return results === null ? "" : decodeURIComponent(results[1].replace(/\+/g, " "));
};


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
        $temperatureUnits = $('#temperatureUnits'),
        url = document.location.href,
        stockSymbol = getParameterByName(url, 'stockSymbol') || 'GOOG',
        temperatureUnits = getParameterByName(url, 'temperatureUnits') || 'f';

    $stockSymbol.val(stockSymbol.toUpperCase());
    $temperatureUnits.val(temperatureUnits);
};


var getAndStoreConfigData = function () {

    var $stockSymbol = $('#stockSymbol'),
        $temperatureUnits = $('#temperatureUnits'),
        stockSymbolStr = $stockSymbol.val().replace(/^\s+|\s+$/g, '').toUpperCase();

    return {
        stockSymbol: stockSymbolStr || null,
        temperatureUnits: $temperatureUnits.val()
    };
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