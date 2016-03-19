var Clay = require("clay");
var clayConfig = require("config");
var customClay = require("custom-clay");
var clay = new Clay(clayConfig, customClay, {autoHandleEvents: false});

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

var lang; // Language code
var show_weather = 1;

function locationSuccess(pos) {
  // Construct URL
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' + pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=2874bea34ea1f91820fa07af69939eea&lang=' + lang;
  
  console.log("Lat is " + pos.coords.latitude);
  console.log("Lon is " + pos.coords.longitude);
  console.log('URL is ' + url);

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      console.log("Parsing JSON");
      
      var json = JSON.parse(responseText); // Parse JSON response
      console.log(JSON.parse(responseText));

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
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS Ready!');

  Pebble.sendAppMessage({'15': 'ready'
	}, function(e) {
      console.log('Sent ready message');
		console.log(e);
  }, function(e) {
      console.log('Failed to send ready message');
			console.log(e);
  });
	
	lang = localStorage.lang;
	show_weather = localStorage.showWeather;
});

Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
    console.log('Message contents: ' + JSON.stringify(e.payload));

    var messageContents = e.payload;
    console.log(messageContents.KEY_CONDITIONS);

    if (messageContents.KEY_CONDITIONS === 0) { // If KEY_CONDITIONS exists in the appmessage
      console.log('KEY_CONDITIONS received in appmessage');
      getWeather(); // Fetch the weather
    }

    /*if (e.payload.langSel) { // If KEY_LANGUAGE exists in appmessage
      console.log('KEY_LANGUAGE recevied in appmessage');
      lang = messageContents.langSel; // Set lang to the value of KEY_LANGUAGE
			if (show_weather === 1) {
      	getWeather(); // Fetch the weather with new language
			}
    }*/

  }                     
);

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
	
	console.log("Dict contents: " + dict);
	console.log("Selected language: " + dict.langSel);
	console.log("Weather display: " + dict.showWeather);
	
	// Get the language and weather display settings and save them
	lang = dict.langSel;
	show_weather = dict.showWeather;
	
	localStorage.lang = lang;
	localStorage.show_weather = show_weather;

  // Send settings values to watch side
  Pebble.sendAppMessage(dict, 
		function(e) {
    	console.log("Config messaged ACK'd");
			console.log("ACK: " + e);
  	}, 
		function(e) {
    	console.log('Failed to send config data!');
			console.log("NACK: " + e);
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