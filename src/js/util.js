/**
 * Created by vovan on 1/17/16.
 */


var VERBOSE = true;

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
                var outOfDateSec = (currentTs - (lastTs + timeout)) / 1000;
                logger('Cached information for key ' + key + ' is out of date for ' +
                    outOfDateSec + ' seconds');
                this.clear();
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
