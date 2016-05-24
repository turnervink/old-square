/*
* KiezelPay Integration Library - v1.2 - Copyright Kiezel 2016
*
* BECAUSE THE LIBRARY IS LICENSED FREE OF CHARGE, THERE IS NO 
* WARRANTY FOR THE LIBRARY, TO THE EXTENT PERMITTED BY APPLICABLE 
* LAW. EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT 
* HOLDERS AND/OR OTHER PARTIES PROVIDE THE LIBRARY "AS IS" 
* WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, 
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE
* RISK AS TO THE QUALITY AND PERFORMANCE OF THE LIBRARY IS WITH YOU.
* SHOULD THE LIBRARY PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL 
* NECESSARY SERVICING, REPAIR OR CORRECTION.
* 
* IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN 
* WRITING WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY 
* MODIFY AND/OR REDISTRIBUTE THE LIBRARY AS PERMITTED ABOVE, BE 
* LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL, 
* INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR 
* INABILITY TO USE THE LIBRARY (INCLUDING BUT NOT LIMITED TO LOSS
* OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY 
* YOU OR THIRD PARTIES OR A FAILURE OF THE LIBRARY TO OPERATE WITH
* ANY OTHER SOFTWARE), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN 
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*/

/* === KIEZELPAY === SET TO false BEFORE RELEASING === */
var KIEZELPAY_LOGGING = true;
/* === KIEZELPAY === SET TO false BEFORE RELEASING === */


/**********************************************************************/
/* === KIEZELPAY === GENERATED CODE BEGIN === DO NOT MODIFY BELOW === */
/**********************************************************************/
var kiezelPayAppMessageHandler = null;

function kiezelPayInit(appMessageHandler) {
  kiezelPayLog("kiezelPayInit() called");
  kiezelPayAppMessageHandler = appMessageHandler;
  
  var msg = {
    "KIEZELPAY_READY": 1
  };
  Pebble.sendAppMessage(msg, 
                        function(e) {
                          kiezelPayLog("KiezelPay Ready msg successfully sent.");
                        },
                        function(e) {
                          kiezelPayLog("KiezelPay Ready msg failed.");
                        });
}

function kiezelPayLog(msg) {
  if (KIEZELPAY_LOGGING) {
    console.log(msg);
  }
}

var xhrRequest = function (url, type, callback, errorCallback, timeout) {
  try {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
      callback(this.responseText);
    };
    xhr.open(type, url);
    if (timeout) {
      xhr.timeout = timeout;
    }
    xhr.send();
  }
  catch (ex) {
    if (errorCallback) {
      errorCallback(ex);
    }
  }
};

function kiezelPayOnAppMessage(appmsg) {
  if (appmsg && appmsg.payload && 
      appmsg.payload.hasOwnProperty('KIEZELPAY_STATUS_CHECK') && 
      appmsg.payload.KIEZELPAY_STATUS_CHECK > 0) {
    //its a status check message, handle it
    var devicetoken = appmsg.payload.KIEZELPAY_DEVICE_TOKEN;
    var appId = appmsg.payload.KIEZELPAY_APP_ID;
    var random = appmsg.payload.KIEZELPAY_RANDOM;
    var accounttoken = Pebble.getAccountToken();
    var platform = getPlatform();
    
    //build url
    var kiezelpayStatusUrl = 'https://www.kiezelpay.com/api/v1/status?';
    kiezelpayStatusUrl += 'appid=' + encodeURIComponent(appId);
    kiezelpayStatusUrl += '&devicetoken=' + encodeURIComponent(devicetoken);
    kiezelpayStatusUrl += '&rand=' + encodeURIComponent(random);
    kiezelpayStatusUrl += '&accounttoken=' + encodeURIComponent(accounttoken);
    kiezelpayStatusUrl += '&platform=' + encodeURIComponent(platform);
    kiezelpayStatusUrl += '&flags=' + encodeURIComponent(appmsg.payload.KIEZELPAY_STATUS_CHECK);
    kiezelpayStatusUrl += '&nocache=' + encodeURIComponent(Math.round(new Date().getTime()));
    kiezelPayLog(kiezelpayStatusUrl);
    
    //perform the request
    xhrRequest(kiezelpayStatusUrl, "GET", 
      function(responseText) {
        kiezelPayLog("KiezelPay status response: " + responseText);
        var response = JSON.parse(responseText);
        var status = 0;
        var trialDuration = 0;
        var paymentCode = 0;
        var purchaseStatus = 0;
        if (response.status === 'unlicensed') {
          status = 0;
          paymentCode = Number(response.paymentCode);
          if (response.purchaseStatus == 'waitForUser') {
            purchaseStatus = 0;
          }
          else if (response.purchaseStatus == 'inProgress') {
            purchaseStatus = 1;
          }
        } else if (response.status == 'trial') {
          status = 1;
          trialDuration = Number(response.trialDurationInSeconds);
        } else if (response.status == 'licensed') {
          status = 2;
        }

        var msg = {
          KIEZELPAY_STATUS_RESULT: status,
          KIEZELPAY_STATUS_TRIAL_DURATION: trialDuration,
          KIEZELPAY_PURCHASE_CODE: paymentCode,
          KIEZELPAY_PURCHASE_STATUS: purchaseStatus,
          KIEZELPAY_STATUS_VALIDITY_PERIOD: response.validityPeriodInDays,
          KIEZELPAY_STATUS_CHECKSUM: kiezelpay_toByteArray(response.checksum)
        };
        kiezelPayLog("KiezelPay watch status msg: " + JSON.stringify(msg));
        Pebble.sendAppMessage(msg,
                              function (e) {
                                kiezelPayLog('KiezelPay status msg successfully sent to watch');
                              },
                              function (e) {
                                kiezelPayLog('KiezelPay status msg failed sending to watch');
                              });
      },
      function (error) {
        kiezelPayLog('KiezelPay status request failed: ' + JSON.stringify(error));
        kiezelpay_sendInternetFailedMsg();
      }, 5000);
    return true;    //its our message, we handled it
  }
  
  return false;    //not a kiezelpay message
}

