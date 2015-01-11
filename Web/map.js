/**
 * Created by Orthocenter on 1/2/15.
 */

tiles_l = 256;
real_tiles_l = 0;
tiles_filepath = "Tiles/";

function getMouseCoordFromEvent(event) {
    var x = 0;
    var y = 0;

    var element = event.target;
    do {
        x += element.offsetLeft;
        y += element.offsetTop;
    } while (element = element.offsetParent);

    x = (window.pageXOffset + event.clientX) - x;
    y = (window.pageYOffset + event.clientY) - y;

    return {x: x, y: y};
}

function onCanvasMouseEvent(event) {
    event.preventDefault();
    var coord = getMouseCoordFromEvent(event);
    if (event.type == "mousedown") {
        dragging = true;
        lastCoord = coord;
    }
    else if (event.type == "mousemove") {
        if (dragging) {
            updateTLPixel(-(coord.x - lastCoord.x), -(coord.y - lastCoord.y));
            lastCoord = coord;
            //drawTiles();
            drawShortPath();
        }
    }
    else if (event.type == "mouseup") {
        updataURL();
        dragging = false;
    }
    else if (event.type == "mousewheel") {
        if (typeof wheelDelta == "undefined")
            wheelDelta = 0;
        else
            wheelDelta += event.wheelDelta;

        if (wheelDelta < -100) {
            wheelDelta = 0;
            if (level - 1 >= minLevel) {
                level--;
                setTLPixel(tl_px / 2 - coord.x / 2, tl_py / 2 - coord.y / 2);
                drawTiles();
                drawShortPath();
                updataURL();
            }
        }

        if (wheelDelta > 100) {
            wheelDelta = 0;
            if (level + 1 <= maxLevel) {
                level++;
                setTLPixel(tl_px * 2 + coord.x, tl_py * 2 + coord.y);
                drawTiles();
                drawShortPath();
                updataURL();
            }
        }
    }
    else if (event.type == "dblclick") {
        if (!event.altKey) {
            if (level + 1 <= maxLevel) {
                level++;
                bottomLayerContext.scale(1.25, 1.25);
                bottomLayerContext.scale(1.25, 1.25);
                bottomLayerContext.scale(1.25, 1.25);
                setTLPixel(tl_px * 2 + coord.x, tl_py * 2 + coord.y);
                drawTiles();
                drawShortPath();
                updataURL();
            }
        }
        else
        {
            if (level - 1 >= minLevel) {
                level--;
                setTLPixel(tl_px / 2 - coord.x / 2, tl_py / 2 - coord.y / 2);
                drawTiles();
                drawShortPath();
                updataURL();
            }
        }
    }
}

function getTouchCoordFromEvent(event) {
    return {x: event.targetTouches[0].pageX,
        y: event.targetTouches[0].pageY};
}

function onCanvasTouchEvent(event) {
    event.preventDefault();
    var coord = getTouchCoordFromEvent(event);
    if (event.type == "touchstart") {
        dragging = true;
        lastCoord = coord;
    }
    else if (event.type == "touchmove" && event.targetTouches.length == 1) {
        if (dragging) {
            updateTLPixel(-(coord.x - lastCoord.x), -(coord.y - lastCoord.y));
            lastCoord = coord;
            //drawTiles();
            drawShortPath();
        }
    }
    else if (event.type == "touchmove" && event.targetTouches.length == 2) {
        if (typeof lastTouchesDist == "undefined") lastTouchesDist = 0;

        if (typeof touchesDelta == "undefined") {
            touchesDelta = 0;
        } else {
            var dist = Math.abs(event.targetTouches[0].pageX - event.targetTouches[1].pageX) +
                Math.abs(event.targetTouches[0].pageY - event.targetTouches[1].pageY);
            touchesDelta += dist - lastTouchesDist;
            lastTouchesDist = dist;
        }

        var centerX = (event.targetTouches[0].pageX + event.targetTouches[1].pageX) / 2;
        var centerY = (event.targetTouches[0].pageY + event.targetTouches[1].pageY) / 2;

        if (touchesDelta < -30) {
            touchesDelta = 0;
            if (level - 1 >= minLevel) {
                level--;
                setTLPixel(tl_px / 2 - centerX / 2, tl_py / 2 - centerY / 2);
                drawTiles();
                drawShortPath();
                updataURL();
            }
        }

        if (touchesDelta > 30) {
            touchesDelta = 0;
            if (level + 1 <= maxLevel) {
                level++;
                setTLPixel(tl_px * 2 + centerX, tl_py * 2 + centerY);
                drawTiles();
                drawShortPath();
                updataURL();
            }
        }
    }
    else if (event.type == "touchend") {
        dragging = false;
        updataURL();
    }
}

