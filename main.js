
const exec = require('child_process').exec;
const fs = require("fs");
const cv = require("opencv");
const SerialPort = require("serialport");

var port ;
var vid;
var frameWidth = 160;
var frameHeight = 120;
var cx = frameWidth/2;
var cy = frameHeight/2;
var received = "";
var isCapturing = false;
var lastTime = 0;
exec('ls /dev/video*', (error, stdout, stderr) => {
	if (error) {
		console.error(`exec error: ${error}`);
		return;
	}
	var device = stdout.split("\n").join("");
	vid = new cv.VideoCapture(device.split("video")[1]*1);
	vid.setWidth(frameWidth);
	vid.setHeight(frameHeight);
	port = new SerialPort("/dev/ttyS0",{baudRate:115200},function(err){
		if(err){
			return;
		}
		port.write(new Buffer("reset,0,0\n"));
		port.on('data',function(data){
			var res = data.toString();
			received+=res;
			console.log(received);
			if(received.indexOf("ok")>-1){
				isCapturing = false;	
				received="";
			}
		});

	});
	loop();
});
function loop(){
	var ct = new Date().getTime();
	if(ct-lastTime>1000||!isCapturing){	
		//console.log('capturing');
		lastTime = ct;
		isCapturing = true;
		vid.read(function(err, im){
			if(err){
				console.log(err);
				return;
			}
			im.detectObject("data/haarcascade_frontalface_alt.xml", {}, function(err, faces){
				if(err){
					return;
				}
				if(faces){
					for (var i=0;i<faces.length; i++){
						var face = faces[i];
						var faceX = (face.x + face.width/2-cx)*90/frameWidth;
						var faceY = (face.y + face.height/2-cy)*90/frameHeight;
						var faceW = face.width;
						var faceH = face.height;
						move(faceX,faceY,faceW,faceH);
						break;
					}
					if(faces.length ==0){
						isCapturing = false;
					}
				}else{
				}
			});
		})
	}
	setTimeout(loop,25);
}
function move(faceX,faceY,faceW,faceH){
	console.log("face,"+Math.round(-faceX)+","+Math.round(faceY)+","+Math.round(faceW)+","+Math.round(faceH));
	port.write(new Buffer("face,"+Math.round(-faceX)+","+Math.round(faceY)+","+Math.round(faceW)+","+Math.round(faceH)+"\n"));
}
process.on("SIGINT",function(){
	if(port){
		port.close();
	}
	if(vid){
		vid.release();
	}
	process.exit();
});
