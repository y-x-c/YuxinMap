/**
 * Created by Orthocenter on 1/2/15.
 */

tiles_l = 256;
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

function onCanvasClick(event) {
    event.preventDefault();
    var coord = getMouseCoordFromEvent(event);
    if (event.type == "mousedown" || event.type == "touchstart") {
        //console.log("down")
        dragging = true;
        lastCoord = coord;
    }
    else if (event.type == "mousemove" || event.type == "touchmove") {
        if (dragging) {
            //console.log("move")
            updateTLPixel(-(coord.x - lastCoord.x), -(coord.y - lastCoord.y));
            lastCoord = coord;
            drawTiles();
        }
    }
    else if (event.type == "mouseup" || event.type == "touchend") {
        dragging = false;
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
    var ty = Math.floor(0.5 - log((1 + sinLat) / (1 - sinLat)) / (4 * Math.PI)) * n;

    return {tx: tx, ty: ty};
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

    tl_ox = Math.floor(tl_tx * tiles_l - px);
    tl_oy = Math.floor(tl_ty * tiles_l - py);
}

function updateTLPixel(dpx, dpy) {
    setTLPixel(tl_px + dpx * ratio, tl_py + dpy * ratio);
    console.log(tl_px, tl_py, tl_ox, tl_oy);
}

function setCanvasSize() {
    devicePixelRatio = window.devicePixelRatio || 1;
    backingStoreRatio = context.webkitBackingStorePixelRatio || 1;
    ratio = devicePixelRatio / backingStoreRatio;
    //ratio = 2;
    canvas.width = window.innerWidth * ratio;
    canvas.height = window.innerHeight * ratio;
    canvas.style.height = window.innerHeight + "px";
    canvas.style.width = window.innerWidth + "px";
}

function setDrawParam() {
    numTilesX = (canvas.width / tiles_l) + 1;
    numTilesY = (canvas.height / tiles_l) + 1;

    numTiles = numTilesX * numTilesY;
}

function drawTiles() {
    //var tiles = new Array();
    //var loadedTiles = 0;

    var tx_begin = tl_tx;
    var tx_end = tl_tx + numTilesX;
    var ty_begin = tl_ty;
    var ty_end = tl_ty + numTilesY;

    //context.fillStyle = "#f7f5f0";
    //context.fillRect(0, 0, canvas.width, canvas.height);

    for (var tx = tx_begin; tx < tx_end; tx++) {
        //tiles[tx - tx_begin] = new Array();

        for (var ty = ty_begin; ty < ty_end; ty++) {
            var tile_filename = tiles_filepath + tx + "_" + ty + "_" + level + ".png";

            var tile = new Image();
            tile.src = tile_filename;
            tile.tx = tx;
            tile.ty = ty;
            tile.onload = function () {
                //if (++loadedTiles >= numTiles) {
                //    drawImgs(tiles);
                //}
                drawTile(this);
            }
            tile.onerror = function () {
                //if (++loadedTiles >= numTiles) {
                //    drawImgs(tiles);
                //}
            }

            //tiles[tx - tx_begin].push(tile);
        }
    }

    function drawTile(tile) {
        context.drawImage(tile, tiles_l * (tile.tx - tl_tx) + tl_ox, tiles_l * (tile.ty - tl_ty) + tl_oy);
    }

    function drawImgs(imgs) {
        context.fillStyle = "#f7f5f0";
        context.fillRect(0, 0, canvas.width, canvas.height);
        for (var tx = 0; tx < numTilesX; tx++) {
            for (var ty = 0; ty < numTilesY; ty++) {
                if (imgs[tx][ty].width) context.drawImage(imgs[tx][ty], tiles_l * tx + tl_ox, tiles_l * ty + tl_oy);
            }
        }
    }
}

function onload() {
    canvas = document.getElementById("main");
    context = canvas.getContext("2d");

    dragging = false;
    canvas.addEventListener("mousedown", onCanvasClick);
    canvas.addEventListener("mouseup", onCanvasClick);
    canvas.addEventListener("mousemove", onCanvasClick);
    canvas.addEventListener("touchstart", onCanvasClick);
    canvas.addEventListener("touchup", onCanvasClick);
    canvas.addEventListener("touchend", onCanvasClick);

    setCanvasSize();
    setDrawParam();
    level = 17;
    n = Math.pow(2, level);
    var tl_tx = 109772;
    var tl_ty = 53548;

    setTLTxTy(tl_tx, tl_ty);

    drawTiles();
}

window.addEventListener('load', onload, false);