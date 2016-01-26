"""Lambda function for weather service.

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
"""

from __future__ import print_function

import json
import sys
import urllib2

REQUIRED_PARAMS = ('lat', 'long')
API_KEY = '__SOMETHING__'
URL_TMPL = 'http://api.openweathermap.org/data/2.5/weather' \
           '?lat=%(lat)s&lon=%(long)s&units=metric&APPID=' + API_KEY


print('Loading lambda function.')


def make_result(data):

    return {'success': True, 'data': data}


def make_error(message):

    return {'success': False, 'data': {'message': message}}


def convert_result_dict(api_resp):

    result = {}
    result.update(api_resp['main'])

    result['weather_short'] = api_resp['weather'][0]['main']
    result['weather_long'] = api_resp['weather'][0]['description']
    result['weather_icon'] = api_resp['weather'][0]['icon']

    result['city'] = api_resp['name']
    result['country'] = api_resp['sys']['country']
    result['latitude'] = api_resp['coord']['lat']
    result['longitude'] = api_resp['coord']['lon']
    result['sunset'] = api_resp['sys']['sunset']
    result['sunrise'] = api_resp['sys']['sunrise']
    result['wind_speed'] = api_resp['wind'].get('speed', None)
    result['wind_deg'] = api_resp['wind'].get('deg', None)

    return result


def query_api(url):

    data = urllib2.urlopen(url, timeout=10).read()
    return json.loads(data)


def parse_event(event):

    result = {}

    for param_name in REQUIRED_PARAMS:
        try:
            param_val = float(event.get(param_name))
        except (TypeError, ValueError):
            raise ValueError('Invalid parameter')

        result[param_name] = param_val

    return result


def lambda_handler(event, _):

    print('Received event: %s' % (event,))
    try:
        url = URL_TMPL % parse_event(event)
        print('Querying %s for weather info' % (url,))
        api_resp = query_api(url)
        print('Received API response: %s' % (api_resp,))
        return make_result(convert_result_dict(api_resp))
    except Exception as err:
        print('Exception: %s' % (err,))
        return {'success': False, 'data': {'message': str(err)}}
