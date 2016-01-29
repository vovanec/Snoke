/*

 The MIT License (MIT)

 Copyright © 2016 Volodymyr Kuznetsov

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

var VERBOSE = false,
    USE_CACHE = true;


function logger (msg) {

    if (VERBOSE) {
        console.log(msg);
    }
}


function Cached (key, timeout) {

    var tsKey = key + 'Ts';

    return {

        get: function () {

            if(!USE_CACHE) {
                return null;
            }

            var lastTs = parseInt(localStorage.getItem(tsKey)),
                currentTs = new Date().getTime();

            if (!isNaN(lastTs) && lastTs + timeout > currentTs) {
                var value = localStorage.getItem(key),
                    valueAge = (currentTs - lastTs) / 1000;

                logger('Cached information for key ' + key + ' is up to date: `' +
                       value + '`, age: ' + valueAge +'seconds.');

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

            if(!USE_CACHE) {
                return;
            }

            localStorage.setItem(tsKey, new Date().getTime().toString());
            localStorage.setItem(key, value);
        },

        clear: function() {

            if(!USE_CACHE) {
                return;
            }

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


function getURL (url) {

    var req = new XMLHttpRequest(),
        p = new Promise();

    req.open('GET', url, true);

    req.onload = function () {
        if (req.readyState == 4) {
            if (req.status == 200) {
                p.resolve(req);
            } else {
                p.reject(req);
            }
        }
    };

    req.send(null);

    return p;
}
