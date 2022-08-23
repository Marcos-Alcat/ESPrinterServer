//------------inicio opciones menú header------//
function logoutButton() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/logout", true);
    xhr.send();
    setTimeout(function() {
        window.open("/logged-out", "_self");
    }, 1000);
}

function rebootButton() {
    document.getElementById("statusdetails").innerHTML = "Invoking Reboot ...";
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/reboot", true);
    xhr.send();
    window.open("/reboot", "_self");
}
//------------fin opciones menú header------//

//------------inicio de script para manejo de SD------//
function listFilesButton() {
    xmlhttp = new XMLHttpRequest();
    xmlhttp.open("GET", "/listfiles", false);
    xmlhttp.send();
    document.getElementById("detailsheader").innerHTML = "<h3>Files<h3>";
    document.getElementById("details").innerHTML = xmlhttp.responseText;
}

function printDeleteButton(filename, action) {
    var urltocall = "/file?name=" + filename + "&action=" + action;
    xmlhttp = new XMLHttpRequest();
    if (action == "delete") {
        xmlhttp.open("GET", urltocall, false);
        xmlhttp.send();
        document.getElementById("status").innerHTML = xmlhttp.responseText;
        xmlhttp.open("GET", "/listfiles", false);
        xmlhttp.send();
        document.getElementById("details").innerHTML = xmlhttp.responseText;
    }
    if (action == "print") {
        xmlhttp.open("GET", urltocall, false);
        xmlhttp.send();
        document.getElementById("status").innerHTML = xmlhttp.responseText;
    }
}

function showUploadButtonFancy() {
    document.getElementById("detailsheader").innerHTML = "<h3>Upload File<h3>"
    document.getElementById("status").innerHTML = "";
    var uploadform = "<form method = \"POST\" action = \"/\" enctype=\"multipart/form-data\"><input type=\"file\" name=\"data\"/><input type=\"submit\" name=\"upload\" value=\"Upload\" title = \"Upload File\"></form>"
    document.getElementById("details").innerHTML = uploadform;
    var uploadform =
        "<form id=\"upload_form\" enctype=\"multipart/form-data\" method=\"post\">" +
        "<input type=\"file\" name=\"file1\" id=\"file1\" onchange=\"uploadFile()\"><br>" +
        "<progress id=\"progressBar\" value=\"0\" max=\"100\" style=\"width:300px;\"></progress>" +
        "<h3 id=\"status\"></h3>" +
        "<p id=\"loaded_n_total\"></p>" +
        "</form>";
    document.getElementById("details").innerHTML = uploadform;
}

function _(el) {
    return document.getElementById(el);
}

function uploadFile() {
    var file = _("file1").files[0];
    var formdata = new FormData();
    formdata.append("file1", file);
    var ajax = new XMLHttpRequest();
    ajax.upload.addEventListener("progress", progressHandler, false);
    ajax.addEventListener("load", completeHandler, false); // doesnt appear to ever get called even upon success
    ajax.addEventListener("error", errorHandler, false);
    ajax.addEventListener("abort", abortHandler, false);
    ajax.open("POST", "/");
    ajax.send(formdata);
}

function progressHandler(event) {
    _("loaded_n_total").innerHTML = "Uploaded " + event.loaded + " bytes";
    var percent = (event.loaded / event.total) * 100;
    _("progressBar").value = Math.round(percent);
    _("status").innerHTML = Math.round(percent) + "% uploaded... please wait";
    if (percent >= 100) {
        _("status").innerHTML = "Please wait, writing file to filesystem";
    }
}

function completeHandler(event) {
    _("status").innerHTML = "Upload Complete";
    _("progressBar").value = 0;
    xmlhttp = new XMLHttpRequest();
    xmlhttp.open("GET", "/listfiles", false);
    xmlhttp.send();
    document.getElementById("status").innerHTML = "File Uploaded";
    document.getElementById("detailsheader").innerHTML = "<h3>Files<h3>";
    document.getElementById("details").innerHTML = xmlhttp.responseText;
}

function errorHandler(event) {
    _("status").innerHTML = "Upload Failed";
}

function abortHandler(event) {
    _("status").innerHTML = "inUpload Aborted";
}
//------------fin de script para manejo de SD------//


/*-----------inicio scripts para gráficos-------------*/
var chartT = new Highcharts.Chart({
    chart: {
        renderTo: 'chart-air'
    },
    title: {
        text: 'Temp:'
    },
    series: [{
        showInLegend: false,
        color: '#059e8a',
        name: 'Bed',
        data: []
    }, {
        showInLegend: false,
        color: '#F24A26',
        name: 'HotEnd',
        data: []
    }],
    plotOptions: {
        line: {
            animation: false,
            dataLabels: {
                enabled: true
            }
        },

    },
    xAxis: {
        type: 'datetime',
        dateTimeLabelFormats: {
            second: '%H:%M:%S'
        }
    },
    yAxis: {
        title: {
            text: 'Celsius'
        }
    },
    credits: {
        enabled: false
    }
});

