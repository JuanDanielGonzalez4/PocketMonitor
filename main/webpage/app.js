/**
 * Add gobals here
 */
var seconds = null;
var otaTimerVar = null;
var wifiConnectInterval = null;
var yValues = [];
var array_length = 100;
var myChart;



/**
 * Initialize functions here.
 */

$(document).ready(function () {
  var xValues = [];
  for (let i = 0; i < array_length; i++) {
    xValues[i] = i;
  }

  const ctx = document.getElementById('myChart').getContext('2d');
  myChart = new Chart(ctx, {
    type: 'line',
    data: {
      labels: xValues,
      datasets: [{
        fill: false,
        lineTension: 0,
        backgroundColor: "#1de9b6",
        borderColor: "#1de9b6 ",
        pointRadius: 0,
        data: yValues
      }]
    },
    options: {
      legend: {display: false},
      scales: {
        yAxes: [{ticks: {min: 0, max:4000}}],
      },
      title:{
        display: true,
        text: 'Electrocardiogram'
      },
      responsive: true,
      maintainAspectRatio: false
    }
  });

  getUpdateStatus();
  $("#connect_wifi").on("click", function () {
    checkCredentials();
  });
});

/**
 * Gets file name and size for display on the web page.
 */
function getFileInfo() {
  var x = document.getElementById("selected_file");
  var file = x.files[0];

  document.getElementById("file_info").innerHTML =
    "<h4>File: " + file.name + "<br>" + "Size: " + file.size + " bytes</h4>";
}

/**
 * Handles the firmware update.
 */
function updateFirmware() {
  // Form Data
  var formData = new FormData();
  var fileSelect = document.getElementById("selected_file");

  if (fileSelect.files && fileSelect.files.length == 1) {
    var file = fileSelect.files[0];
    formData.set("file", file, file.name);
    document.getElementById("ota_update_status").innerHTML =
      "Uploading " + file.name + ", Firmware Update in Progress...";

    // Http Request
    var request = new XMLHttpRequest();

    request.upload.addEventListener("progress", updateProgress);
    request.open("POST", "/OTAupdate");
    request.responseType = "blob";
    request.send(formData);
  } else {
    window.alert("Select A File First");
  }
}

/**
 * Progress on transfers from the server to the client (downloads).
 */
function updateProgress(oEvent) {
  if (oEvent.lengthComputable) {
    getUpdateStatus();
  } else {
    window.alert("total size is unknown");
  }
}

/**
 * Posts the firmware udpate status.
 */
function getUpdateStatus() 
{
    var xhr = new XMLHttpRequest();
    var requestURL = "/OTAstatus";
    xhr.open('POST', requestURL, false);
    xhr.send('ota_update_status');

    if (xhr.readyState == 4 && xhr.status == 200) 
	{		
        var response = JSON.parse(xhr.responseText);
						
	 	document.getElementById("latest_firmware").innerHTML = response.compile_date + " - " + response.compile_time

		// If flashing was complete it will return a 1, else -1
		// A return of 0 is just for information on the Latest Firmware request
        if (response.ota_update_status == 1) 
		{
    		// Set the countdown timer time
            seconds = 10;
            // Start the countdown timer
            otaRebootTimer();
        } 
        else if (response.ota_update_status == -1)
		{
            document.getElementById("ota_update_status").innerHTML = "!!! Upload Error !!!";
        }
    }
}

/**
 * Displays the reboot countdown.
 */
function otaRebootTimer() {
  document.getElementById("ota_update_status").innerHTML =
    "OTA Firmware Update Complete. This page will close shortly, Rebooting in: " +
    seconds;

  if (--seconds == 0) {
    clearTimeout(otaTimerVar);
    window.location.reload();
  } else {
    otaTimerVar = setTimeout(otaRebootTimer, 1000);
  }
}

/**
 * Clears the connection status interval.
 */
function stopWifiConnectStatusInterval() {
  if (wifiConnectInterval != null) {
    clearInterval(wifiConnectInterval);
    wifiConnectInterval = null;
  }
}

/**
 * Gets the WiFi connection status.
 */
function getWifiConnectStatus() {
  var xhr = new XMLHttpRequest();
  var requestURL = "/wifiConnectStatus";
  xhr.open("POST", requestURL, false);
  xhr.send("wifi_connect_status");

  if (xhr.readyState == 4 && xhr.status == 200) {
    var response = JSON.parse(xhr.responseText);

    document.getElementById("wifi_connect_status").innerHTML = "Conectando...";

    if (response.wifi_connect_status == 2) {
      document.getElementById("wifi_connect_status").innerHTML =
        "<h4 class='rd'>Failed to Connect. Please check your AP credentials and compatibility</h4>";
      stopWifiConnectStatusInterval();
    } else if (response.wifi_connect_status == 3) {
      document.getElementById("wifi_connect_status").innerHTML =
        "Conexión Exitosa";



      stopWifiConnectStatusInterval();
    }
  }
}

