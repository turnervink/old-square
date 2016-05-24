module.exports = [
	
	{
		"type": "section",
		"items": [
			{
				"type": "heading",
				"id": "main-heading",
				"defaultValue": "<h1 style='text-align:center'>Square</h1>",
				"size": 1
			},
			{
				"type": "heading",
				"id": "dev-heading",
				"defaultValue": "<p style='text-align:center;'>by Turner Vink<br> <a href='http://www.turnervink.com' style='color:#FF4500; text-decoration:none;'>www.turnervink.com</a></p>",
				"size": 6
			},
		]
	},
	{
		"type": "section",
		"items": [
			{
				"type": "heading",
				"id": "localization-heading",
				"defaultValue": "Localization",
				"size": 4
			},
			{
				"type": "select",
				"appKey": "langSel",
				"defaultValue": "en",
				"label": "Language",
				"options": [
					{
						"label": "English",
						"value": "en"
					},
					{
						"label": "Français",
						"value": "fr"
					},
					{
						"label": "Español",
						"value": "es"
					},
					{
						"label": "Deutsch",
						"value": "de"
					}
				]
			},
			{
				"type": "select",
				"appKey": "dateFormat",
				"defaultValue": "sdate",
				"label": "Date format",
				"options": [
					{
						"label": "Thu Oct 15",
						"value": "sdate",
					},
					{
						"label": "Thu 15 Oct",
						"value": "edate"
					}
				]
			}
		]
	},
	{
		"type": "section",
		"items": [
			{
				"type": "heading",
				"id": "appearance-heading",
				"defaultValue": "Appearance",
				"size": 4
			},
			{
				"type": "color",
				"appKey": "backgroundColor",
				"defaultValue": "000000",
				"label": "Background colour",
				"sunlight": true,
			},
			{
				"type": "color",
				"appKey": "textColor",
				"defaultValue": "00ff00",
				"label": "Text colour",
				"sunlight": true,
			},
			{
				"type": "toggle",
				"appKey": "invertColors",
				"label": "Invert colours",
				"defaultValue": false
			},
			{
				"type": "select",
				"appKey": "barType",
				"defaultValue": "0",
				"label": "Middle bar",
				"options": [
					{
						"label": "Battery level",
						"value": 0
					},
					{
						"label": "Step goal progress",
						"value": 1
					},
					{
						"label": "Static bar",
						"value": 2
					}
				]
			},
			{
				"type": "toggle",
				"appKey": "showSeconds",
				"label": "Show seconds",
				"defaultValue": false
			}
		]
	},
	{
		"type": "section",
		"items": [
			{
				"type": "heading",
				"id": "weather-heading",
				"defaultValue": "Weather",
				"size": 4
			},
			{
				"type": "toggle",
				"appKey": "showWeather",
				"label": "Show weather",
				"defaultValue": true
			},
			{
				"type": "toggle",
				"appKey": "useCelsius",
				"label": "Use Celsius",
				"defaultValue": false
			},
			{
				"type": "toggle",
				"appKey": "shakeWeather",
				"label": "Shake for weather",
				"defaultValue": true
			},
			{
				"type": "toggle",
				"appKey": "largeFont",
				"label": "Use large weather font",
				"defaultValue": false,
			},
		]
	},
	{
		"type": "section",
		"items": [
			{
				"type": "heading",
				"id": "bluetooth-heading",
				"defaultValue": "Bluetooth",
				"size": 4
			},
			{
				"type": "toggle",
				"appKey": "vibeDisconnect",
				"label": "Vibrate on disconnect",
				"defaultValue": true
			},
			{
				"type": "toggle",
				"appKey": "vibeConnect",
				"label": "Vibrate on reconnect",
				"defaultValue": false
			}
		]
	},
	{
		"type": "section",
		"items": [
			{
				"type": "heading",
				"id": "premium-heading",
				"defaultValue": "Premium Settings",
				"size": 4
			},
			{
				"type": "toggle",
				"appKey": "buyPremium",
				"label": "Premium Features - $1.99",
				"description": "Please switch this toggle, save, and then come back to adjust new settings (even if you've already paid and reinstalled; you won't have to pay again!)",
				"defaultValue": false
			},
			{
				"type": "toggle",
				"appKey": "manualGoal",
				"label": "Custom step goal",
				"defaultValue": false
			},
			{
				"type": "slider",
				"appKey": "stepGoal",
				"defaultValue": 10000,
				"label":"Step goal",
				"min": 1000,
				"max": 10000,
				"step": 1000,
				"description": "Change middle bar setting to step goal under Appearance to view step progress"
			},
			{
				"type": "input",
				"appKey": "nightModeStart",
				"defaultValue": 20,
				"label": "Night Mode Start Hour (0 - 24)",
				"attributes": {
					"limit": 2,
					"type": "number",
					"min": 0,
					"max": 24
				}
			},
			{
				"type": "input",
				"appKey": "nightModeEnd",
				"defaultValue": 20,
				"label": "Night Mode End Hour (0 - 24)",
				"attributes": {
					"limit": 2,
					"type": "number",
					"min": 0,
					"max": 24
				}
			},
			{
				"type": "color",
				"appKey": "nightBackgroundColor",
				"defaultValue": "000000",
				"label": "Night Mode Background colour",
				"sunlight": true,
			},
			{
				"type": "color",
				"appKey": "nightTextColor",
				"defaultValue": "00ff00",
				"label": "Night Mode Text colour",
				"sunlight": true,
			},
			{
				"type": "input",
				"appKey": "cityName",
				"defaultValue": "",
				"label": "Weather location",
				"description": "Enter a city to use for weather updates, or leave blank to use GPS location.",
				"attributes": {
					"placeholder": "",
				}
			}
			]
	},
	{
		"type": "section",
			"items": [
				{
					"type": "button",
					"defaultValue": "Donate",
					"id": "donateButton",
					"description": "<p style='text-align:center;'>Before you tap save, consider donating a dollar or two through PayPal by tapping the button above.</p>"
				},
				{
					"type": "submit",
					"defaultValue": "Save"
				}
			]
	}
];