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
				"label": "Date Format",
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
				"id": "weather-heading",
				"defaultValue": "Weather",
				"size": 4
			},
			{
				"type": "toggle",
				"appKey": "showWeather",
				"label": "Show Weather",
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
				"defaultValue": false
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
				"appKey": "vibeConnect",
				"label": "Vibrate on Bluetooth Reconnect",
				"defaultValue": true
			},
			{
				"type": "toggle",
				"appKey": "vibeDisconnect",
				"label": "Vibrate on Bluetooth Disconnect",
				"defaultValue": true
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
				"type": "toggle",
				"appKey": "reflectBatt",
				"label": "Bar shows battery level",
				"defaultValue": true
			},
			{
				"type": "color",
				"appKey": "backgroundColor",
				"defaultValue": "000000",
				"label": "Background Color",
				"sunlight": false,
			},
			{
				"type": "color",
				"appKey": "textColor",
				"defaultValue": "00ff00",
				"label": "Text Color",
				"sunlight": false,
			},
			{
				"type": "toggle",
				"appKey": "invertColors",
				"label": "Invert Colors",
				"defaultValue": false
			}
		]
	},
	{
		"type": "section",
			"items": [
				{
					"type": "heading",
					"id": "paypal-heading",
					"defaultValue": "<h4 style='text-align:center;'>Before you tap save, consider donating a dollar or two through PayPal by tapping below.</h4>",
					"size": 4
				},
				{
					"type": "button",
					"defaultValue": "Donate",
					"id": "donateButton"
				}
			]
	},
	
	{
  	"type": "submit",
  	"defaultValue": "Save"
	}
	
];