function kiezelpay_sendInternetFailedMsg() {
  var msg = {
    KIEZELPAY_INTERNET_FAIL: 1
  };
  Pebble.sendAppMessage(msg,
                        function (e) {
                          kiezelPayLog('KiezelPay internet fail successfully sent');
                        },
                        function (e) {
                          kiezelPayLog('KiezelPay internet fail not sent');
                        });
}

function kiezelpay_toByteArray(hexStringValue) {
  var bytes = [];
  for (var i = 0; i < hexStringValue.length; i += 2) {
    bytes.push(parseInt(hexStringValue.substr(i, 2), 16));
  }
  return bytes;
}

function getPlatform() {
  if (Pebble.getActiveWatchInfo) {
    return Pebble.getActiveWatchInfo().platform;
  }
  else {
    return "aplite";
  }
}

var Clay = require("clay");
var clayConfig = require("config");
var customClay = require("custom-clay");
var clay = new Clay(clayConfig, customClay, {autoHandleEvents: false});
var premium = false;

Pebble.addEventListener("appmessage", function (e) {
  if (!kiezelPayOnAppMessage(e) && kiezelPayAppMessageHandler) {
    kiezelPayAppMessageHandler(e);
  }
	
	console.log('AppMessage received!');
	console.log('Message contents: ' + JSON.stringify(e.payload));

	var messageContents = e.payload;
	console.log(messageContents.KEY_CONDITIONS);

	if (messageContents.KEY_CONDITIONS === 0) { // If KEY_CONDITIONS exists in the appmessage
		console.log('KEY_CONDITIONS received in appmessage');
		getWeather(); // Fetch the weather
	}
	
	if (messageContents.KEY_PREMIUM_ACTIVATED === 1) {
		console.log("Marking premium as activated in JS");
		premium = true;
		localStorage.premium = premium;
	}
});

/********************************************************************/
/* === KIEZELPAY === GENERATED CODE END === DO NOT MODIFY ABOVE === */
/********************************************************************/

var lang; // Language code
var show_weather = 1;
var city = "";

Pebble.addEventListener("ready", function (e) {
  //init kiezelpay and register your own handler to receive appMessages
  kiezelPayInit(onAppMessageReceived);

  console.log('PebbleKit JS Ready!');

  Pebble.sendAppMessage({'15': 'ready'
	}, function(e) {
      console.log('Sent ready message');
		console.log(e);
  }, function(e) {
      console.log('Failed to send ready message');
			console.log(e.response);
  });
	
	lang = localStorage.lang;
	show_weather = localStorage.showWeather;
	city = localStorage.city;
	
	if (localStorage.getItem("premium") !== null) {
		console.log("Getting premium status from localStorage");
		premium = localStorage.premium;
	}
	
	console.log("Premium is " + premium);
	console.log("City is " + city);
	
});

function onAppMessageReceived(appMsg) {
   
}