function WGS84_to_px_py(lat, lon, level) {
    var sinLat = Math.sin(lat * Math.PI / 180);
    var n = Math.pow(2, level);

    var px = (lon + 180.0) / 360.0 * tiles_l * n;
    var py = (0.5 - Math.log((1 + sinLat) / (1 - sinLat)) / (4 * Math.PI)) * tiles_l * n;

    return {px: px, py: py}
}

function WGS84_to_tx_ty(lat, lon, level) {
    var sinLat = Math.sin(lat * Math.PI / 180);
    var n = Math.pow(2, level);
    var tx = Math.floor((lon + 180) / 360 * n);
    var ty = Math.floor((0.5 - Math.log((1 + sinLat) / (1 - sinLat)) / (4 * Math.PI)) * n);

    return {tx: tx, ty: ty};
}

function updataURL() {
    var n = Math.pow(2, level);
    var center_px = tl_px + window.innerWidth / 2;
    var center_py = tl_py + window.innerHeight / 2;
    var lon = center_px / tiles_l / n * 360.0 - 180.0;
    var lat = Math.atan(Math.sinh(Math.PI * (1 - 2.0 * center_py / tiles_l / n))) * 180 / Math.PI;
    var url = window.location.href;
    var new_url = url.substring(0, url.lastIndexOf("?"));
    new_url += "?lat=" + lat.toFixed(5);
    new_url += "&lon=" + lon.toFixed(5);
    new_url += "&level=" + level;
    window.history.pushState({}, "", new_url);
}

function setTLTxTy(tx, ty) {
    tl_tx = Math.floor(tx);
    tl_ty = Math.floor(ty);

    tl_px = tx * tiles_l;
    tl_py = ty * tiles_l;

    tl_ox = 0;
    tl_oy = 0;
}

function setTLPixel(px, py) {
    tl_px = px;
    tl_py = py;

    tl_tx = Math.floor(tl_px / tiles_l);
    tl_ty = Math.floor(tl_py / tiles_l);

    tl_ox = Math.floor(tl_tx * tiles_l - px) * retina_ratio;
    tl_oy = Math.floor(tl_ty * tiles_l - py) * retina_ratio;
}

function updateTLPixel(dpx, dpy) {
    var _tl_px = tl_px;
    var _tl_py = tl_py;
    var _tl_tx = tl_tx;
    var _tl_ty = tl_ty;

    setTLPixel(tl_px + dpx, tl_py + dpy);

    console.log(dpx, dpy);
    console.log(loading.length)
    count++;

    var imgData = bottomLayerContext.getImageData(0, 0, bottomLayer.width, bottomLayer.height);
    bottomLayerContext.putImageData(imgData, - dpx * retina_ratio, - dpy * retina_ratio);

    console.log(bottomLayer.width)
    var _tlx = 0, _tly = 0;
    var _brx = bottomLayer.width, _bry = bottomLayer.height;
    var tx_begin = tl_tx;
    var tx_end = tl_tx + numTilesX;
    var ty_begin = tl_ty;
    var ty_end = tl_ty + numTilesY;

    bottomLayerContext.fillStyle = "#f7f5f0canvas";
    if (dpx < 0) {
        bottomLayerContext.fillRect(0, 0, -dpx * retina_ratio, bottomLayer.height);
        _tlx -= dpx * retina_ratio;
    } else {
        bottomLayerContext.fillRect(bottomLayer.width - dpx * retina_ratio, 0, dpx * retina_ratio, bottomLayer.height);
        _brx -= dpx * retina_ratio;
    }

    if (dpy < 0) {
        bottomLayerContext.fillRect(0, 0, bottomLayer.width, -dpy * retina_ratio);
        _tly -= dpy * retina_ratio;
    } else {
        bottomLayerContext.fillRect(0, bottomLayer.height - dpy * retina_ratio, bottomLayer.width, dpy * retina_ratio);
        _bry -= dpy * retina_ratio;
    }

    for (var tx = tx_begin; tx <= tx_end; tx++) {
        for (var ty = ty_begin; ty <= ty_end; ty++) {
            var tlx = real_tiles_l * (tx - tl_tx) + tl_ox;
            var tly = real_tiles_l * (ty - tl_ty) + tl_oy;
            var brx = tlx + real_tiles_l;
            var bry = tly + real_tiles_l;
            var flagx = false, flagy = false;

            if (dpx < 0 && tlx >= _tlx) {
                flagx = true;
            }
            if (dpx > 0 && brx <= _brx) {
                flagx = true;
            }
            flagx |= (dpx == 0);

            if (dpy < 0 && tly >= _tly) {
                flagy = true;
            }
            if (dpy > 0 && bry <= _bry) {
                flagy = true;
            }
            flagy |= (dpy == 0);

            if (flagx && flagy) {
                continue;
            }


            if (retina_ratio == 1) {
                var tile_filename = tiles_filepath + tx + "_" + ty + "_" + level + ".png";
            } else {
                var tile_filename = tiles_filepath + tx + "_" + ty + "_" + level + "_r.png";
            }

            if (loading.indexOf(tile_filename) >= 0) continue;

            var tile = new Image();
            tile.src = tile_filename;
            tile.tx = tx;
            tile.ty = ty;
            tile.count = count;
            loading.push(tile.src);
            console.log(tx, ty);
            tile.onload = function () {
                drawTile(this);
            }
        }
    }

    function drawTile(tile) {
        //if (count != tile.count)
            console.log(tile.src)
        loading.splice(loading.indexOf(tile.src), 1);
        bottomLayerContext.drawImage(tile, real_tiles_l * (tile.tx - tl_tx) + tl_ox, real_tiles_l * (tile.ty - tl_ty) + tl_oy);
    }
}

