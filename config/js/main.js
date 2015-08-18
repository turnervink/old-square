(function () {
	loadOptions();
	submitHandler();
})();

function submitHandler() {
	var $submitButton = $('#submitButton');

	$submitButton.on('click', function() {
		console.log('Submit');

		var return_to = getQueryParam('return_to', 'pebblejs://close#');
		document.location = return_to + encodeURIComponent(JSON.stringify(getAndStoreConfigData()));
	});
}

function loadOptions() {
	var $textColorPicker = $('#textColorPicker');
	var $invertColors = $('#invertColors');

	if (localStorage.invertColors) {
		$textColorPicker[0].value = localStorage.textColor;
		$invertColors[0].checked = localStorage.invertColors === 'true';
	}
}

function getAndStoreConfigData() {
	var $textColorPicker = $('#textColorPicker');
	var $invertColors = $('#invertColors');

	var options = {
		textColor: $textColorPicker.val(),
		invertColors: $invertColors[0].checked
	};

	localStorage.textColor = options.textColor;
	localStorage.invertColors = options.invertColors;

	console.log('Got options: ' + JSON.stringify(options));
	return options;
}

function getQueryParam(variable, defaultValue) {
  var query = location.search.substring(1);
  var vars = query.split('&');
  for (var i = 0; i < vars.length; i++) {
    var pair = vars[i].split('=');
    if (pair[0] === variable) {
      return decodeURIComponent(pair[1]);
    }
  }
  return defaultValue || false;
}