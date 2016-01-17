/**
 * Created by vovan on 1/17/16.
 */


var VERBOSE = false;

function logger (msg) {

    if (VERBOSE) {
        console.log(msg);
    }
}


function Cached (key, timeout) {

    var tsKey = key + 'Ts';

    return {
        get: function () {
            var lastTs = parseInt(localStorage.getItem(tsKey)),
                currentTs = new Date().getTime();

            if (!isNaN(lastTs) && lastTs + timeout > currentTs) {
                var value = localStorage.getItem(key);
                logger('Cached information for key ' + key + ' is up to date: ' + value);
                return value;
            } {
                if(!isNaN(lastTs)) {
                    var outOfDateSec = (currentTs - (lastTs + timeout)) / 1000;
                    logger('Cached information for key ' + key + ' is out of date for ' +
                        outOfDateSec + ' seconds');
                    this.clear();
                } else {
                    logger('Cached information for key ' + key + ' is out of date.');
                }
            }

            return null;
        },

        set: function (value) {
            localStorage.setItem(tsKey, new Date().getTime().toString());
            localStorage.setItem(key, value);
        },

        clear: function() {
            localStorage.removeItem(tsKey);
            localStorage.removeItem(key);
        }
    };

}


function sendMessageWithRetries (msg, maxRetries) {

    maxRetries = maxRetries || 3;

    var _sendWithRetries = function (retryNum) {

        if (retryNum === undefined) {
            retryNum = 0;
        }

        if (retryNum >= maxRetries) {
            logger('Error: could not send app message to the frontend three times. Giving up.');
            return;
        }

        Pebble.sendAppMessage(msg,
            function (obj) {},
            function (obj) {
                logger('Could not send app message. Error: ' + JSON.stringify(obj));
                setTimeout(function () {
                    _sendWithRetries(retryNum + 1);
                }, 1000);
            }
        );
    };

    _sendWithRetries();
}
