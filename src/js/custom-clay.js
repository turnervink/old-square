module.exports = function(minified) {
  var clayConfig = this;
	
	function weatherSection() {
		if (this.get()) {
			clayConfig.getItemByAppKey("useCelsius").show();
			clayConfig.getItemByAppKey("shakeWeather").show();
			clayConfig.getItemByAppKey("largeFont").show();
		} else {
			clayConfig.getItemByAppKey("useCelsius").hide();
			clayConfig.getItemByAppKey("shakeWeather").hide();
			clayConfig.getItemByAppKey("largeFont").hide();
		}
	}
	
	/*function fontToggle() {
		var largeFont = clayConfig.getItemByAppKey("largeFont");
		if (this.get()) {
			largeFont.disable();
			largeFont.set(false);
		} else {
			largeFont.enable();
		}
	}*/
	
  clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
		var _ = minified._;
  	var $ = minified.$;
  	var HTML = minified.HTML;
		
		var platform = clayConfig.meta.activeWatchInfo.platform;
		
		if (platform == "aplite") {
			clayConfig.getItemByAppKey("backgroundColor").hide();
			clayConfig.getItemByAppKey("textColor").hide();
		}
		
		if (platform == "basalt") {
			clayConfig.getItemByAppKey("invertColors").hide();
		}
		
		var weatherToggle = clayConfig.getItemByAppKey("showWeather");
		weatherSection.call(weatherToggle);
		weatherToggle.on("change", weatherSection);
		
		var donateButton = clayConfig.getItemById("donateButton");
		donateButton.on("click", function() {
			window.location.href="https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=XM6V4BDNBC2TJ";
		});
		
		/*var showSeconds = clayConfig.getItemByAppKey("showSeconds");
		fontToggle.call(showSeconds);
		showSeconds.on("change", fontToggle);*/
  });
};