setInterval(function() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            var x = (new Date()).getTime() - 10800000, //resta 3hs.
                y = this.responseText;
            var datosTemp = JSON.parse(y); //Formato del JSON ejemplo: {"Ex0Temp": 10.00, "Ex0TempSet": 0.00, "BedTemp": 15.00, "BedTempSet": 50.00}.
            document.getElementById("setTempEx0").innerHTML = datosTemp.Ex0TempSet;
            document.getElementById("setTempBed").innerHTML = datosTemp.BedTempSet;
            if (chartT.series[0].data.length > 40) {
                chartT.series[0].addPoint([x, datosTemp.Ex0Temp], true, true, true);
                chartT.series[1].addPoint([x, datosTemp.BedTemp], true, true, true);
            } else {
                chartT.series[0].addPoint([x, datosTemp.Ex0Temp], true, false, true);
                chartT.series[1].addPoint([x, datosTemp.BedTemp], true, false, true);
            }
        }
    };
    xhttp.open("GET", "/temp", true);
    xhttp.send();
}, 4000);
/*-----------fin scripts para gráficos-------------*/

/*-----------inicio Controles de impresora---------*/
function restore_x(e) {
    centerxytext.textContent = 'X/Y';
    show_hide(e, 0)
}

function changecolor() { //cambia el color de íconos de homing. 
    if (isDown) return;
    var len = arguments.length;
    if (len < 2) return;
    var color = arguments[len - 1];
    for (var i = 0; i < len - 1; i++) {
        arguments[i].style.fill = color;
    }
}


function restore_z(e) {
    centerztext.textContent = 'Z';
    show_hide(e, 0);
}

function restore_e(e) {
    show_hide(extruder, 1);
    show_hide(centeretext, 0);
    show_hide(e, 0);
}

function settext_e(text, e) {
    if (isDown) return;
    show_hide(extruder, 0);
    centeretext.textContent = text;
    show_hide(centeretext, 1);
    show_hide(e, 0.5);
}

function show_hide(e, show) {
    e.style.opacity = show;
}


function settext_x(text, e) {
    if (isDown) return;
    centerxytext.textContent = text;
    show_hide(e, 0.5)
}

function settext_z(text, e) {
    if (isDown) return;
    centerztext.textContent = text;
    show_hide(e, 0.5);
}


var isDown = false;

document.addEventListener('mouseup', function() {
    isDown = false;
}, true);


document.addEventListener('mousemove', function(event) {
    if (isDown) { movethumb(event.clientX); }
}, true);

function sendMoveControl(move, value) {
    var urltocall = "/controlMove?move=" + move + "&value=" + value;
    xmlhttp = new XMLHttpRequest();
    xmlhttp.open("GET", urltocall, false);
    xmlhttp.send();
    document.getElementById("statusComand").innerHTML = xmlhttp.responseText;
}
/*fin Controles de impresora*/


//script de SLIDERS
const sliders = document.querySelectorAll(".slider-ui");
var iniciar = false;
var valorPrevio = 0;
sliders.forEach(slider => {
    let input = slider.querySelector("input[type=range]");
    let min = input.getAttribute("min");
    let max = input.getAttribute("max");
    let valueElem = slider.querySelector(".value");
    slider.querySelector(".min").innerText = min;
    slider.querySelector(".max").innerText = max;

    function setValueElem(iniciar) {
        if (iniciar) {
            let percent = (input.value - min) / (max - min) * 100;
            valueElem.style.left = percent + "%";
            valueElem.innerText = input.value;
        } else {
            valueElem.style.left = 0 + "%";
            valueElem.innerText = 0;
            iniciar = true;
        }
    }

    function sendSliderData() {
        var opcion = confirm("Confirma el valor: " + input.value);
        if (opcion == true) {
            var urltocall = "/slider?name=" + slider.className + "&value=" + input.value;
            xmlhttp = new XMLHttpRequest();
            xmlhttp.open("GET", urltocall, false);
            xmlhttp.send();
            document.getElementById("statusComand").innerHTML = xmlhttp.responseText;
        } else {
            let percent = (valorPrevio - min) / (max - min) * 100;
            valueElem.style.left = percent + "%";
            valueElem.innerText = valorPrevio;
        }
    }

    setValueElem();
    input.addEventListener("mousedown", () => {
        valorPrevio = valueElem.innerText;
        document.getElementById("codSetExt").innerHTML = valueElem.innerText;
        valueElem.classList.add("up");
    });
    input.addEventListener("input", setValueElem);
    input.addEventListener("mouseup", () => {
        valueElem.classList.remove("up");
        sendSliderData();
    });
});
//script fin de sliders


//enviar comandos a la impresora.
function sendGCODE() {
    var urltocall = "/sendGCODE?value=" + document.getElementById("inputgcode").value;
    xmlhttp = new XMLHttpRequest();
    xmlhttp.open("GET", urltocall, false);
    xmlhttp.send();
    document.getElementById("gcode").innerHTML = xmlhttp.responseText;
    document.getElementById("inputgcode").value = "";
}
//fin enviar comandos a la impresora.