function setCanvasSize() {
    devicePixelRatio = window.devicePixelRatio || 1;
    backingStoreRatio = bottomLayerContext.webkitBackingStorePixelRatio || 1;
    retina_ratio = (devicePixelRatio / backingStoreRatio) > 1 ? 2 : 1;
    real_tiles_l = tiles_l * retina_ratio;
    bottomLayer.width = window.innerWidth * retina_ratio;
    bottomLayer.height = window.innerHeight * retina_ratio;
    bottomLayer.style.height = window.innerHeight + "px";
    bottomLayer.style.width = window.innerWidth + "px";

    shortPathLayer.width = window.innerWidth * retina_ratio;
    shortPathLayer.height = window.innerHeight * retina_ratio;
    shortPathLayer.style.height = window.innerHeight + "px";
    shortPathLayer.style.width = window.innerWidth + "px";
}

function setDrawParam() {
    numTilesX = (bottomLayer.width / real_tiles_l) + 1;
    numTilesY = (bottomLayer.height / real_tiles_l) + 1;

    numTiles = numTilesX * numTilesY;

    maxLevel = 17;
    minLevel = 10;
}

function drawTiles() {
    var tx_begin = tl_tx;
    var tx_end = tl_tx + numTilesX;
    var ty_begin = tl_ty;
    var ty_end = tl_ty + numTilesY;

    for (var tx = tx_begin; tx < tx_end; tx++) {
        for (var ty = ty_begin; ty < ty_end; ty++) {
            if (retina_ratio == 1) {
                var tile_filename = tiles_filepath + tx + "_" + ty + "_" + level + ".png";
            } else {
                var tile_filename = tiles_filepath + tx + "_" + ty + "_" + level + "_r.png";
            }

            bottomLayerContext.fillStyle = "#f7f5f0";
            bottomLayerContext.fillRect(real_tiles_l * (tx - tl_tx) + tl_ox, real_tiles_l * (ty - tl_ty) + tl_oy
                , real_tiles_l, real_tiles_l);

            var tile = new Image();
            tile.src = tile_filename;
            tile.tx = tx;
            tile.ty = ty;
            tile.onload = function () {
                drawTile(this);
            }
        }
    }

    function drawTile(tile) {
        bottomLayerContext.drawImage(tile, real_tiles_l * (tile.tx - tl_tx) + tl_ox, real_tiles_l * (tile.ty - tl_ty) + tl_oy);
    }
}

function getURLParameter(name) {
    return decodeURIComponent((new RegExp('[?|&]' + name + '=' + '([^&;]+?)(&|#|;|$)').exec(location.search)||[,""])[1].replace(/\+/g, '%20'))||null
}