/**
 * Starts the interval for checking the connection status.
 */
function startWifiConnectStatusInterval() {
  wifiConnectInterval = setInterval(getWifiConnectStatus, 2800);
}

/**
 * Connect WiFi function called using the SSID and password entered into the text fields.
 */
function connectWifi() {

  selectedSSID = $("#connect_ssid").val();
  pwd = $("#connect_pass").val();

  // Create an object to hold the data to be sent in the request body
  var requestData = {
    selectedSSID: selectedSSID,
    pwd: pwd,
    timestamp: Date.now(),
  };

  // Serialize the data object to JSON
  var requestDataJSON = JSON.stringify(requestData);

  $.ajax({
    url: "/wifiConnect.json",
    dataType: "json",
    method: "POST",
    cache: false,
    data: requestDataJSON, // Send the JSON data in the request body
    contentType: "application/json", // Set the content type to JSON
    success: function (response) {
      // Handle the success response from the server
      console.log(response);
    },
    error: function (xhr, status, error) {
      // Handle errors
      console.error(xhr.responseText);
    },
  });

  startWifiConnectStatusInterval();
}

/**
 * Checks credentials on connect_wifi button click.
 */
function checkCredentials() {
  errorList = "";
  credsOk = true;

  selectedSSID = $("#connect_ssid").val();
  pwd = $("#connect_pass").val();

  if (selectedSSID == "") {
    errorList += "<h4 class='rd'>SSID cannot be empty!</h4>";
    credsOk = false;
  }
  if (pwd == "") {
    errorList += "<h4 class='rd'>Password cannot be empty!</h4>";
    credsOk = false;
  }

  if (credsOk == false) {
    $("#wifi_connect_credentials_errors").html(errorList);
  } else {
    $("#wifi_connect_credentials_errors").html("");
    connectWifi();
  }
}

/**
 * Shows the WiFi password if the box is checked.f
 */
function showPassword() {
  var x = document.getElementById("connect_pass");
  if (x.type === "password") {
    x.type = "text";
  } else {
    x.type = "password";
  }
}

/**
 * Update the adc value once is fetched.
 */
function updateADCValue() {
  fetch("/adc_value")
    .then((response) => {
      if (!response.ok) {
        throw new Error("Network response was not ok " + response.statusText);
      }
      return response.text();
    })
    .then((data) => {
      // console.log("ADC Value:", data); // Logging ADC value to the console
      // document.getElementById("adcValue").innerText = data;

      var adcValue = parseFloat(data); // Convert string to float
      yValues.push(adcValue); // Add the value to the array

      // Limit the array to a certain length (for example, 10)
      if (yValues.length > array_length) {
        yValues.shift(); // Remove the oldest element
      }

      // Update the chart
      myChart.data.datasets[0].data = yValues;
      myChart.update();

    })
    .catch((error) => {
      console.error("There was a problem with the fetch operation:", error);
    });
}

setInterval(updateADCValue, 100);


function updateTEMPValue() {
  fetch("/temp_value")
    .then((response) => {
      if (!response.ok) {
        throw new Error("Network response was not ok " + response.statusText);
      }
      return response.text();
    })
    .then((data) => {
      console.log("Temp Value:", data); // Logging ADC value to the console
      document.getElementById("adcValue").innerText = data;
      // document.getElementById("adcValue").innerText = data;
    })
    .catch((error) => {
      console.error("There was a problem with the fetch operation:", error);
    });
}
// Update every 2 seconds
setInterval(updateTEMPValue, 500);

function updateFirmwareButton(){
  document.getElementById("OTA").style.display = "block";
  document.querySelector('footer').style = 'display: none';
}
const delayUpdateTemp = 1000;

updateButtonA = setInterval(updateButonA, delayUpdateTemp);
updateButtonB=setInterval(updateButonB, delayUpdateTemp);
valueB = 0;
valueA = 0;

function updateButonA() {
	let requestA = new XMLHttpRequest();

	requestA.addEventListener("readystatechange", () => {
		// console.log(requestA, requestA.readyState);
		if (requestA.readyState === 4) {
      if (requestA.responseText != valueA){
        console.log(requestA.responseText);
        document.getElementById("heartRate").style.display = "none";
        document.getElementById("chart-container").style.display = "block";
        valueA = requestA.responseText;
      }
		}
	});
	requestA.open('GET', "/buttonA");
	requestA.responseType = "text";
	requestA.send();
}
function updateButonB() {
	let requestB = new XMLHttpRequest();

	requestB.addEventListener("readystatechange", () => {
    console.log(requestB.responseText);
		if (requestB.readyState === 4) {
      if (requestB.responseText != valueB){
        console.log(requestB.responseText);
        document.getElementById("heartRate").style.display = "block";
        document.getElementById("chart-container").style.display = "none";
        valueB = requestB.responseText;
      }
		}
	});
	requestB.open('GET', "/buttonB");
	requestB.responseType = "text";
	requestB.send();
}

