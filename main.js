var FaceDetection = require('ee-face-detection');
var v4l2camera = require("v4l2camera");
var cam = new v4l2camera.Camera("/dev/video0");

// create an instacne
var detector =  new FaceDetection();

function faceDetect(image){
	detector.detect(image, function(err, focalPoint) {
	    if (err) console.log('#Fail :(');
	    else if (focalPoint) console.log('Yeah, we got it! x %s, y %s :)', focalPoint.x, focalPoint.y);
	    else console.log('sorry, failed to detect any faces in this image ...');
	});
}
if (cam.configGet().formatName !== "MJPG") {
  console.log("NOTICE: MJPG camera required");
  process.exit(1);
}
cam.start();
cam.capture(function (success) {
  var frame = cam.frameRaw();
  faceDetect(frame);
});
