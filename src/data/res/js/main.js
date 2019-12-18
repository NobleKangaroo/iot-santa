// Variables to keep track of the last refresh times
var lastRefreshState = 0;
var lastRefreshClients = 0;

// Generic AJAX call, to reduce code duplication
function doAjax(method, endpoint, eleid) {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            for (var ele of document.querySelectorAll(eleid)) {
                ele.innerHTML = this.responseText;
            }
        }
    };
    xhr.open(method, endpoint, true);
    xhr.send();
}

// Refresh Sonoff state
function refreshState() {
    lastRefreshState = Date.now();
    doAjax('GET', 'state', '#state');
}

// Refresh the client list
function refreshClients() {
    lastRefreshClients = Date.now();
    doAjax('GET', 'clients', '#clients');
}

// Toggle Sonoff state and refresh Sonoff state
function toggleState() {
    doAjax('POST', 'toggle', '#state');
    refreshState();
}

// Refresh Sonoff state and the client list every 3 seconds
setInterval(function() {
    var now = Date.now();
    if (now - lastRefreshState >= 3000) { refreshState(); }
    if (now - lastRefreshClients >= 3000) { refreshClients(); }
}, 50);