function onWindowLoad() {
    loading = [];
    count = 0;

    bottomLayer = document.getElementById("bottomLayer");
    bottomLayerContext = bottomLayer.getContext("2d");

    shortPathLayer = document.getElementById("shortPathLayer");
    shortPathLayerContext = shortPathLayer.getContext("2d");

    dragging = false;
    shortPathLayer.addEventListener("mousedown", onCanvasMouseEvent);
    shortPathLayer.addEventListener("mouseup", onCanvasMouseEvent);
    shortPathLayer.addEventListener("mousemove", onCanvasMouseEvent);
    shortPathLayer.addEventListener("mousewheel", onCanvasMouseEvent);
    shortPathLayer.addEventListener("dblclick", onCanvasMouseEvent);

    shortPathLayer.addEventListener("touchstart", onCanvasTouchEvent);
    shortPathLayer.addEventListener("touchmove", onCanvasTouchEvent);
    shortPathLayer.addEventListener("touchend", onCanvasTouchEvent);

    setCanvasSize();
    setDrawParam();

    var _lat = getURLParameter("lat");
    var _lon = getURLParameter("lon");
    var _level = getURLParameter("level");
    var tl_tx, tl_ty;

    if (_lat == null || _lon == null || _level == null) {
        level = 15;
        tl_tx = 54876 / 2;
        tl_ty = 26764 / 2;

        setTLTxTy(tl_tx, tl_ty);
    } else {
        level = +_level;
        var lat = +_lat;
        var lon = +_lon;

        var coord = WGS84_to_px_py(lat, lon, level);

        setTLPixel(coord.px, coord.py)
        updateTLPixel(-window.innerWidth / 2 , -window.innerHeight / 2);
    }

    drawTiles();

    queryShortPath();
    drawShortPath();
}

function onWindowResize() {
    setCanvasSize();
    setDrawParam();
    drawTiles();
    drawShortPath();
}

function queryShortPath(url) {
    if(typeof url == "undefined") {
        pts = [];
        return;
    }
    var request = new XMLHttpRequest();
    request.open("GET", url, false);
    request.send();

    var data = request.responseText;
    console.log(data);
    var idx = data.indexOf("\n");
    var num = +data.substring(0, idx);
    var coords = data.substring(idx + 1, data.length - 1).split(" ");
    pts = new Array();

    for (var i = 0; i < num; i++) {
        pts.push ({lat: +coords[i * 2], lon: +coords[i * 2 + 1]})
    }
}

function drawShortPath() {
    var ctx = shortPathLayerContext;

    var coord = WGS84_to_px_py(pts[0].lat, pts[0].lon, level);

    ctx.clearRect(0, 0, shortPathLayer.width, shortPathLayer.height)
    ctx.strokeStyle = "#00B2FC";
    ctx.lineWidth = 10;
    ctx.shadowColor = "gray";
    ctx.shadowBlur = 10;
    ctx.lineCap = "round";
    ctx.beginPath();
    ctx.moveTo((coord.px - tl_px) * retina_ratio, (coord.py - tl_py) * retina_ratio);
    for (var i = 1; i < pts.length; i++) {
        coord = WGS84_to_px_py(pts[i].lat, pts[i].lon, level);

        var x, y;
        x = (coord.px - tl_px) * retina_ratio;
        y = (coord.py - tl_py) * retina_ratio;
        
        ctx.lineTo(x, y);
    }
    ctx.stroke();
}

function getLocation(name) {
    var prefix = "/place?name=";
    var url = prefix + name;

    var request = new XMLHttpRequest();
    request.open("GET", url, false);
    request.send();

    var data = request.responseText;
    data = JSON.parse(data);

    return data[0].geometry.location;
}

function search() {
    var name = document.getElementById("searchCtx").value;
    var idx = name.indexOf("-->");

    if(idx == -1) {
        var location = getLocation(name);
        var lat = location.lat, lon = location.lng;

        var coord = WGS84_to_px_py(lat, lon, level);

        setTLPixel(coord.px, coord.py);
        updateTLPixel(-window.innerWidth / 2 , -window.innerHeight / 2);
        drawTiles();
        drawShortPath();
        updataURL();
    } else {
        var name1 = name.substring(0, idx);
        var name2 = name.substring(idx + 3, name.length);

        var l1 = getLocation(name1), l2 = getLocation(name2);
        var params = ["lat1="+l1.lat, "lon1="+l1.lng, "lat2="+l2.lat, "lon2="+l2.lng, "t="+"driving"].join("&");
        var url = "/shortpath?" + params;

        queryShortPath(url);
        drawShortPath();
    }


}

window.addEventListener('load', onWindowLoad, false);
window.addEventListener('resize', onWindowResize, false);
