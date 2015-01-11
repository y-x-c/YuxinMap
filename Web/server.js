/**
 * Created by Orthocenter on 1/4/15.
 */
var fs = require('fs');
var express = require('express');
var bodyParser = require('body-parser');
var shelljs = require('shelljs/global');
var https = require('https');
var app = express();
var et = require('eviltransform');

app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());

function queryPlace(name, lat, lon, s) {
    var prefix = 'https://maps.googleapis.com/maps/api/place/textsearch/json?query=';
    var key = '&key=AIzaSyCVrW2q0C4cdAucpFSRSQ44V0pgYXXVGsI';
    var location = '&location=' + lat + ',' + lon + '&radius=5000';
    var url = prefix + name + key + location;

    console.log(url);
    https.get(url, function(res) {
        //s.send(JSON.stringify(res.result));
        var output = "";

        res.on('data', function (chunk) {
            output += chunk;
            //console.log(output)
        });

        res.on('end', function() {
            //console.log(typeof res);
            output = JSON.parse(output);

            for(var i = 0; i < output["results"].length; i++) {
                var lat = output["results"][i].geometry.location.lat;
                var lng = output["results"][i].geometry.location.lng;

                var ret = et.gcj2wgs_exact(lat, lng);

                output["results"][i].geometry.location.lat = ret.lat;
                output["results"][i].geometry.location.lng = ret.lng;
            }

            s.send(output["results"]);
        });
    });
}

app.get('/place', function (req, res) {
    var name = req.query.name;
    var lat = req.query.lat;
    var lon = req.query.lon;

    var place = queryPlace(name, lat, lon, res);

    //res.send(place);
    //console.log(place);
    //res.send('Query Place: ' + name);
    //console.log(ls(''));
    //res.sendFile(__dirname + '/map.css');
});

app.get('/nodeid', function (req, res) {
    var lat = req.query.lat;
    var lon = req.query.lon;

    res.send(getNearestNode(lat, lon));
});

app.get('/shortPath', function (req, res) {
    var lat1 = req.query.lat1;
    var lon1 = req.query.lon1;
    var lat2 = req.query.lat2;
    var lon2 = req.query.lon2;
    var t = req.query.t;

    var id1 = getNearestNode(lat1, lon1, t);
    var id2 = getNearestNode(lat2, lon2, t);

    console.log(id1, id2);

    var appPath = __dirname + '/Apps/ShortPath';
    var confPath = __dirname + '/Apps/ShortPath.conf';
    var dataPath = __dirname + "/Apps/Data/map_shanghai.osm";
    var date = new Date();
    var outputPath = __dirname + "/Apps/SP" + date.toLocaleTimeString() + date.getMilliseconds();
    console.log(confPath);
    var cmd = [appPath, dataPath, confPath, t, id1, id2, outputPath].join(" ");

    if (exec(cmd).code == 0) {
        var data = fs.readFileSync(outputPath, "utf-8");
        res.send(data);
        exec("rm " + outputPath);
    }
});

function getNearestNode(lat, lon, t) {
    var appPath = __dirname + '/Apps/NearestNode';
    var confPath = __dirname + '/Apps/NearestNode.conf';
    var dataPath = __dirname + "/Apps/Data/map_shanghai.osm";
    var date = new Date();
    var outputPath = __dirname + "/Apps/NN" + date.toLocaleTimeString() + date.getMilliseconds() + Math.random();

    if (exec([appPath, dataPath, confPath, outputPath, lat, lon, t].join(' ')).code == 0) {
        var data = fs.readFileSync(outputPath, "utf-8");
        console.log(data);
        exec("rm " + outputPath);
        return data;
    }
}

app.use(express.static(__dirname));

app.listen(3000, "0.0.0.0", function () {
    console.log('hello');
});