// app.js //

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  // Construct URL
  var url;
  
	if (city !== "" && premium === true) {
		console.log("Using city to create URL");
		// Construct URL
  	url = 'http://api.openweathermap.org/data/2.5/weather?&q=' + city + '&appid=2874bea34ea1f91820fa07af69939eea&lang=' + lang;
	
		console.log('URL is ' + url);
	} else {
		console.log("Using pos to create URL");
		// Construct URL
		url = 'http://api.openweathermap.org/data/2.5/weather?lat=' + pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=2874bea34ea1f91820fa07af69939eea&lang=' + lang;

		console.log("Lat is " + pos.coords.latitude);
		console.log("Lon is " + pos.coords.longitude);
		console.log('URL is ' + url);
	}
	
  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      console.log("Parsing JSON");
      
      var json = JSON.parse(responseText); // Parse JSON response
      //console.log(JSON.parse(responseText));

      var temperature = Math.round(((json.main.temp - 273.15) * 1.8) + 32); // Convert from Kelvin to Fahrenheit
      console.log("Temperature in Fahrenheit is " + temperature);
      
      var temperaturec = Math.round(json.main.temp - 273.15); // Convert from Kelvin to Celsius
      console.log("Temperature in Celsius is " + temperaturec);

      // Conditions
      var conditions = json.weather[0].description;      
      console.log("Conditions are " + conditions);
      
      // Assemble weather info into dictionary
      var dictionary = {
        "KEY_TEMPERATURE": temperature,
        "KEY_TEMPERATURE_IN_C": temperaturec,
        "KEY_CONDITIONS": conditions,
      };

      // Send dictionary to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Weather info sent to Pebble successfully!");
					console.log(e);
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
					console.log(e);
        }
      );
    }      
  );
}

function locationError(err) {
  console.log('Error requesting location!');
}

function getWeather() {
	console.log("Getting weather");
	navigator.geolocation.getCurrentPosition(
		locationSuccess,
		locationError,
		{timeout: 15000, maximumAge: 60000}
	);
}

/*Pebble.addEventListener('appmessage',
  

    if (e.payload.langSel) { // If KEY_LANGUAGE exists in appmessage
      console.log('KEY_LANGUAGE recevied in appmessage');
      lang = messageContents.langSel; // Set lang to the value of KEY_LANGUAGE
			if (show_weather === 1) {
      	getWeather(); // Fetch the weather with new language
			}
    }

  }                     
);*/

//===== Config =====//

Pebble.addEventListener('showConfiguration', function(e) {
	console.log("Showing configuration page");
  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (e && !e.response) { 
		console.log("No response from config page!");
    return; 
  }
	
	console.log("Configuration page returned: " + e.response);

  // Get the keys and values from each config item
  var dict = clay.getSettings(e.response);
	//var response = JSON.parse(e.response);
	//city = response.cityName.value;
	
	console.log("Dict contents: " + dict);
	console.log("Selected language: " + dict.langSel);
	console.log("Weather display: " + dict.showWeather);
	
	// Get the language and weather display settings and save them
	if (premium === true) {
		city = dict.cityName.value;
		localStorage.city = city;
	}
	
	console.log("Entered city is: " + city);
	lang = dict.langSel;
	show_weather = dict.showWeather;
	
	localStorage.lang = lang;
	localStorage.show_weather = show_weather;

  // Send settings values to watch side
  Pebble.sendAppMessage(dict, 
		function(e) {
    	console.log("Config messaged ACK'd");
			
			if (city !== "") {
				getWeather();
			}
  	}, 
		function(e) {
    	console.log('Failed to send config data!');
  	}
	);
	
});

// ========== CONFIGURATION ========== //

/*Pebble.addEventListener('showConfiguration', function() {
  var url = 'http://turnervink.github.io/square-config/';

  console.log('Showing configuration page: ' + url);

  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  var configData = JSON.parse(decodeURIComponent(e.response));

  console.log('Configuration page returned: ' + JSON.stringify(configData));
  console.log('Background color is ' + parseInt(configData.textColor, 16));

  if (configData.textColor) { // If we have received the correct data (not sure why we wouldn't, but who knows?)
    // Send all keys to Pebble
		console.log("Sending config dict");
    Pebble.sendAppMessage({
      textColor: parseInt(configData.textColor, 16),
      backgroundColor: parseInt(configData.backgroundColor, 16),
      invertColors: configData.invertColors ? 1 : 0,
      showWeather: configData.showWeather ? 1 : 0,
      shakeWeather: configData.shakeWeather ? 1 : 0,
      useCelsius: configData.useCelsius ? 1 : 0,
      vibeDisconnect: configData.vibeDisconnect ? 1 : 0,
      vibeConnect: configData.vibeConnect ? 1 : 0,
      reflectBatt: configData.reflectBatt ? 1 : 0,
      dateFormat: configData.dateFormat,
      langSel: configData.langSel,
			largeFont: configData.largeFont ? 1 : 0
    }, function(e) {
      console.log('Send successful!');
			console.log(e);
    }, function(e) {
      console.log('Send failed!');
			console.log(e);
    });
  }
});*/