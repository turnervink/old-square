Pebble.addEventListener('ready', function() {
	console.log('PebbleKit JS Ready!');
});

Pebble.addEventListener('showConfiguration', function() {
	var url = 'http://49994a02.ngrok.com/';

	console.log('Showing configuration page: ' + url);

	Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
	var configData = JSON.parse(decodeURIComponent(e.response));

	console.log('Configuration page returned: ' + JSON.stringify(configData));

	if (configData.textColor) {
		Pebble.sendAppMessage({
			textColor: parseInt(configData.textColor, 16),
			invertColors: configData.invertColors ? 1 : 0
		}, function() {
			console.log('Send successful!');
		}, function() {
			console.log('Send failed!');
		});
	}
});